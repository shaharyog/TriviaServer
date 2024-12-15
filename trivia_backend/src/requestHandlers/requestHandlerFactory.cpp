#include "requestHandlerFactory.h"
#include "loginRequestHandler.h"
#include "menuRequestHandler.h"
#include "roomMemberRequestHandler.h"
#include "roomAdminRequestHandler.h"
#include "gameRequestHandler.h"
#include "verificationHandler.h"


std::unique_ptr<IRequestHandler> RequestHandlerFactory::createLoginRequestHandler(const Endpoint &endpoint) {
    return std::make_unique<LoginRequestHandler>(*this, endpoint);
}

std::unique_ptr<IRequestHandler>
RequestHandlerFactory::createMenuRequestHandler(const LoggedUser &user, const Endpoint &endpoint) {
    return std::make_unique<MenuRequestHandler>(*this, user, endpoint);
}

LoginManager &RequestHandlerFactory::getLoginManager() {
    return _loginManager;
}

RoomManager &RequestHandlerFactory::getRoomManager() {
    return _roomManager;
}

StatisticsManager &RequestHandlerFactory::getStatisticsManager() {
    return _statisticsManager;
}

usersManager &RequestHandlerFactory::getUsersManager() {
    return _usersManager;
}

GameManager &RequestHandlerFactory::getGameManager() {
    return _gameManager;
}

std::unique_ptr<IRequestHandler>
RequestHandlerFactory::createRoomMemberRequestHandler(Room &room, const LoggedUser &user,
                                                      const Endpoint &endpoint) {
    return std::make_unique<RoomMemberRequestHandler>(room, user, *this, endpoint);
}

std::unique_ptr<IRequestHandler>
RequestHandlerFactory::createRoomAdminRequestHandler(Room &room, const LoggedUser &user, const Endpoint &endpoint) {
    return std::make_unique<RoomAdminRequestHandler>(*this, user, room, endpoint);
}

std::unique_ptr<IRequestHandler>
RequestHandlerFactory::createGameRequestHandler(Game &game, const LoggedUser &user, const Endpoint &endpoint) {
    return std::make_unique<GameRequestHandler>(game, *this, user, endpoint);
}

std::unique_ptr<IRequestHandler>
RequestHandlerFactory::createVerificationRequestHandler(const std::string &email, const std::string &verificationCode,
                                                        const std::string &username,
                                                        const Endpoint &endpoint) {
    return std::make_unique<VerificationRequestHandler>(*this, email, verificationCode, username, endpoint);
}


