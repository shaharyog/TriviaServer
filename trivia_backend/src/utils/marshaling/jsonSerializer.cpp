#include "jsonSerializer.h"
#include "../conversionHelper/conversionHelper.h"
#include <nlohmann/json.hpp>
#include <iostream>

using Json = nlohmann::json;

std::vector<unsigned char> JsonSerializer::serializeResponse(const LoginResponse &loginResponse) {
    std::vector<unsigned char> response;

    response.push_back(static_cast<unsigned char>(ResponseId::LOGIN_RESPONSE));
    Json j;
    j["status"] = loginResponse.status;
    j["message"] = loginResponse.message;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const SignupResponse &signupResponse) {
    std::vector<unsigned char> response;

    response.push_back(static_cast<unsigned char>(ResponseId::SIGNUP_RESPONSE));
    Json j;
    j["status"] = signupResponse.status;
    j["message"] = signupResponse.message;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const ErrorResponse &errorResponse) {
    std::vector<unsigned char> response;

    response.push_back(static_cast<unsigned char>(ResponseId::ERROR_RESPONSE));
    Json j;
    j["message"] = errorResponse.errorMessage;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const LogoutResponse &logoutResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::LOGOUT_RESPONSE));

    Json j;
    j["status"] = logoutResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetRoomsResponse &getRoomsResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_ROOMS_RESPONSE));

    Json j;
    j["status"] = getRoomsResponse.status;
    j["rooms"] = Json::array();

    for (const auto &room: getRoomsResponse.rooms)
        j["rooms"].push_back(ConversionHelper::roomDataToJson(room));

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetPlayersInRoomResponse &getPlayersInRoomResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_PLAYERS_IN_ROOM_RESPONSE));

    Json j;
    j["status"] = getPlayersInRoomResponse.status;
    j["players"] = Json::array();

    for (const auto &player: getPlayersInRoomResponse.players)
        j["players"].push_back(ConversionHelper::playerToJson(player));

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetHighScoresResponse &getHighScoreResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_HIGHSCORES_RESPONSE));

    Json j;
    j["status"] = getHighScoreResponse.status;
    j["players"] = Json::array();

    for (const auto &player: getHighScoreResponse.statistics)
        j["players"].push_back(ConversionHelper::playerToJson(player));

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;


}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetPersonalStatsResponse &getPersonalStatsResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_PERSONAL_STATS_RESPONSE));

    Json j;
    j["status"] = getPersonalStatsResponse.status;
    j["statistics"] = ConversionHelper::playerStatsToJson(getPersonalStatsResponse.statistics);

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const JoinRoomResponse &joinRoomResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::JOIN_ROOM_RESPONSE));

    Json j;
    j["status"] = joinRoomResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const CreateRoomResponse &createRoomResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::CREATE_ROOM_RESPONSE));

    Json j;
    j["status"] = createRoomResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetUserDataResponse &getUserDataResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_USER_DATA_RESPONSE));

    Json j;
    j["status"] = getUserDataResponse.status;
    Json userData;
    userData["username"] = getUserDataResponse.userData.username;
    userData["email"] = getUserDataResponse.userData.email;
    userData["address"] = getUserDataResponse.userData.address;
    userData["phoneNumber"] = getUserDataResponse.userData.phone_number;
    userData["birthday"] = getUserDataResponse.userData.birthday;
    userData["avatarColor"] = getUserDataResponse.userData.avatar_color;
    userData["memberSince"] = getUserDataResponse.userData.member_since;
    j["userData"] = userData;
    j["userStatistics"] = ConversionHelper::playerStatsToJson(getUserDataResponse.statistics);

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const UpdateUserDataResponse &updateUserDataResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::UPDATE_USER_DATA_RESPONSE));

    Json j;
    j["status"] = updateUserDataResponse.status;
    j["message"] = updateUserDataResponse.message;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}


std::vector<unsigned char> JsonSerializer::serializeResponse(const CloseRoomResponse &closeRoomResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::CLOSE_ROOM_RESPONSE));

    Json j;
    j["status"] = closeRoomResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const StartGameResponse &startGameResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::START_GAME_RESPONSE));

    Json j;
    j["status"] = startGameResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetRoomStateResponse &roomStateResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_ROOM_STATE_RESPONSE));

    Json j;
    j["status"] = roomStateResponse.status;
    j["hasGameBegun"] = roomStateResponse.hasGameBegun;
    j["players"] = Json::array();

    for (const auto &player: roomStateResponse.players)
        j["players"].push_back(ConversionHelper::playerToJson(player));

    j["questionCount"] = roomStateResponse.questionCount;
    j["answerTimeout"] = roomStateResponse.answerTimeout;
    j["maxPlayers"] = roomStateResponse.maxPlayers;
    j["isClosed"] = roomStateResponse.isClosed;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const LeaveRoomResponse &leaveRoomResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::LEAVE_ROOM_RESPONSE));

    Json j;
    j["status"] = leaveRoomResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetQuestionResponse &getQuestionResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_QUESTION_RESPONSE));

    Json j;
    j["status"] = getQuestionResponse.status;
    j["questionId"] = getQuestionResponse.questionId;
    j["question"] = getQuestionResponse.question;
    j["answers"] = Json::array();

    for (const auto &answer: getQuestionResponse.answers)
        j["answers"].push_back(answer);


    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const SubmitAnswerResponse &submitAnswerResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::SUBMIT_ANSWER_RESPONSE));

    Json j;
    j["status"] = submitAnswerResponse.status;
    j["correctAnswerId"] = submitAnswerResponse.correctAnswerId;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const GetGameResultsResponse &getGameResultsResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::GET_GAME_RESULTS_RESPONSE));


    Json j;
    j["status"] = getGameResultsResponse.status;
    j["userAnswers"] = Json::array();
    for (const auto &userAnswer: getGameResultsResponse.userAnswers)
        j["userAnswers"].push_back(ConversionHelper::userAnswerToJson(userAnswer));

    j["players"] = Json::array();

    for (const auto &player: getGameResultsResponse.players)
        j["players"].push_back(ConversionHelper::playerResultToJson(player));

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const LeaveGameResponse &LeaveGameResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::LEAVE_GAME_RESPONSE));

    Json j;
    j["status"] = LeaveGameResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char>
JsonSerializer::serializeResponse(const SubmitVerificationCodeResponse &submitVerificationCodeRequest) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::SUBMIT_VERIFICATION_CODE_RESPONSE));

    Json j;
    j["status"] = submitVerificationCodeRequest.status;
    j["isVerified"] = submitVerificationCodeRequest.isVerified;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char>
JsonSerializer::serializeResponse(const ResendVerificationCodeResponse &resendVerificationCodeResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::RESEND_VERIFICATION_CODE_RESPONSE));

    Json j;
    j["status"] = resendVerificationCodeResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}

std::vector<unsigned char> JsonSerializer::serializeResponse(const ForgotPasswordResponse &forgotPasswordResponse) {
    std::vector<unsigned char> response;
    response.push_back(static_cast<unsigned char>(ResponseId::FORGOT_PASSWORD_RESPONSE));

    Json j;
    j["status"] = forgotPasswordResponse.status;

    std::vector<unsigned char> sizeBytes = ConversionHelper::jsonToSizeBytes(j);
    response.insert(response.end(), sizeBytes.begin(), sizeBytes.end());

    std::string jsonString = j.dump(); // Serialize JSON to a string
    response.insert(response.end(), jsonString.begin(), jsonString.end());

    return response;
}




