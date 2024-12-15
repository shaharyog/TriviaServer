#pragma once

#include <vector>
#include <nlohmann/json.hpp>
#include "../../managers/roomData.h"
#include "../../managers/userStatistics.h"
#include "../databaseAccess/User.h"
#include "../responses/responses.h"

using Json = nlohmann::json;

class ConversionHelper {
public:
    ConversionHelper() = delete;  // Prevent construction
    ~ConversionHelper() = delete;  // Prevent destruction

    static std::vector<unsigned char> jsonToSizeBytes(const Json &json);

    static Json roomDataToJson(const RoomDataResponse &roomData);

    static Json playerToJson(const Player &player);

    static Json playerStatsToJson(const UserStatistics &playerStats);

    static std::string avatarColorToString(const uint8_t &avatarColorNumber);

    static Json playerResultToJson(const PlayerResult &playerResult);

    static Json userAnswerToJson(const std::pair<Question, std::pair<unsigned int, unsigned int>> &userAnswer);
};