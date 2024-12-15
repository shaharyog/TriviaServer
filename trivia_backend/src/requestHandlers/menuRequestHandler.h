#pragma once

#include <utility>

#include "IRequestHandler.h"
#include "requestHandlerFactory.h"

class RequestHandlerFactory;

class MenuRequestHandler : public IRequestHandler {
public:
    explicit MenuRequestHandler(RequestHandlerFactory &requestHandlerFactory, LoggedUser user, Endpoint userEndpoint)
            : _requestHandlerFactory(requestHandlerFactory), _user(std::move(user)), _userEndpoint(std::move(userEndpoint)) {}

    ~MenuRequestHandler() override = default;

    bool isRequestRelevant(const RequestInfo &request) override;
    RequestResult handleRequest(const RequestInfo &request) override;

private:
    RequestResult logout();
    RequestResult getRooms();
    RequestResult getPlayersInRoom(const RequestInfo &request);
    RequestResult getPersonalStatistics();
    RequestResult getHighScores();
    RequestResult joinRoom(const RequestInfo &request);
    RequestResult createRoom(const RequestInfo &request);
    RequestResult getUserData();
    RequestResult updateUserData(const RequestInfo &request);
    static bool isValidRoomDetails(const RoomData &roomData);

    Endpoint _userEndpoint;
    LoggedUser _user;
    RequestHandlerFactory &_requestHandlerFactory;
};
