#pragma once

#include <utility>

#include "IRequestHandler.h"
#include "../managers/room.h"
#include "requestHandlerFactory.h"

class RoomMemberRequestHandler : public IRequestHandler {
public:
    RoomMemberRequestHandler(Room &room, LoggedUser user, RequestHandlerFactory &requestHandlerFactory,
                             Endpoint userEndpoint) : _room(room),
                                                      _roomUuid(room.getMetadata().uuid),
                                                      _roomName(room.getMetadata().name),
                                                      _user(std::move(user)),
                                                      _requestHandlerFactory(
                                                              requestHandlerFactory),
                                                      _userEndpoint(std::move(userEndpoint)) {}

    ~RoomMemberRequestHandler() override = default;


    RequestResult handleRequest(const RequestInfo &request) override;

    bool isRequestRelevant(const RequestInfo &request) override;

private:
    RequestResult leaveRoom();
    RequestResult getRoomState();
    RequestResult leaveRoomAndExit();

    Room &_room;
    std::string _roomUuid;
    std::string _roomName;
    LoggedUser _user;
    Endpoint _userEndpoint;
    RequestHandlerFactory &_requestHandlerFactory;
};