#pragma once
#include <memory>
#include "IRequestHandler.h"
#include "../managers/loginManager.h"
#include "../managers/roomManager.h"
#include "../managers/statisticsManager.h"
#include "../managers/usersManager.h"
#include "../managers/gameManager.h"

class RequestHandlerFactory {
public:
    explicit RequestHandlerFactory(const std::shared_ptr<IDatabase> &database) : _database(database),
                                                                                 _loginManager(database),
                                                                                 _statisticsManager(database),
                                                                                 _usersManager(database),
                                                                                 _gameManager(database) {}

    ~RequestHandlerFactory() = default;

    std::unique_ptr<IRequestHandler> createLoginRequestHandler(const Endpoint& endpoint);
    std::unique_ptr<IRequestHandler> createMenuRequestHandler(const LoggedUser& user, const Endpoint& endpoint);
    std::unique_ptr<IRequestHandler> createRoomMemberRequestHandler(Room& room, const LoggedUser& user, const Endpoint& endpoint);
    std::unique_ptr<IRequestHandler> createRoomAdminRequestHandler(Room& room, const LoggedUser& user, const Endpoint& endpoint);
    std::unique_ptr<IRequestHandler> createGameRequestHandler(Game& game, const LoggedUser& user, const Endpoint& endpoint);
    std::unique_ptr<IRequestHandler> createVerificationRequestHandler(const std::string& email, const std::string& verificationCode, const std::string &username, const Endpoint& endpoint);

    LoginManager &getLoginManager();
    RoomManager &getRoomManager();
    StatisticsManager &getStatisticsManager();
    usersManager &getUsersManager();
    GameManager &getGameManager();
private:
    LoginManager _loginManager;
    RoomManager _roomManager;
    StatisticsManager _statisticsManager;
    usersManager _usersManager;
    GameManager _gameManager;
    std::weak_ptr<IDatabase> _database;
};