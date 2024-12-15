#pragma once

#include <string>
#include <kissnet.hpp>
#include <vector>
#include "../responses/responses.h"
#include "../requests/requests.h"



class JsonSerializer {
public:
    JsonSerializer() = delete;  // Prevent construction
    ~JsonSerializer() = delete;  // Prevent destruction

    static std::vector<unsigned char> serializeResponse(const LoginResponse &loginResponse);

    static std::vector<unsigned char> serializeResponse(const SignupResponse &signupResponse);

    static std::vector<unsigned char> serializeResponse(const ErrorResponse &errorResponse);

    static std::vector<unsigned char> serializeResponse(const LogoutResponse &logoutResponse);

    static std::vector<unsigned char> serializeResponse(const GetRoomsResponse &getRoomsResponse);

    static std::vector<unsigned char> serializeResponse(const GetPlayersInRoomResponse &getPlayersInRoomResponse);

    static std::vector<unsigned char> serializeResponse(const GetHighScoresResponse &getHighScoreResponse);

    static std::vector<unsigned char> serializeResponse(const GetPersonalStatsResponse &getPersonalStatsResponse);

    static std::vector<unsigned char> serializeResponse(const JoinRoomResponse &joinRoomResponse);

    static std::vector<unsigned char> serializeResponse(const CreateRoomResponse &createRoomResponse);

    static std::vector<unsigned char> serializeResponse(const GetUserDataResponse &getUserDataResponse);

    static std::vector<unsigned char> serializeResponse(const UpdateUserDataResponse &updateUserDataResponse);

    static std::vector<unsigned char> serializeResponse(const CloseRoomResponse& closeRoomResponse);

    static std::vector<unsigned char> serializeResponse(const StartGameResponse& startGameResponse);

    static std::vector<unsigned char> serializeResponse(const GetRoomStateResponse& roomStateResponse);

    static std::vector<unsigned char> serializeResponse(const LeaveRoomResponse& leaveRoomResponse);

    static std::vector<unsigned char> serializeResponse(const GetQuestionResponse& getQuestionResponse);

    static std::vector<unsigned char> serializeResponse(const SubmitAnswerResponse& submitAnswerResponse);

    static std::vector<unsigned char> serializeResponse(const GetGameResultsResponse& getGameResultsResponse);

    static std::vector<unsigned char> serializeResponse(const LeaveGameResponse& LeaveGameResponse);

    static std::vector<unsigned char> serializeResponse(const SubmitVerificationCodeResponse& submitVerificationCodeRequest);

    static std::vector<unsigned char> serializeResponse(const ResendVerificationCodeResponse& resendVerificationCodeResponse);

    static std::vector<unsigned char> serializeResponse(const ForgotPasswordResponse& forgotPasswordResponse);
};