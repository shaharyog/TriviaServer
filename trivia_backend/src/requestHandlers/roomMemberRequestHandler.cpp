#include "roomMemberRequestHandler.h"

bool RoomMemberRequestHandler::isRequestRelevant(const RequestInfo &request) {
    return request.requestId == RequestId::LEAVE_ROOM_REQUEST ||
           request.requestId == RequestId::GET_ROOM_STATE_REQUEST ||
           request.requestId == RequestId::EXIT;
}

RequestResult RoomMemberRequestHandler::handleRequest(const RequestInfo &request) {
    if (!isRequestRelevant(request))
        return Error(ErrorType::InvalidRequest, "Request is not relevant").toRequestResult<RoomMemberRequestHandler>(__func__,
                                                                                                          _userEndpoint);

    if (request.requestId == RequestId::LEAVE_ROOM_REQUEST) {
        return leaveRoom();
    } else if (request.requestId == RequestId::GET_ROOM_STATE_REQUEST) {
        return getRoomState();
    } else if (request.requestId == RequestId::EXIT) {
        return leaveRoomAndExit();
    }
    return RequestResult{};
}

RequestResult RoomMemberRequestHandler::leaveRoom() {
    const auto &roomState = _requestHandlerFactory.getRoomManager().getRoomState(_roomUuid);

    if (!roomState.has_value()) {
        log<RoomMemberRequestHandler>(__func__, "Room '" + _roomName + "' was closed. Member '" + _user.username + "' is transferred back to menu", true,
                           _userEndpoint);
        return RequestResult{JsonSerializer::serializeResponse(LeaveRoomResponse{false}),
                             _requestHandlerFactory.createMenuRequestHandler(
                                     _user, _userEndpoint
                             )};
    }

    // if the game has already started, remove the player from the room and game.
    if (roomState.value().roomData.isActive) {
        _requestHandlerFactory.getGameManager().removePlayerFromGame(_roomUuid, _user);
        log<RoomMemberRequestHandler>(__func__, "Member '" + _user.username + "' left room '" + _roomName + "' after the game has started, punishing him.",
                           true,
                           _userEndpoint);

        _room.removeUser(_user);
        return RequestResult{JsonSerializer::serializeResponse(LeaveRoomResponse{true}),
                             _requestHandlerFactory.createMenuRequestHandler(
                                     _user, _userEndpoint
                             )};

    }

    // at last remove the player from the room regularly
    _room.removeUser(_user);

    log<RoomMemberRequestHandler>(__func__,
                       "Member '" + _user.username + "' left room '" + _roomName + "' with uuid: '" + _room.getMetadata().uuid + "'", true,
                       _userEndpoint);
    const auto response = LeaveRoomResponse{true};
    return RequestResult{JsonSerializer::serializeResponse(response), _requestHandlerFactory.createMenuRequestHandler(
            _user, _userEndpoint
    )};
}

RequestResult RoomMemberRequestHandler::getRoomState() {
    const auto &roomState = _requestHandlerFactory.getRoomManager().getRoomState(_roomUuid);

    if (!roomState.has_value()) {
        log<RoomMemberRequestHandler>(__func__, "Room '" + _roomName + "' was closed. Member '" + _user.username + "' is transferred back to menu", true,
                           _userEndpoint);
        return RequestResult{JsonSerializer::serializeResponse(
                GetRoomStateResponse{true, false, std::vector<Player>(), 0, 0, 0, true}),
                             _requestHandlerFactory.createMenuRequestHandler(
                                     _user, _userEndpoint
                             )};
    }

    const auto players = _requestHandlerFactory.getUsersManager().transformLoggedUsersToPlayers(
            roomState.value().users);
    if (players.isError())
        return players.error().toRequestResult<RoomMemberRequestHandler>(__func__, _userEndpoint);
    const auto response = GetRoomStateResponse{true, roomState.value().roomData.isActive, players.value(),
                                               roomState.value().roomData.questionCount,
                                               roomState.value().roomData.timePerQuestion,
                                               roomState.value().roomData.maxPlayers,
                                               false};
    if (response.hasGameBegun) {
        log<RoomMemberRequestHandler>(__func__, "Game '" + _roomName + "' has already started. Member '" + _user.username + "' is transferred to game", true,
                           _userEndpoint);

        // retrieve the game for this room (game id and room id are identical)
        auto &game = _requestHandlerFactory.getGameManager().getGameById(_roomUuid);
        return RequestResult{JsonSerializer::serializeResponse(response),
                             _requestHandlerFactory.createGameRequestHandler(
                                     game, _user, _userEndpoint
                             )};
    }

    return RequestResult{JsonSerializer::serializeResponse(response)};
}

RequestResult RoomMemberRequestHandler::leaveRoomAndExit() {
    _requestHandlerFactory.getLoginManager().logout(_user.username);
    log<RoomMemberRequestHandler>(__func__, "Member '" + _user.username + "' has disconnected, leaving room '" + _roomName + "' with uuid: '" + _roomUuid + "', logging him out",
                       false,
                       _userEndpoint);
    return leaveRoom();
}

