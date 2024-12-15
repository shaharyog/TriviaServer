#pragma once

#include <utility>

#include "IRequestHandler.h"
#include "../managers/loginManager.h"
#include "../utils/databaseAccess/sqliteDatabase.h"
#include "requestHandlerFactory.h"
#include "../utils/communicator/endpoint.h"

class VerificationRequestHandler : public IRequestHandler {
public:
    explicit VerificationRequestHandler(RequestHandlerFactory &requestHandlerFactory, std::string email,
                                        std::string verificationCode, std::string username, Endpoint userEndpoint)
            : _requestHandlerFactory(requestHandlerFactory), _userEndpoint(std::move(userEndpoint)),
              _email(std::move(email)), _currentVerificationCode(std::move(verificationCode)), _username(std::move(username)) {};

    ~VerificationRequestHandler() override = default;

    bool isRequestRelevant(const RequestInfo &request) override;

    RequestResult handleRequest(const RequestInfo &request) override;

private:

    RequestResult submitVerificationCode(const RequestInfo &request);
    RequestResult resendVerificationCode();
    RequestResult abortSignupProcess();

    unsigned int _tries = 0;
    std::string _email;
    std::string _currentVerificationCode;
    std::string _username;
    Endpoint _userEndpoint;
    RequestHandlerFactory &_requestHandlerFactory;
};