#pragma once

#include <map>
#include <vector>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include "loggedUser.h"
#include "room.h"
#include "../errors/result.h"
#include "usersManager.h"

struct RoomState
{
    RoomData roomData;
    std::vector<LoggedUser> users;
};

class RoomManager {
public:
    Room& createRoom(const LoggedUser& admin, RoomData& roomData);
    void deleteRoom(const std::string& roomUUID);
    [[nodiscard]] std::vector<RoomData> getRooms() const;
    Result<std::reference_wrapper<Room>> getRoom(const std::string& roomUUID);
    std::optional<RoomState> getRoomState(const std::string &roomUUID);
    void removePlayerFromRoom(const std::string& roomUUID, const LoggedUser& user);

private:
    std::string generateRoomUUID();

    mutable std::shared_mutex _roomsMutex;
    std::map<std::string, Room> _rooms;
};