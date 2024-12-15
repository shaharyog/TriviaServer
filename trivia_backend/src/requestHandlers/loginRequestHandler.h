#pragma once
#include <utility>
#include "IRequestHandler.h"
#include "../managers/loginManager.h"
#include "../utils/databaseAccess/sqliteDatabase.h"
#include "requestHandlerFactory.h"
#include "../utils/communicator/endpoint.h"

class RequestHandlerFactory;

class LoginRequestHandler : public IRequestHandler {
public:
    explicit LoginRequestHandler(RequestHandlerFactory &requestHandlerFactory, Endpoint userEndpoint)
            : _requestHandlerFactory(requestHandlerFactory), _userEndpoint(std::move(userEndpoint)) {};

    ~LoginRequestHandler() override = default;

    bool isRequestRelevant(const RequestInfo &request) override;

    RequestResult handleRequest(const RequestInfo &request) override;

private:

    RequestResult login(const RequestInfo &request);

    RequestResult signup(const RequestInfo &request);

    RequestResult forgotPassword(const RequestInfo &request);

    Endpoint _userEndpoint;
    RequestHandlerFactory &_requestHandlerFactory;
};