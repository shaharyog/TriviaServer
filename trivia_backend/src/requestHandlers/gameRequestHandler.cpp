#include "gameRequestHandler.h"
#include "../utils/marshaling/jsonDeserializer.h"

bool GameRequestHandler::isRequestRelevant(const RequestInfo &request) {
    return request.requestId == RequestId::LEAVE_GAME_REQUEST ||
           request.requestId == RequestId::GET_QUESTION_REQUEST ||
           request.requestId == RequestId::SUBMIT_ANSWER_REQUEST ||
           request.requestId == RequestId::GET_GAME_RESULTS_REQUEST ||
           request.requestId == RequestId::EXIT;
}

RequestResult GameRequestHandler::handleRequest(const RequestInfo &request) {
    if (!isRequestRelevant(request))
        return Error(ErrorType::InvalidRequest, "Request is not relevant").toRequestResult<GameRequestHandler>(__func__,
                                                                                                          _userEndpoint);

    switch (request.requestId) {
        case RequestId::LEAVE_GAME_REQUEST:
            return leaveGame();
        case RequestId::GET_QUESTION_REQUEST:
            return getQuestion();
        case RequestId::SUBMIT_ANSWER_REQUEST:
            return submitAnswer(request);
        case RequestId::GET_GAME_RESULTS_REQUEST:
            return getGameResults();
        case RequestId::EXIT:
            return leaveGameAndExit();
        default:
            return Error(ErrorType::InvalidRequest, "Request is not relevant").toRequestResult<GameRequestHandler>(__func__,
                                                                                                              _userEndpoint);
    }
}

RequestResult GameRequestHandler::getQuestion() {
    if (_game.isFinished())
        return Error(ErrorType::InvalidRequest, "Game is already finished").toRequestResult<GameRequestHandler>(__func__, _userEndpoint);

    const auto question = _game.getCurrentQuestion();

    const auto possibleAnswers = question.second.getPossibleAnswers();
    std::map<unsigned int, std::string> answersMap;

    for (unsigned int i = 0; i < possibleAnswers.size(); i++)
        answersMap[i] = possibleAnswers[i];

    log<GameRequestHandler>(__func__, "Player '" + _user.username + "' got question", true, _userEndpoint);
    const auto response = GetQuestionResponse{true, question.first,question.second.getQuestion(), answersMap};
    return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
}

RequestResult GameRequestHandler::submitAnswer(const RequestInfo &request) {
    const auto submitAnswerRequest = JsonDeserializer::deserializeSubmitAnswerRequest(request.buffer);
    if (submitAnswerRequest.isError())
        return submitAnswerRequest.error().toRequestResult<GameRequestHandler>(__func__, _userEndpoint);

    if (_game.isFinished())
        return Error(ErrorType::InvalidRequest, "Game is already finished").toRequestResult<GameRequestHandler>(__func__, _userEndpoint);

    const auto correctAnswer = _game.submitAnswer(_user, submitAnswerRequest.value().answerId, submitAnswerRequest.value().questionId);
    if (correctAnswer.isError())
        return correctAnswer.error().toRequestResult<GameRequestHandler>(__func__, _userEndpoint);

    log<GameRequestHandler>(__func__, "Player '" + _user.username + "' submitted answer", true, _userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(SubmitAnswerResponse{true, correctAnswer.value()}), nullptr};
}

RequestResult GameRequestHandler::getGameResults() {
    if (!_game.isFinished())
        return Error(ErrorType::InvalidRequest, "Game is not yet finished").toRequestResult<GameRequestHandler>(__func__, _userEndpoint);

    const auto usersMap = _game.getPlayersResults();
    const auto onlinePlayers = _game.getOnlinePlayers();
    std::vector<LoggedUser> users;
    for (const auto& user : usersMap)
        users.push_back(user.first);

    auto players = _requestHandlerFactory.getUsersManager().transformLoggedUsersToPlayers(users);
    if (players.isError())
        return players.error().toRequestResult<GameRequestHandler>(__func__, _userEndpoint);

    std::vector<PlayerResult> playerResults;
    for (const auto& player : players.value()) {
        const auto playerGameData = usersMap.at(LoggedUser{player.username});
        const auto doesPlayerOnline = std::find_if(onlinePlayers.begin(), onlinePlayers.end(), [&player](const auto& onlinePlayer) { return onlinePlayer.username == player.username; }) != onlinePlayers.end();
        playerResults.push_back(PlayerResult{player.username, player.avatar_color, doesPlayerOnline, static_cast<int>(playerGameData.scoreChange()), playerGameData.getNumOfCorrectAnswers(), playerGameData.getNumOfWrongAnswers(), playerGameData.getAverageAnswerTime()});
    }

    const auto response = GetGameResultsResponse{true, usersMap.at(_user).answers, playerResults};
    return RequestResult{JsonSerializer::serializeResponse(response), nullptr};
}

RequestResult GameRequestHandler::leaveGame() {
    const std::string& roomUuid = _game.getUuid();
    if (_game.isFinished()) {
        const auto result = _requestHandlerFactory.getGameManager().removePlayerFromGame(_game, _user);
        if (result.has_value()) {
            log<GameRequestHandler>(__func__, "Player '" + _user.username + "' left the game", true,
                                    _userEndpoint);
            return RequestResult{JsonSerializer::serializeResponse(LeaveGameResponse{true}), _requestHandlerFactory.createMenuRequestHandler(_user, _userEndpoint)};
        }

        // remove the player from the room
        _requestHandlerFactory.getRoomManager().removePlayerFromRoom(roomUuid, _user);

        log<GameRequestHandler>(__func__, "Player '" + _user.username + "' left the game", true,
                                _userEndpoint);
        return RequestResult{JsonSerializer::serializeResponse(LeaveGameResponse{true}), _requestHandlerFactory.createMenuRequestHandler(_user, _userEndpoint)};
    } else {
        const auto result = _requestHandlerFactory.getGameManager().removePlayerFromGame(_game, _user);

        if (result.has_value()) {
            log<GameRequestHandler>(__func__, "Player '" + _user.username + "' left the game, but failed to get punished", true,
                                    _userEndpoint);
            return RequestResult{JsonSerializer::serializeResponse(LeaveGameResponse{true}), _requestHandlerFactory.createMenuRequestHandler(_user, _userEndpoint)};
        }

        // remove the player from the room
        _requestHandlerFactory.getRoomManager().removePlayerFromRoom(roomUuid, _user);

        log<GameRequestHandler>(__func__, "Player '" + _user.username + "' left the game, and got punished", true,
                                _userEndpoint);
        return RequestResult{JsonSerializer::serializeResponse(LeaveGameResponse{true}), _requestHandlerFactory.createMenuRequestHandler(_user, _userEndpoint)};
    }
}

RequestResult GameRequestHandler::leaveGameAndExit() {
    _requestHandlerFactory.getLoginManager().logout(_user.username);
    log<GameRequestHandler>(__func__, "Player '" + _user.username + "' has disconnected, logging him out", true,
                       _userEndpoint);
    return leaveGame();
}
