#pragma once

#include <vector>
#include <ctime>
#include <memory>
#include "../../requestHandlers/IRequestHandler.h"
#include <string>
#include <chrono>
#include <optional>

class IRequestHandler;

enum class RequestId {
    LOGIN_REQUEST = 1,
    SIGNUP_REQUEST = 2,
    LOGOUT_REQUEST = 3,
    GET_ROOMS_REQUEST = 4,
    GET_PLAYERS_IN_ROOM_REQUEST = 5,
    GET_HIGHSCORES_REQUEST = 6,
    GET_PERSONAL_STATS_REQUEST = 7,
    CREATE_ROOM_REQUEST = 8,
    JOIN_ROOM_REQUEST = 9,
    GET_USER_DATA_REQUEST = 10,
    UPDATE_USER_DATA_REQUEST = 11,
    CLOSE_ROOM_REQUEST = 12,
    START_GAME_REQUEST = 13,
    GET_ROOM_STATE_REQUEST = 14,
    LEAVE_ROOM_REQUEST = 15,
    LEAVE_GAME_REQUEST = 16,
    GET_QUESTION_REQUEST = 17,
    SUBMIT_ANSWER_REQUEST = 18,
    GET_GAME_RESULTS_REQUEST = 19,
    SUBMIT_VERIFICATION_CODE_REQUEST = 20,
    RESEND_VERIFICATION_CODE_REQUEST = 21,
    FORGOT_PASSWORD_REQUEST = 22,
    EXIT = 99   // for the client Socket Errors / Disconnections
};

struct RequestInfo {
    RequestId requestId;
    std::vector<unsigned char> buffer;
};

struct RequestResult {
    std::vector<unsigned char> buffer;
    std::unique_ptr<IRequestHandler> newHandler;
};


struct LoginRequest {
    std::string username;
    std::string password;
};

struct SignupRequest {
    std::string username;
    std::string password;
    std::string email;
    std::string address;
    std::string phoneNumber;
    std::string birthday;
};

struct GetPlayersInRoomRequest {
    std::string roomId;
};

struct JoinRoomRequest {
    std::string roomId;
};

struct CreateRoomRequest {
    std::string roomName;
    unsigned int maxPlayers;
    unsigned int questionCount;
    unsigned int timePerQuestion;
};

struct UpdateUserDataRequest {
    std::optional<std::string> password;
    std::string address;
    std::string phoneNumber;
    std::string avatarColor;
};


struct SubmitAnswerRequest
{
    unsigned int answerId;
    unsigned int questionId;
};


struct SubmitVerificationCodeRequest
{
    std::string code;
};


struct ForgotPasswordRequest
{
    std::string email;
};