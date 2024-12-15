#include <algorithm>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "roomManager.h"

Room& RoomManager::createRoom(const LoggedUser &admin, RoomData &roomData) {
    const auto roomUUID = generateRoomUUID();
    roomData.uuid = roomUUID;
    std::lock_guard lock(_roomsMutex);
    _rooms.emplace(roomUUID, Room(admin, roomData));
    return _rooms.at(roomUUID);
}

void RoomManager::deleteRoom(const std::string &roomUUID) {
    std::lock_guard lock(_roomsMutex);
    _rooms.erase(roomUUID);
}

std::vector<RoomData> RoomManager::getRooms() const {
    std::vector<RoomData> rooms;
    // lambda function to get the RoomData from the pair
    const auto getRoomData = [](const std::pair<const std::string, Room> &room) { return room.second.getMetadata(); };

    std::shared_lock sharedLock(_roomsMutex);
    // transform the map into a vector of RoomData
    std::transform(_rooms.cbegin(), _rooms.cend(), std::back_inserter(rooms), getRoomData);
    return rooms;
}

Result<std::reference_wrapper<Room>> RoomManager::getRoom(const std::string &roomUUID) {
    std::shared_lock sharedLock(_roomsMutex);
    if (_rooms.find(roomUUID) == _rooms.cend())
        return Error(ErrorType::NotFound, "Room not found");

    return _rooms.at(roomUUID); // maybe wrap with std::ref() in the future
}

std::string RoomManager::generateRoomUUID() {
    boost::uuids::uuid uuid{};

    // generate random uuid until it's unique, supposed to be in the first try.
    // there are  2^122, or 5.3×10^36 (5.3 undecillion) different uuids.
    // so it's not that big of a deal to just loop until it's unique
    // the chances of it not being unique are extremely low
    std::shared_lock sharedLock(_roomsMutex);
    do {
        boost::uuids::random_generator gen;
        uuid = gen();
    } while (_rooms.find(boost::uuids::to_string(uuid)) != _rooms.end());
    sharedLock.unlock();

    return boost::uuids::to_string(uuid);
}

std::optional<RoomState> RoomManager::getRoomState(const std::string &roomUUID) {
    std::shared_lock sharedLock(_roomsMutex);

    if (_rooms.find(roomUUID) == _rooms.cend())
        return std::nullopt;

    const auto &room = _rooms.at(roomUUID);
    const auto &roomData = room.getMetadata();
    const auto &roomUsers = room.getAllUsers();

    return RoomState{roomData, roomUsers};
}

void RoomManager::removePlayerFromRoom(const std::string &roomUUID, const LoggedUser &user) {
    std::lock_guard lock(_roomsMutex);
    _rooms.at(roomUUID).removeUser(user);

    // remove room if no players are left
    if (_rooms.at(roomUUID).getAllUsers().empty())
        _rooms.erase(roomUUID);
}
