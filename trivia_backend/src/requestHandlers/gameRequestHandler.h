#pragma once

#include "IRequestHandler.h"
#include "../utils/communicator/endpoint.h"
#include "../managers/loggedUser.h"
#include "requestHandlerFactory.h"

class GameRequestHandler : public IRequestHandler {
public:
    explicit GameRequestHandler(Game &game, RequestHandlerFactory &requestHandlerFactory, LoggedUser user,
                                Endpoint userEndpoint)
            : _game(game), _requestHandlerFactory(requestHandlerFactory), _user(std::move(user)),
              _userEndpoint(std::move(userEndpoint)) {}

    ~GameRequestHandler() override = default;

    bool isRequestRelevant(const RequestInfo &request) override;

    RequestResult handleRequest(const RequestInfo &request) override;

private:
    RequestResult getQuestion();
    RequestResult submitAnswer(const RequestInfo &request);
    RequestResult getGameResults();
    RequestResult leaveGame();
    RequestResult leaveGameAndExit();


    Game &_game;
    Endpoint _userEndpoint;
    LoggedUser _user;
    RequestHandlerFactory &_requestHandlerFactory;
};