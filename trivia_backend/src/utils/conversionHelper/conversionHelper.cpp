#include "conversionHelper.h"



std::vector<unsigned char> ConversionHelper::jsonToSizeBytes(const Json &json) {
    unsigned int size = json.dump().size();
    std::vector<unsigned char> bytes(4);

    // store the size as binary in 4 bytes, in big-endian
    for (int i = 3; i >= 0; --i) {
        bytes[i] = size & 0xFF; // get the last 8 bits
        size >>= 8; // remove the last 8 bits
    }

    return bytes;
}

Json ConversionHelper::roomDataToJson(const RoomDataResponse &roomData) {
    Json j;
    j["id"] = roomData.roomData.uuid;
    j["name"] = roomData.roomData.name;
    j["maxPlayers"] = roomData.roomData.maxPlayers;
    j["questionCount"] = roomData.roomData.questionCount;
    j["timePerQuestion"] = roomData.roomData.timePerQuestion;
    j["isActive"] = roomData.roomData.isActive;
    j["players"] = Json::array();
    j["isFinished"] = roomData.isFinished;
    for (const auto &player : roomData.players)
        j["players"].push_back(playerToJson(player));

    return j;
}


Json ConversionHelper::playerStatsToJson(const UserStatistics &playerStats) {
    Json j;
    if (playerStats.averageAnswerTime.has_value())
        j["avgAnswerTime"] = playerStats.averageAnswerTime.value();
    else
        j["avgAnswerTime"] = nullptr;
    j["correctAnswers"] = playerStats.numOfCorrectAnswers;
    j["wrongAnswers"] = playerStats.numOfWrongAnswers;
    j["totalAnswers"] = playerStats.numOfTotalAnswers;
    j["totalGames"] = playerStats.numOfTotalGames;
    j["score"] = playerStats.userScore;
    return j;
}

std::string ConversionHelper::avatarColorToString(const uint8_t &avatarColorNumber) {

    auto avatarColor = static_cast<AvatarColor>(avatarColorNumber);
    // convert the avatar color to a string
    switch (avatarColor) {
        case AvatarColor::RED:
            return "Red";
        case AvatarColor::ORANGE:
            return "Orange";
        case AvatarColor::GREEN:
            return "Green";
        case AvatarColor::BLUE:
            return "Blue";
        case AvatarColor::PURPLE:
            return "Purple";
        case AvatarColor::PINK:
            return "Pink";
        case AvatarColor::CYAN:
            return "Cyan";
        case AvatarColor::TEAL:
            return "Teal";
        default:
            return "unknown";
    }
}

Json ConversionHelper::playerToJson(const Player &player) {
    Json j;
    j["username"] = player.username;
    j["avatarColor"] = player.avatar_color;
    j["score"] = player.score;
    return j;

}

Json ConversionHelper::playerResultToJson(const PlayerResult &playerResult) {
    Json j;
    j["username"] = playerResult.username;
    j["avatarColor"] = playerResult.avatar_color;
    j["isOnline"] = playerResult.isOnline;
    j["scoreChange"] = playerResult.scoreChange;
    j["correctAnswerCount"] = playerResult.correctAnswerCount;
    j["wrongAnswerCount"] = playerResult.wrongAnswerCount;
    j["avgAnswerTime"] = playerResult.avgAnswerTime;
    return j;
}

Json ConversionHelper::userAnswerToJson(const std::pair<Question, std::pair<unsigned int, unsigned int>> &userAnswer) {
    Json j;
    j["question"] = userAnswer.first.getQuestion();
    j["answers"] = Json::array();
    for (const auto &answer: userAnswer.first.getPossibleAnswers())
        j["answers"].push_back(answer);

    j["correctAnswer"] = userAnswer.first.getCorrectAnswerIndex();
    j["userAnswer"] = userAnswer.second.first;
    j["timeTaken"] = userAnswer.second.second;

    return j;
}

