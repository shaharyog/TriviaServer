#include <iostream>
#include "roomAdminRequestHandler.h"

bool RoomAdminRequestHandler::isRequestRelevant(const RequestInfo &request) {
    return request.requestId == RequestId::CLOSE_ROOM_REQUEST ||
           request.requestId == RequestId::START_GAME_REQUEST ||
           request.requestId == RequestId::GET_ROOM_STATE_REQUEST ||
           request.requestId == RequestId::EXIT;
}

RequestResult RoomAdminRequestHandler::handleRequest(const RequestInfo &request) {

    if (!isRequestRelevant(request))
        return Error(ErrorType::InvalidRequest, "Request is not relevant").toRequestResult<RoomAdminRequestHandler>(__func__,
                                                                                                          _userEndpoint);

    switch (request.requestId) {
        case RequestId::CLOSE_ROOM_REQUEST:
            return closeRoom();
        case RequestId::START_GAME_REQUEST:
            return startGame();
        case RequestId::GET_ROOM_STATE_REQUEST:
            return getRoomState();
        case RequestId::EXIT:
            return closeRoomAndExit();
        default:
            return Error(ErrorType::InvalidRequest, "Request is not relevant").toRequestResult<RoomAdminRequestHandler>(__func__,
                                                                                                              _userEndpoint);
    }
}

RequestResult RoomAdminRequestHandler::closeRoom() {
    auto &roomManager = _requestHandlerFactory.getRoomManager();

    log<RoomAdminRequestHandler>(__func__,
                                 "Admin '" + _user.username + "' closed room '" + _room.getMetadata().name + "' with uuid '" + _room.getMetadata().uuid + "'", true,
                                 _userEndpoint);

    roomManager.deleteRoom(_room.getMetadata().uuid);

    return RequestResult{JsonSerializer::serializeResponse(CloseRoomResponse{true}),
                         _requestHandlerFactory.createMenuRequestHandler(_user, _userEndpoint)};
}

RequestResult RoomAdminRequestHandler::startGame() {
    if (_room.getAllUsers().size() <= 1)
        return Error(ErrorType::NotEnoughPlayers).toRequestResult<RoomAdminRequestHandler>(__func__, _userEndpoint);

    const auto res = _requestHandlerFactory.getGameManager().createGame(_room);
    if (res.isError())
        return res.error().toRequestResult<RoomAdminRequestHandler>(__func__, _userEndpoint);

    _room.setIsActive(true);    // set the room as active only after the game has been created
    log<RoomAdminRequestHandler>(__func__,
                                 "Admin '" + _user.username + "' started game '" + _room.getMetadata().name + "' with uuid '" + _room.getMetadata().uuid + "'", true,
                                 _userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(StartGameResponse{true}), _requestHandlerFactory.createGameRequestHandler(res.value(), _user, _userEndpoint)};
}

RequestResult RoomAdminRequestHandler::getRoomState() {
    // because the admin got the state of the room, the room mus be present, thus we can use roomState without checking it
    const auto &roomState = _requestHandlerFactory.getRoomManager().getRoomState(_room.getMetadata().uuid);

    const auto players = _requestHandlerFactory.getUsersManager().transformLoggedUsersToPlayers(_room.getAllUsers());
    if (players.isError())
        return players.error().toRequestResult<RoomAdminRequestHandler>(__func__, _userEndpoint);

    const auto response = GetRoomStateResponse{true, roomState.value().roomData.isActive, players.value(),
                                               roomState.value().roomData.questionCount,
                                               roomState.value().roomData.timePerQuestion,
                                               roomState.value().roomData.maxPlayers,
                                               false};
    return RequestResult{JsonSerializer::serializeResponse(response)};
}

RequestResult RoomAdminRequestHandler::closeRoomAndExit() {
    log<RoomAdminRequestHandler>(__func__, "Admin '" + _user.username + "' has disconnected, closing room '" + _room.getMetadata().name + "' with uuid '" + _room.getMetadata().uuid + "', logging him out",
                       false, _userEndpoint);
    _requestHandlerFactory.getLoginManager().logout(_user.username);
    return closeRoom();
}
