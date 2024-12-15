#include "statisticsManager.h"


Result<std::vector<Player>> StatisticsManager::getHighScores() const {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    return db_ptr->getHighScores();
}

Result<UserStatistics> StatisticsManager::getUserStatistics(const std::string &username) const {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    const auto res =  db_ptr->getUserStatistics(username);
    if (res.isError())
        return res.error();

    return res.value().second;
}
