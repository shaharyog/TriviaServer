#include <thread>
#include "game.h"

Result<unsigned int>
Game::submitAnswer(const LoggedUser &user, unsigned int answerIndex, unsigned int questionIndex) {
    const auto timeSinceGameStart = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - _gameStartTime
    );
    const auto currentQuestionIndex = static_cast<unsigned int>(timeSinceGameStart.count() /
                                                                (_timePerQuestion + 5));
    if (answerIndex > 4)
        return Error("Invalid answer index, use only our client!");

    if (questionIndex != currentQuestionIndex)
        return Error("Answer submitted for wrong question");

    const auto currQuestionReleaseTime = (currentQuestionIndex+1)*(_timePerQuestion + 5) - 5;

    if (timeSinceGameStart.count() >= currQuestionReleaseTime)
        return Error("Answer already released"); // answer already released, player submit answer after release.

    // sleep until (game start time) + currQuestionReleaseTime
    const auto sleepUntilReleased = _gameStartTime + std::chrono::seconds(currQuestionReleaseTime);
    std::this_thread::sleep_until(sleepUntilReleased);

    const auto &answerTimeInSeconds = _timePerQuestion - (currQuestionReleaseTime - timeSinceGameStart.count());
    _players.at(user).submitAnswer(answerIndex, questionIndex, static_cast<unsigned int>(answerTimeInSeconds));
    return _questions[questionIndex].getCorrectAnswerIndex();
}

void Game::removePlayer(const LoggedUser &user) {
    std::lock_guard lock(_onlinePlayersMutex);
    _onlinePlayers.erase(std::remove(_onlinePlayers.begin(), _onlinePlayers.end(), user), _onlinePlayers.end());
}

Game::Game(const std::vector<Question> &questions, std::vector<LoggedUser> players, std::string uuid,
           unsigned int timePerQuestion) :  _timePerQuestion(timePerQuestion),
                                           _uuid(std::move(uuid)), _questions(questions),
                                           _onlinePlayers(players) {

    _gameStartTime = std::chrono::steady_clock::now();
    for (auto &player: players)
        _players.emplace(player, GameData(questions, timePerQuestion));
}

bool Game::operator==(const Game &other) const {
    return _uuid == other._uuid;
}

std::map<LoggedUser, GameData> Game::getPlayersResults() const {
    std::shared_lock lock(_playersMutex);
    return _players;
}

std::pair<unsigned int, Question> Game::getCurrentQuestion() const {
    const auto timeSinceGameStart = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - _gameStartTime
    );
    const auto currentQuestionIndex = static_cast<unsigned int>(timeSinceGameStart.count() /
                                                                        (_timePerQuestion + 5));
    return std::make_pair(currentQuestionIndex, _questions[currentQuestionIndex]);
}

std::vector<LoggedUser> Game::getOnlinePlayers() const {
    std::shared_lock lock(_onlinePlayersMutex);
    return _onlinePlayers;
}

void Game::punishPlayer(const LoggedUser &user) {
    std::unique_lock lock(_playersMutex);
    _players.at(user).isPunished = true;
}

void Game::markUserResultsAsSubmittedToDB(const LoggedUser &user) {
    std::unique_lock lock(_onlinePlayersMutex);
    _players.at(user).isScoreSubmittedToDB = true;
}

bool Game::isFinished() const {
    const auto timeSinceGameStart = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - _gameStartTime
    );
    return timeSinceGameStart.count() >= _questions.size() * (_timePerQuestion + 5);
}
