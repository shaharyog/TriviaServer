#pragma once

#include <string>

struct LoggedUser {
    std::string username;

    bool operator==(const LoggedUser& other) const {
        return username == other.username;
    }
    bool operator >(const LoggedUser& other) const {
        return username > other.username;
    }

    bool operator <(const LoggedUser& other) const {
        return username < other.username;
    }

};