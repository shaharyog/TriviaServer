#pragma once

#include <memory>
#include "usersManager.h"
#include "../utils/databaseAccess/IDatabasae.h"
#include "game.h"
#include "room.h"

class GameManager
{
public:
    explicit GameManager(const std::shared_ptr<IDatabase> &database) : _database(database) {};
    ~GameManager() = default;

    Result<std::reference_wrapper<Game>> createGame(const Room& room);
    std::optional<Error> removePlayerFromGame(Game& game, const LoggedUser& user);
    std::optional<Error> removePlayerFromGame(const std::string& gameId, const LoggedUser& user);
    Game& getGameById(const std::string& id);

    void submitAllGameStatsToDB(const std::string& gameId);
private:

    std::weak_ptr<IDatabase> _database;
    std::vector<std::unique_ptr<Game>> _games;
    mutable std::shared_mutex _gamesMutex;

};