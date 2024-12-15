#include <algorithm>
#include "menuRequestHandler.h"
#include "../utils/marshaling/jsonDeserializer.h"

bool MenuRequestHandler::isRequestRelevant(const RequestInfo &request) {
    return request.requestId == RequestId::LOGOUT_REQUEST || request.requestId == RequestId::GET_ROOMS_REQUEST ||
           request.requestId == RequestId::GET_PLAYERS_IN_ROOM_REQUEST ||
           request.requestId == RequestId::GET_HIGHSCORES_REQUEST ||
           request.requestId == RequestId::GET_PERSONAL_STATS_REQUEST ||
           request.requestId == RequestId::JOIN_ROOM_REQUEST ||
           request.requestId == RequestId::CREATE_ROOM_REQUEST ||
           request.requestId == RequestId::EXIT ||
           request.requestId == RequestId::GET_USER_DATA_REQUEST ||
           request.requestId == RequestId::UPDATE_USER_DATA_REQUEST;
}

RequestResult MenuRequestHandler::handleRequest(const RequestInfo &request) {
    if (!isRequestRelevant(request))
        return Error(ErrorType::InvalidRequest, "Request is not relevant").toRequestResult<MenuRequestHandler>(__func__,
                                                                                                          _userEndpoint);

    switch (request.requestId) {
        case RequestId::LOGOUT_REQUEST:
            return logout();
        case RequestId::GET_ROOMS_REQUEST:
            return getRooms();
        case RequestId::GET_PLAYERS_IN_ROOM_REQUEST:
            return getPlayersInRoom(request);
        case RequestId::GET_HIGHSCORES_REQUEST:
            return getHighScores();
        case RequestId::GET_PERSONAL_STATS_REQUEST:
            return getPersonalStatistics();
        case RequestId::JOIN_ROOM_REQUEST:
            return joinRoom(request);
        case RequestId::CREATE_ROOM_REQUEST:
            return createRoom(request);
        case RequestId::GET_USER_DATA_REQUEST:
            return getUserData();
        case RequestId::UPDATE_USER_DATA_REQUEST:
            return updateUserData(request);
        case RequestId::EXIT:
            // if the User has exited, we need to log him out
            logout();
            return RequestResult{{}, nullptr};
        default:
            return Error(ErrorType::InvalidRequest, "Request is not relevant").toRequestResult<MenuRequestHandler>(__func__,
                                                                                                              _userEndpoint);
    }
}

RequestResult MenuRequestHandler::logout() {
    auto &loginManager = _requestHandlerFactory.getLoginManager();
    loginManager.logout(_user.username);

    LogoutResponse response{true};
    log<MenuRequestHandler>(__func__, "User '" + _user.username + "' logged out", true, _userEndpoint);
    auto newHandler = _requestHandlerFactory.createLoginRequestHandler(_userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(response), std::move(newHandler)};
}

RequestResult MenuRequestHandler::getRooms() {
    auto &roomManager = _requestHandlerFactory.getRoomManager();
    auto roomsData = roomManager.getRooms();
    std::vector<RoomDataResponse> rooms;
    for (auto &room: roomsData) {
        const auto &roomRes = roomManager.getRoom(room.uuid);
        if (roomRes.isError()) {
            log<MenuRequestHandler>(__func__, "User '" + _user.username + "' failed to get rooms", false, _userEndpoint);
            continue;
        }
        const auto playersInRoom = _requestHandlerFactory.getUsersManager().transformLoggedUsersToPlayers(
                roomRes.value().get().getAllUsers());
        if (playersInRoom.isError()) {
            log<MenuRequestHandler>(__func__, "User '" + _user.username + "' failed to get rooms", false,
                               _userEndpoint);
            continue;
        }

        rooms.push_back(RoomDataResponse{room, room.isActive && _requestHandlerFactory.getGameManager().getGameById(room.uuid).isFinished(), playersInRoom.value()});
    }
    GetRoomsResponse response{true, std::move(rooms)};
    return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
}

RequestResult MenuRequestHandler::getPlayersInRoom(const RequestInfo &request) {
    auto getPlayersRequest = JsonDeserializer::deserializeGetPlayersInRoomRequest(request.buffer);
    if (getPlayersRequest.isError())
        return getPlayersRequest.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    auto &roomManager = _requestHandlerFactory.getRoomManager();
    auto roomRes = roomManager.getRoom(getPlayersRequest.value().roomId);
    if (roomRes.isError()) {
        log<MenuRequestHandler>(__func__, "User '" + _user.username + "' failed to get players in room", false,
                           _userEndpoint);
        GetPlayersInRoomResponse response{false, {}};
        return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
    }

    const auto users = roomRes.value().get().getAllUsers();

    const auto players = _requestHandlerFactory.getUsersManager().transformLoggedUsersToPlayers(users);
    if (players.isError()) {
        log<MenuRequestHandler>(__func__, "User '" + _user.username + "' failed to get players in room", false,
                           _userEndpoint);
        GetPlayersInRoomResponse response{false, {}};
        return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
    }

    GetPlayersInRoomResponse response{true, std::move(players.value())};
    return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
}

RequestResult MenuRequestHandler::getPersonalStatistics() {
    auto &statisticsManager = _requestHandlerFactory.getStatisticsManager();
    auto statisticsRes = statisticsManager.getUserStatistics(_user.username);
    if (statisticsRes.isError())
        return statisticsRes.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    GetPersonalStatsResponse response{true, statisticsRes.value()};

    log<MenuRequestHandler>(__func__, "User '" + _user.username + "' requested personal statistics", true,
                       _userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
}

RequestResult MenuRequestHandler::getHighScores() {
    auto &statisticsManager = _requestHandlerFactory.getStatisticsManager();
    auto highScoresRes = statisticsManager.getHighScores();
    if (highScoresRes.isError())
        return highScoresRes.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    GetHighScoresResponse response{true, highScoresRes.value()};

    return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
}

RequestResult MenuRequestHandler::joinRoom(const RequestInfo &request) {
    auto &roomManager = _requestHandlerFactory.getRoomManager();
    auto joinRoomRequest = JsonDeserializer::deserializeJoinRoomRequest(request.buffer);
    if (joinRoomRequest.isError())
        return joinRoomRequest.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    auto roomRes = roomManager.getRoom(joinRoomRequest.value().roomId);
    if (roomRes.isError())
        return roomRes.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    auto &room = roomRes.value().get();
    auto joinRes = room.addUser(_user);
    JoinRoomResponse response{};

    if (joinRes.has_value()) {
        log<MenuRequestHandler>(__func__,
                           "User '" + _user.username + "' failed to join room '" + joinRoomRequest.value().roomId + "'",
                           false, _userEndpoint);
        response.status = false;
        return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
        } else {
        log<MenuRequestHandler>(__func__,
                           "User '" + _user.username + "' joined room '" + joinRoomRequest.value().roomId + "'",
                           true, _userEndpoint);
        response.status = true;
    }

    return RequestResult{JsonSerializer::serializeResponse(response),
                         _requestHandlerFactory.createRoomMemberRequestHandler(room, _user, _userEndpoint)};
}

RequestResult MenuRequestHandler::createRoom(const RequestInfo &request) {
    auto &roomManager = _requestHandlerFactory.getRoomManager();
    auto createRoomRequest = JsonDeserializer::deserializeCreateRoomRequest(request.buffer);
    if (createRoomRequest.isError())
        return createRoomRequest.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    RoomData roomData{
            "", // The uuid will be generated in the manager
            createRoomRequest.value().roomName,
            createRoomRequest.value().maxPlayers,
            createRoomRequest.value().questionCount,
            createRoomRequest.value().timePerQuestion,
            false
    };
    CreateRoomResponse response{};

    if (!isValidRoomDetails(roomData)) {
        response.status = false;
        log<MenuRequestHandler>(__func__, "User '" + _user.username + "' failed to create room (invalid rom details)",
                           false, _userEndpoint);
        return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
    }
    Room &room = roomManager.createRoom(_user, roomData);
    response.status = true;
    log<MenuRequestHandler>(__func__, "User '" + _user.username + "' created room '" + roomData.name + "'",
                       true, _userEndpoint);

    return RequestResult{JsonSerializer::serializeResponse(response),
                         _requestHandlerFactory.createRoomAdminRequestHandler(room, _user, _userEndpoint)};
}

bool MenuRequestHandler::isValidRoomDetails(const RoomData &roomData) {
    return roomData.name.length() >= 4 && roomData.maxPlayers > 1 && roomData.questionCount >= 2 &&
           roomData.timePerQuestion >= 5;
}

RequestResult MenuRequestHandler::getUserData() {
    auto userData = _requestHandlerFactory.getUsersManager().getUserData(_user.username);
    if (userData.isError())
        return userData.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    auto userStatistics = _requestHandlerFactory.getStatisticsManager().getUserStatistics(_user.username);
    if (userStatistics.isError())
        return userStatistics.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    GetUserDataResponse response{true, userData.value(), userStatistics.value()};

    log<MenuRequestHandler>(__func__, "User '" + _user.username + "' requested user data", true, _userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(response),
                         nullptr};
}

RequestResult MenuRequestHandler::updateUserData(const RequestInfo &request) {
    Result<UpdateUserDataRequest> updateUserDataRequest = JsonDeserializer::deserializeUpdateUserDataRequest(
            request.buffer);

    if (updateUserDataRequest.isError())
        return updateUserDataRequest.error().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    auto usersManager = _requestHandlerFactory.getUsersManager();

    auto isValidData = usersManager::isValidUserData(updateUserDataRequest.value());
    if (!isValidData.first) {
        UpdateUserDataResponse response{};
        response.status = false;
        response.message = isValidData.second;
        return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
    }

    auto result = usersManager.updateUserData(updateUserDataRequest.value(), _user.username);
    if (result.has_value())
        return result.value().toRequestResult<MenuRequestHandler>(__func__, _userEndpoint);

    UpdateUserDataResponse response{};
    response.status = true;
    response.message = "";
    log<MenuRequestHandler>(__func__, "User '" + _user.username + "' updated user data", true, _userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
}



