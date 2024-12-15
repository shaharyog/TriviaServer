#pragma once

#include <string>
#include <optional>
#include "../../errors/result.h"
#include <list>
#include <vector>
#include "QuestionDb.h"
#include "User.h"
#include "../../managers/game.h"

class IDatabase {
public:
    virtual ~IDatabase() = default;

    [[nodiscard]] virtual Result<bool> doesUserExist(const std::string &username) const = 0;

    [[nodiscard]] virtual Result<bool>
    doesPasswordMatch(const std::string &userName, const std::string &password) const = 0;

    virtual std::optional<Error>
    addUser(const std::string &userName, const std::string &password, const std::string &email,
            const std::string &address, const std::string &phoneNumber, const std::string &birthday, const AvatarColor &avatarColor) = 0;

    [[nodiscard]] virtual Result<bool> doesEmailExist(const std::string &email) const = 0;

    [[nodiscard]] virtual Result<std::list<QuestionDb>>
    getQuestions(unsigned int amount) = 0;  // not const, because it adds questions if needed

    [[nodiscard]] virtual Result<std::pair<unsigned int, UserStatistics>>  getUserStatistics(const std::string& username) const = 0;

    [[nodiscard]] virtual Result<std::vector<Player>> getHighScores() const = 0;
    [[nodiscard]] virtual Result<UserData> getUserData(const std::string& username) const = 0;
    [[nodiscard]] virtual std::optional<Error> updateUser(const UpdateUserDataRequest& user, const std::string& username) = 0;
    [[nodiscard]] virtual Result<Player> getPlayerByName(const std::string& username) const = 0;

    [[nodiscard]] virtual std::optional<Error> submitGameStatistics(const GameData& gameData, const std::string &user) = 0;

    [[nodiscard]] virtual std::optional<Error> removeUser(const std::string& username) = 0;
    [[nodiscard]] virtual Result<std::string> getUserPasswordByEmail(const std::string& email) = 0;
};