#include "room.h"
#include <algorithm>

std::optional<Error> Room::addUser(const LoggedUser &user) {
    std::lock_guard lock(*_usersMutex);
    if (_users.size() >= _metadata.maxPlayers)
        return Error(ErrorType::RoomIsFull);

    // check if the User is already in the room
    if (std::find(_users.begin(), _users.end(), user) != _users.end())
        return Error(ErrorType::AlreadyExists, "User already in the room");

    _users.push_back(user);
    return std::nullopt;    // success
}

void Room::removeUser(const LoggedUser &user) {
    std::lock_guard lock(*_usersMutex);

    _users.erase(std::remove(_users.begin(), _users.end(), user), _users.end());
}

std::vector<LoggedUser> Room::getAllUsers() const {
    std::shared_lock sharedLock(*_usersMutex);
    return _users;
}
