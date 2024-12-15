#pragma once


#include "../utils/databaseAccess/IDatabasae.h"
#include "userStatistics.h"
#include "../utils/databaseAccess/User.h"

class StatisticsManager
{
public:
    explicit StatisticsManager(const std::shared_ptr<IDatabase> &database) : _database(database) {};
    ~StatisticsManager() = default;

    [[nodiscard]] Result<std::vector<Player>> getHighScores() const;
    [[nodiscard]] Result<UserStatistics> getUserStatistics(const std::string &username) const;
private:
    std::weak_ptr<IDatabase> _database;
};