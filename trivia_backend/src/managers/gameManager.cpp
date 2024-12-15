#include <thread>
#include "gameManager.h"

Result<std::reference_wrapper<Game>> GameManager::createGame(const Room &room) {

    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    const auto questionsDbRes = db_ptr->getQuestions(room.getMetadata().questionCount);

    if (questionsDbRes.isError())
        return questionsDbRes.error();

    const auto questionsDb = questionsDbRes.value();
    if (questionsDb.size() != room.getMetadata().questionCount)
        return Error(ErrorType::NotFound, "Not enough questions found");

    std::vector<Question> questions;
    std::transform(questionsDb.cbegin(), questionsDb.cend(), std::back_inserter(questions),
                   [](const auto &question) {
                       return Question(question);
                   });

    std::lock_guard lock(_gamesMutex);
    auto gamePtr = std::make_unique<Game>(questions, room.getAllUsers(), room.getMetadata().uuid,
                                          room.getMetadata().timePerQuestion);
    _games.push_back(std::move(gamePtr));

    return std::ref(*_games.back());
}

std::optional<Error> GameManager::removePlayerFromGame(Game &game, const LoggedUser &user) {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    if (!game.isFinished())
        game.punishPlayer(user);
    game.removePlayer(user);
    std::optional<Error> submitResult;

    // update the db immediately if a player left during the game.
    if (!game.isFinished()) {
        game.markUserResultsAsSubmittedToDB(user);
        const auto gameData = game.getPlayersResults().at(user);
        submitResult = db_ptr->submitGameStatistics(gameData, user.username);
    }

    // submit all players stats to db whenever a user left,
    // the score won't be submitted twice as the gameData includes a boolean that
    // tells weather the score was already submitted.
    if (game.isFinished())
        submitAllGameStatsToDB(game.getUuid());

    if (game.getOnlinePlayers().empty()) {
        std::lock_guard lock(_gamesMutex);
        _games.erase(std::find_if(_games.begin(), _games.end(), [&game](const auto &ptr) { return *ptr == game; }));
    }

    if (submitResult.has_value())
        return submitResult.value();

    return std::nullopt;
}


Game &GameManager::getGameById(const std::string &id) {
    std::shared_lock lock(_gamesMutex);
    return *_games.at(
            std::find_if(_games.cbegin(), _games.cend(), [&](const auto &game) { return game->getUuid() == id; }) -
            _games.cbegin());
}

void GameManager::submitAllGameStatsToDB(const std::string &gameId) {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return;

    auto &game = getGameById(gameId);
    std::shared_lock lock(_gamesMutex);
    const auto playersResults = game.getPlayersResults();

    for (const auto &player: playersResults) {
        const auto &gameData = player.second;
        if (gameData.isScoreSubmittedToDB)  // if the score of the user was already submitted, skip
            continue;
        game.markUserResultsAsSubmittedToDB(player.first);
        const auto _ = db_ptr->submitGameStatistics(gameData, player.first.username);
        // we have nothing to do here if we failed to submit the game statistics, just ignore it
    }
}

std::optional<Error> GameManager::removePlayerFromGame(const std::string &gameId, const LoggedUser &user) {
    std::shared_lock lock(_gamesMutex);
    auto &game = getGameById(gameId);
    lock.unlock();
    return removePlayerFromGame(game, user);
}

