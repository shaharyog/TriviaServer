#pragma once

#include <utility>
#include <vector>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include "roomData.h"
#include "loggedUser.h"
#include "../errors/error.h"

class Room {
public:
    explicit Room(const LoggedUser& admin, RoomData metadata) : _metadata(std::move(metadata)), _usersMutex(std::make_shared<std::shared_mutex>()) {
        _users.emplace_back(admin);
    }

    std::optional<Error> addUser(const LoggedUser& user);
    void removeUser(const LoggedUser& user);
    [[nodiscard]] std::vector<LoggedUser> getAllUsers() const;
    void setIsActive(bool isActive) { _metadata.isActive = isActive; }
    [[nodiscard]] RoomData getMetadata() const { return _metadata; }
private:
    mutable std::shared_ptr<std::shared_mutex> _usersMutex;
    RoomData _metadata;
    std::vector<LoggedUser> _users;
};