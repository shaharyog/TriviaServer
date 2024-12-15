#pragma once

#include <string>
#include "../../managers/roomData.h"
#include "../../managers/userStatistics.h"
#include "../databaseAccess/User.h"
#include "../../managers/question.h"
#include <map>

enum class ResponseId {
    ERROR_RESPONSE = 0,
    LOGIN_RESPONSE = 1,
    SIGNUP_RESPONSE = 2,
    LOGOUT_RESPONSE = 3,
    GET_ROOMS_RESPONSE = 4,
    GET_PLAYERS_IN_ROOM_RESPONSE = 5,
    GET_HIGHSCORES_RESPONSE = 6,
    GET_PERSONAL_STATS_RESPONSE = 7,
    CREATE_ROOM_RESPONSE = 8,
    JOIN_ROOM_RESPONSE = 9,
    GET_USER_DATA_RESPONSE = 10,
    UPDATE_USER_DATA_RESPONSE = 11,
    CLOSE_ROOM_RESPONSE = 12,
    START_GAME_RESPONSE = 13,
    GET_ROOM_STATE_RESPONSE = 14,
    LEAVE_ROOM_RESPONSE = 15,
    LEAVE_GAME_RESPONSE = 16,
    GET_QUESTION_RESPONSE = 17,
    SUBMIT_ANSWER_RESPONSE = 18,
    GET_GAME_RESULTS_RESPONSE = 19,
    SUBMIT_VERIFICATION_CODE_RESPONSE = 20,
    RESEND_VERIFICATION_CODE_RESPONSE = 21,
    FORGOT_PASSWORD_RESPONSE = 22,
};

struct LoginResponse {
    bool status;
    std::string message;
};

struct SignupResponse {
    bool status;
    std::string message;
};

struct ErrorResponse {
    std::string errorMessage;
};

struct LogoutResponse {
    bool status;
};

struct Player {
    std::string username;
    std::string avatar_color;
    unsigned int score;
};

struct RoomDataResponse {
    RoomData roomData;
    bool isFinished;
    std::vector<Player> players;
};

struct GetRoomsResponse {
    bool status;
    std::vector<RoomDataResponse> rooms;
};


struct GetPlayersInRoomResponse {
    bool status;
    std::vector<Player> players;
};

struct GetHighScoresResponse {
    bool status;
    std::vector<Player> statistics;
};

struct GetPersonalStatsResponse {
    bool status;
    UserStatistics statistics;
};

struct JoinRoomResponse {
    bool status;
};

struct CreateRoomResponse {
    bool status;
};


struct UserData {
    std::string username;
    std::string email;
    std::string address;
    std::string phone_number;
    std::string birthday;
    std::string avatar_color;
    time_t member_since;
};

struct GetUserDataResponse {
    bool status;
    UserData userData;
    UserStatistics statistics;
};

struct UpdateUserDataResponse {
    bool status;
    std::string message;
};

struct CloseRoomResponse {
    bool status;
};

struct StartGameResponse {
    bool status;
};

struct GetRoomStateResponse {
    bool status;
    bool hasGameBegun;
    std::vector<Player> players;
    unsigned int questionCount;
    unsigned int answerTimeout;
    unsigned int maxPlayers;
    bool isClosed;
};

struct LeaveRoomResponse {
    bool status;
};

struct GetQuestionResponse {
    bool status;
    unsigned int questionId;
    std::string question;
    std::map<unsigned int ,std::string> answers;
};

struct SubmitAnswerResponse {
    bool status;
    unsigned int correctAnswerId;
};


struct PlayerResult {
    std::string username;
    std::string avatar_color;
    bool isOnline;
    int scoreChange;
    unsigned int correctAnswerCount;
    unsigned int wrongAnswerCount;
    unsigned int avgAnswerTime;
};


struct GetGameResultsResponse {
    bool status;
    std::vector<std::pair<Question, std::pair<unsigned int, unsigned int>>> userAnswers;
    std::vector<PlayerResult> players;
};

struct LeaveGameResponse {
    bool status;
};

struct SubmitVerificationCodeResponse {
    bool status;
    bool isVerified;
};

struct ResendVerificationCodeResponse {
    bool status;
};

struct ForgotPasswordResponse {
    bool status;
};