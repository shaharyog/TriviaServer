#pragma once

#include "../utils/databaseAccess/IDatabasae.h"
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include "loggedUser.h"

class LoginManager {
public:
    explicit LoginManager(const std::shared_ptr<IDatabase> &database) : _database(database) {};

    ~LoginManager() = default;

    Result<std::pair<bool, std::string>> signup(const std::string &username, const std::string &password, const std::string &email,
                                const std::string &address, const std::string &phoneNumber,
                                const std::string &birthday);

    Result<std::pair<bool, std::string>> login(const std::string &username, const std::string &password);

    void logout(const std::string &username);

    Result<bool> forgotPassword(const std::string& email);
private:
    static std::pair<bool, std::string> isValidSignupCredentials(const std::string &username, const std::string &password, const std::string &email,
                                  const std::string &address, const std::string &phoneNumber, const std::string &birthday);

    static AvatarColor getRandomAvatarColor() ;
    std::weak_ptr<IDatabase> _database;
    std::vector<LoggedUser> _loggedUsers;
    mutable std::shared_mutex _loggedUsersMutex;
};



