#include "verificationHandler.h"
#include "../utils/marshaling/jsonDeserializer.h"
#include "../utils/email_sender/emailSender.h"

bool VerificationRequestHandler::isRequestRelevant(const RequestInfo &request) {
    return RequestId::SUBMIT_VERIFICATION_CODE_REQUEST == request.requestId ||
           RequestId::EXIT == request.requestId ||
           RequestId::RESEND_VERIFICATION_CODE_REQUEST == request.requestId;
}

RequestResult VerificationRequestHandler::handleRequest(const RequestInfo &requestInfo) {
    if (!isRequestRelevant(requestInfo))
        return Error(ErrorType::InvalidRequest, "User not authorized").toRequestResult<VerificationRequestHandler>(
                __func__,
                _userEndpoint);

    if (requestInfo.requestId == RequestId::SUBMIT_VERIFICATION_CODE_REQUEST) {
        return submitVerificationCode(requestInfo);
    } else if (requestInfo.requestId == RequestId::RESEND_VERIFICATION_CODE_REQUEST) {
        return resendVerificationCode();
    } else if (requestInfo.requestId == RequestId::EXIT) {
        return abortSignupProcess();
    }

    return Error(ErrorType::InvalidRequest, "User not authorized").toRequestResult<VerificationRequestHandler>(__func__,_userEndpoint);
}

RequestResult VerificationRequestHandler::submitVerificationCode(const RequestInfo &request) {
    const auto requestJson = JsonDeserializer::deserializeSubmitVerificationCodeRequest(request.buffer);
    if (requestJson.isError())
        return requestJson.error().toRequestResult<VerificationRequestHandler>(__func__, _userEndpoint);

    _tries++;
    if (requestJson.value().code == _currentVerificationCode) {
        log<VerificationRequestHandler>(__func__, "User submitted correct verification code, logging him in", true,
                                        _userEndpoint);
        return RequestResult{JsonSerializer::serializeResponse(SubmitVerificationCodeResponse{true, true}),
                             _requestHandlerFactory.createMenuRequestHandler(
                                     {_username}, _userEndpoint)};
    }

    if (_tries >= 5)
    {
        log<VerificationRequestHandler>(__func__, "User finished tries limit, transferring him to login", true,
                                        _userEndpoint);
        _requestHandlerFactory.getUsersManager().removeUserUnverifiedUser(_username);
        _requestHandlerFactory.getLoginManager().logout(_username);
        return RequestResult{JsonSerializer::serializeResponse(SubmitVerificationCodeResponse{false, false}),
                             _requestHandlerFactory.createLoginRequestHandler(_userEndpoint)};
    }

    log<VerificationRequestHandler>(__func__, "User submitted incorrect verification code", true, _userEndpoint);
    return RequestResult{JsonSerializer::serializeResponse(SubmitVerificationCodeResponse{true, false}), nullptr};
}

RequestResult VerificationRequestHandler::resendVerificationCode() {
    log<VerificationRequestHandler>(__func__, "User requested to resend verification code", true, _userEndpoint);
    const auto verificationCode = EmailSender::generateRandom6DigitCode();
    const auto emailRes = EmailSender::sendEmailVerification(_email, verificationCode, _username);

    // could not send email
    if (emailRes.has_value()) {
        log<VerificationRequestHandler>(__func__, "Failed to resend verification code to user '" + _username + "'",
                                        true, _userEndpoint);
        return RequestResult{JsonSerializer::serializeResponse(ResendVerificationCodeResponse{false}), nullptr};
    }
    log<VerificationRequestHandler>(__func__, "User '" + _username + "' requested to resend verification code", true,
                                    _userEndpoint);
    _currentVerificationCode = verificationCode;
    return RequestResult{JsonSerializer::serializeResponse(ResendVerificationCodeResponse{true}), nullptr};
}

RequestResult VerificationRequestHandler::abortSignupProcess() {
    _requestHandlerFactory.getUsersManager().removeUserUnverifiedUser(_username);
    _requestHandlerFactory.getLoginManager().logout(_username);
    return RequestResult{JsonSerializer::serializeResponse(ErrorResponse{}),
                         _requestHandlerFactory.createLoginRequestHandler(_userEndpoint)};
}
