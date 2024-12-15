#include "loginRequestHandler.h"
#include "../utils/marshaling/jsonDeserializer.h"
#include "../utils/email_sender/emailSender.h"

bool LoginRequestHandler::isRequestRelevant(const RequestInfo &requestInfo) {
    return requestInfo.requestId == RequestId::LOGIN_REQUEST || requestInfo.requestId == RequestId::SIGNUP_REQUEST ||
           requestInfo.requestId == RequestId::EXIT || requestInfo.requestId == RequestId::FORGOT_PASSWORD_REQUEST;
}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo &requestInfo) {
    if (!isRequestRelevant(requestInfo))
        return Error(ErrorType::InvalidRequest, "User not authorized").toRequestResult<LoginRequestHandler>(__func__,
                                                                                                            _userEndpoint);

    if (requestInfo.requestId == RequestId::LOGIN_REQUEST)
        return login(requestInfo);

    if (requestInfo.requestId == RequestId::SIGNUP_REQUEST)
        return signup(requestInfo);

    if (requestInfo.requestId == RequestId::FORGOT_PASSWORD_REQUEST)
        return forgotPassword(requestInfo);

    // no need to do something, if the User has exited.
    // because we are still in the login phase
    if (requestInfo.requestId == RequestId::EXIT)
        return RequestResult{{}, nullptr};

    return Error(ErrorType::UserNotAuthorized, "User not authorized").toRequestResult<LoginRequestHandler>(__func__,
                                                                                                           _userEndpoint);
}

RequestResult LoginRequestHandler::login(const RequestInfo &request_info) {

    const auto deserializedRequestRes = JsonDeserializer::deserializeLoginRequest(request_info.buffer);
    if (deserializedRequestRes.isError())
        return deserializedRequestRes.error().toRequestResult<LoginRequestHandler>(__func__, _userEndpoint);


    const LoginRequest &request = deserializedRequestRes.value();
    auto &loginManager = _requestHandlerFactory.getLoginManager();

    const auto loginRes = loginManager.login(request.username, request.password);
    if (loginRes.isError())
        return loginRes.error().toRequestResult<LoginRequestHandler>(__func__, _userEndpoint);

    LoginResponse response{};
    response.status = loginRes.value().first;
    std::unique_ptr<IRequestHandler> request_handler;

    if (loginRes.value().first) {
        request_handler = _requestHandlerFactory.createMenuRequestHandler({request.username}, _userEndpoint);
        log<LoginRequestHandler>(__func__, "User '" + request.username + "' logged in", true, _userEndpoint);
    } else {
        request_handler = nullptr;
        response.message = loginRes.value().second;
        log<LoginRequestHandler>(__func__, "User failed to log in (" + loginRes.value().second + ")", false,
                                 _userEndpoint);
    }

    const auto buffer = JsonSerializer::serializeResponse(response);
    return RequestResult{buffer, std::move(request_handler)};
}

RequestResult LoginRequestHandler::signup(const RequestInfo &request_info) {
    const auto deserializedRequestRes = JsonDeserializer::deserializeSignupRequest(request_info.buffer);

    if (deserializedRequestRes.isError())
        return deserializedRequestRes.error().toRequestResult<LoginRequestHandler>(__func__, _userEndpoint);

    const SignupRequest &request = deserializedRequestRes.value();

    auto &loginManager = _requestHandlerFactory.getLoginManager();
    const auto signupRes = loginManager.signup(request.username, request.password, request.email, request.address,
                                               request.phoneNumber, request.birthday);
    if (signupRes.isError())
        return signupRes.error().toRequestResult<LoginRequestHandler>(__func__, _userEndpoint);

    SignupResponse response{};
    response.status = signupRes.value().first;

    if (!signupRes.value().first) {
        log<LoginRequestHandler>(__func__, "User failed to sign up (" + signupRes.value().second + ")", false,
                                 _userEndpoint);
        response.message = signupRes.value().second;
        const auto buffer = JsonSerializer::serializeResponse(response);
        return RequestResult{buffer, nullptr};
    }

    log<LoginRequestHandler>(__func__,
                             "User '" + request.username + "' started signup process, sending verification email...",
                             true, _userEndpoint);
    const auto verificationCode = EmailSender::generateRandom6DigitCode();
    const auto emailRes = EmailSender::sendEmailVerification(request.email, verificationCode, request.username);

    // abort signup process
    if (emailRes.has_value()) {
        _requestHandlerFactory.getUsersManager().removeUserUnverifiedUser(request.username);
        loginManager.logout(request.username);
        return emailRes.value().toRequestResult<LoginRequestHandler>(__func__, _userEndpoint);
    }
    log<LoginRequestHandler>(__func__, "Email sent successfully to user '" + request.username + "'", true,
                             _userEndpoint);

    const auto buffer = JsonSerializer::serializeResponse(response);
    std::unique_ptr<IRequestHandler> request_handler = _requestHandlerFactory.createVerificationRequestHandler(
            request.email, verificationCode, request.username, _userEndpoint);
    return RequestResult{buffer, std::move(request_handler)};
}

RequestResult LoginRequestHandler::forgotPassword(const RequestInfo &request) {
    const auto deserializedRequestRes = JsonDeserializer::deserializeForgotPasswordRequest(request.buffer);
    if (deserializedRequestRes.isError())
        return deserializedRequestRes.error().toRequestResult<LoginRequestHandler>(__func__, _userEndpoint);

    const ForgotPasswordRequest &requestInfo = deserializedRequestRes.value();
    auto &loginManager = _requestHandlerFactory.getLoginManager();
    const auto forgotPasswordRes = loginManager.forgotPassword(requestInfo.email);
    if (forgotPasswordRes.isError())
        return forgotPasswordRes.error().toRequestResult<LoginRequestHandler>(__func__, _userEndpoint);
    if (forgotPasswordRes.value())
        log<LoginRequestHandler>(__func__, "Password Recovery email sent successfully to user '" + requestInfo.email + "'",
                                 true, _userEndpoint);
    else
        log<LoginRequestHandler>(__func__,
                                 "User tried to recover email that does not exist: '" + requestInfo.email + "'", false,
                                 _userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(ForgotPasswordResponse{forgotPasswordRes.value()}), nullptr};
}
