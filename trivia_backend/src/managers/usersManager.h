#pragma once

#include <memory>
#include "../utils/databaseAccess/IDatabasae.h"

class usersManager {
public:
    explicit usersManager(const std::shared_ptr<IDatabase> &database) : _database(database) {};
    ~usersManager() = default;

    [[nodiscard]] Result<UserData> getUserData(const std::string &username) const;
    // [[nodiscard]] Result<std::vector<User>> getAllUsers() const; // impl later maybe
    [[nodiscard]] std::optional<Error> updateUserData(const UpdateUserDataRequest& userData, const std::string& username) const;
    [[nodiscard]] Result<Player> getPlayerByName(const std::string& username) const;
    [[nodiscard]] static std::pair<bool, std::string> isValidUserData(const UpdateUserDataRequest &userData);
    [[nodiscard]] Result<std::vector<Player>> transformLoggedUsersToPlayers(const std::vector<LoggedUser> &users) const;
    void removeUserUnverifiedUser(const std::string &username);
private:
    std::weak_ptr<IDatabase> _database;
};