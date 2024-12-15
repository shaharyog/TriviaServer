#include "jsonDeserializer.h"
#include <nlohmann/json.hpp>

using Json = nlohmann::json;

Result<LoginRequest>
JsonDeserializer::deserializeLoginRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");


    if (!j.contains("username") || !j.contains("password"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'username' or 'password'");
    else if (!j["username"].is_string() || !j["password"].is_string())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'username' or 'password' is not a string");

    LoginRequest loginRequest;
    loginRequest.username = j["username"];
    loginRequest.password = j["password"];

    return loginRequest;
}

Result<SignupRequest>
JsonDeserializer::deserializeSignupRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    // Create a LoginRequest object and populate it
    if (!j.contains("username") || !j.contains("password") || !j.contains("email") || !j.contains("address") ||
        !j.contains("phoneNumber") || !j.contains("birthday"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'username', 'password', 'email', 'address', 'phoneNumber', or 'birthday'");
    else if (!j["username"].is_string() || !j["password"].is_string() || !j["email"].is_string() ||
             !j["address"].is_string() || !j["phoneNumber"].is_string() || !j["birthday"].is_string())
        return Error(ErrorType::DeserializationError,
                     "Invalid JSON. 'username', 'password', 'email', 'address', 'phoneNumber', or 'birthday' is not a string");

    SignupRequest signupRequest;
    signupRequest.username = j["username"];
    signupRequest.password = j["password"];
    signupRequest.email = j["email"];
    signupRequest.address = j["address"];
    signupRequest.phoneNumber = j["phoneNumber"];
    signupRequest.birthday = j["birthday"];

    return signupRequest;
}

Result<GetPlayersInRoomRequest>
JsonDeserializer::deserializeGetPlayersInRoomRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    if (!j.contains("roomId"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'roomId'");

    if (!j["roomId"].is_string())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'roomId' is not a uuid string");

    GetPlayersInRoomRequest getPlayersInRoomRequest{j["roomId"]};

    return getPlayersInRoomRequest;

}

Result<JoinRoomRequest> JsonDeserializer::deserializeJoinRoomRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    if (!j.contains("roomId"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'roomId'");

    if (!j["roomId"].is_string())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'roomId' is not a uuid string");

    JoinRoomRequest joinRoomRequest{j["roomId"]};

    return joinRoomRequest;
}

Result<CreateRoomRequest> JsonDeserializer::deserializeCreateRoomRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    if (!j.contains("name") || !j.contains("maxPlayers") || !j.contains("questionCount") || !j.contains("timePerQuestion"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'name', 'maxPlayers', 'questionCount', or 'timePerQuestion'");

    if (!j["name"].is_string() || !j["maxPlayers"].is_number_unsigned() || !j["questionCount"].is_number_unsigned() || !j["timePerQuestion"].is_number_unsigned())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'name', 'maxPlayers', 'questionCount', or 'timePerQuestion' is not a string or number");

    CreateRoomRequest createRoomRequest;
    createRoomRequest.roomName = j["name"];
    createRoomRequest.maxPlayers = j["maxPlayers"];
    createRoomRequest.questionCount = j["questionCount"];
    createRoomRequest.timePerQuestion = j["timePerQuestion"];

    return createRoomRequest;
}

Result<UpdateUserDataRequest>
JsonDeserializer::deserializeUpdateUserDataRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    if (!j.contains("password") || !j.contains("address") || !j.contains("phoneNumber") || !j.contains("avatarColor"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'password', 'email', 'address', 'phoneNumber', or 'avatarColor'");

    if (!(j["password"].is_string() || j["password"].is_null()) || !j["address"].is_string() || !j["phoneNumber"].is_string() || !j["avatarColor"].is_string())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'password', 'email', 'address', 'phoneNumber', or 'avatarColor' is not a string. (password can be null)");

    UpdateUserDataRequest updateUserDataRequest;
    if (j["password"].is_string())
        updateUserDataRequest.password = j["password"];
    else
        updateUserDataRequest.password = std::nullopt;

    updateUserDataRequest.address = j["address"];
    updateUserDataRequest.phoneNumber = j["phoneNumber"];
    updateUserDataRequest.avatarColor = j["avatarColor"];

    return updateUserDataRequest;
}

Result<SubmitAnswerRequest> JsonDeserializer::deserializeSubmitAnswerRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    if (!j.contains("answerId") || !j.contains("questionId"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'answerId' or 'questionId'");

    if (!j["answerId"].is_number_unsigned(), !j["questionId"].is_number_unsigned())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'answerId' or 'questionId' is not a number");


    SubmitAnswerRequest submitAnswerRequest{j["answerId"], j["questionId"]};
    return submitAnswerRequest;
}

Result<SubmitVerificationCodeRequest>
JsonDeserializer::deserializeSubmitVerificationCodeRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    if (!j.contains("code"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'verificationCode'");

    if (!j["code"].is_string())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'verificationCode' is not a string");

    SubmitVerificationCodeRequest submitVerificationCodeRequest{j["code"]};
    return submitVerificationCodeRequest;
}

Result<ForgotPasswordRequest>
JsonDeserializer::deserializeForgotPasswordRequest(const std::vector<unsigned char> &buffer) {
    Json j = Json::parse(buffer, nullptr, false);
    if (j.is_discarded())
        return Error(ErrorType::DeserializationError, "Invalid JSON");

    if (!j.contains("email"))
        return Error(ErrorType::DeserializationError, "JSON is missing field 'email'");

    if (!j["email"].is_string())
        return Error(ErrorType::DeserializationError, "Invalid JSON. 'email' is not a string");

    ForgotPasswordRequest forgotPasswordRequest{j["email"]};
    return forgotPasswordRequest;
}
