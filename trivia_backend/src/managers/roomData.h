#pragma once

#include <string>

struct RoomData {
    std::string uuid;
    std::string name;
    unsigned int maxPlayers;
    unsigned int questionCount;
    unsigned int timePerQuestion;
    bool isActive;
};