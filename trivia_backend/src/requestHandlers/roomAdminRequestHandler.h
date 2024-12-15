#pragma once

#include "../managers/room.h"
#include "../managers/roomManager.h"
#include "requestHandlerFactory.h"

class RoomAdminRequestHandler : public IRequestHandler {
public:
    explicit RoomAdminRequestHandler(RequestHandlerFactory &requestHandlerFactory, LoggedUser user, Room &room,
                                     Endpoint userEndpoint)
            : _requestHandlerFactory(requestHandlerFactory), _user(std::move(user)), _room(room),
              _userEndpoint(std::move(userEndpoint)) {}
    ~RoomAdminRequestHandler() override = default;

    bool isRequestRelevant(const RequestInfo &request) override;

    RequestResult handleRequest(const RequestInfo &request) override;

private:

    RequestResult closeRoom();
    RequestResult startGame();
    RequestResult getRoomState();
    RequestResult closeRoomAndExit();

    Room &_room;
    LoggedUser _user;
    Endpoint _userEndpoint;
    RequestHandlerFactory &_requestHandlerFactory;
};