#include "usersManager.h"
#include "../utils/validator/validator.h"

Result<UserData> usersManager::getUserData(const std::string &username) const {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    const auto user = db_ptr->getUserData(username);
    if (user.isError())
        return user.error();

    return user.value();
}

std::optional<Error> usersManager::updateUserData(const UpdateUserDataRequest &userData, const std::string &username) const {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    return db_ptr->updateUser(userData, username);
}

std::pair<bool, std::string> usersManager::isValidUserData(const UpdateUserDataRequest &userData) {

    const auto isValidAddress = Validator::isValidAddress(userData.address);
    if (!isValidAddress.first)
        return {false, isValidAddress.second};

    const auto isValidPhoneNumber = Validator::isValidPhoneNumber(userData.phoneNumber);
    if (!isValidPhoneNumber.first)
        return {false, isValidPhoneNumber.second};

    if (userData.password.has_value()) {
        // check password lastly, because it uses the web to check strength and require more resources
        const auto isValidPassword = Validator::isValidPassword(userData.password.value());
        if (!isValidPassword.first)
            return {false, isValidPassword.second};
    }

    return {true, ""};
}

Result<Player> usersManager::getPlayerByName(const std::string &username) const {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    return db_ptr->getPlayerByName(username);
}

Result<std::vector<Player>> usersManager::transformLoggedUsersToPlayers(const std::vector<LoggedUser> &users) const {
    std::vector<Player> players;
    for (const auto &user: users) {
        auto player = getPlayerByName(user.username);
        if (player.isError())
            return player.error();

        players.push_back(player.value());
    }

    return players;
}

void usersManager::removeUserUnverifiedUser(const std::string &username) {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return;

    const auto _ = db_ptr->removeUser(username);
}
