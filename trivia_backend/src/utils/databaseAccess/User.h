#pragma once
#include <string>
#include <cstdint>


enum class AvatarColor
{
    RED = 0,
    BLUE = 1,
    GREEN = 2,
    PURPLE = 3,
    ORANGE = 4,
    CYAN = 5,
    TEAL = 6,
    PINK = 7,
};

struct User {
    unsigned int id = 0;    // unique (auto increment primary key)
    std::string username;   // unique
    std::string password;
    std::string email;      // unique
    std::string address;
    std::string phone_number;
    std::string birthday;
    uint8_t avatar_color;
    time_t member_since;
};
