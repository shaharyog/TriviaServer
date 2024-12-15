#pragma once
#include <list>
#include <sqlite_orm/sqlite_orm.h>
#include <mutex>
#include <shared_mutex>
#include "IDatabasae.h"
#include "../../constants.h"
#include "User.h"


struct Statistics {
    unsigned int user_id;            // foreign key to User
    unsigned int wrong_answers;    // total number of questions the User answered
    unsigned int correct_answers;    // number of questions the User answered correctly
    unsigned int avg_answer_time;       // average time the User took to answer a question
    unsigned int total_games;        // total number of games the User played
    unsigned int score;              // total score the User achieved
};

inline auto create_tables_storage(const std::string &db_name) {
    return sqlite_orm::make_storage(db_name,
                        sqlite_orm::make_table("users",
                                   sqlite_orm::make_column("id", &User::id, sqlite_orm::primary_key().autoincrement()),
                                   sqlite_orm::make_column("username", &User::username, sqlite_orm::unique()),
                                   sqlite_orm::make_column("password", &User::password),
                                   sqlite_orm::make_column("email", &User::email, sqlite_orm::unique()),
                                   sqlite_orm::make_column("address", &User::address),
                                   sqlite_orm::make_column("phone_number", &User::phone_number),
                                   sqlite_orm::make_column("birthday", &User::birthday),
                                   sqlite_orm::make_column("avatar_color", &User::avatar_color),
                                   sqlite_orm::make_column("member_since", &User::member_since)
                        ),

                        sqlite_orm::make_table("questions",
                                                sqlite_orm::make_column("id", &QuestionDb::id, sqlite_orm::primary_key().autoincrement()),
                                                sqlite_orm::make_column("question", &QuestionDb::question),
                                                sqlite_orm::make_column("correct_answer", &QuestionDb::correct_answer),
                                                sqlite_orm::make_column("incorrect_answer1", &QuestionDb::incorrect_answer1),
                                                sqlite_orm::make_column("incorrect_answer2", &QuestionDb::incorrect_answer2),
                                                sqlite_orm::make_column("incorrect_answer3", &QuestionDb::incorrect_answer3)
                                                ),

                        sqlite_orm::make_table("statistics",
                                                sqlite_orm::make_column("user_id", &Statistics::user_id),
                                                sqlite_orm::make_column("wrong_answers", &Statistics::wrong_answers),
                                                sqlite_orm::make_column("correct_answers", &Statistics::correct_answers),
                                                sqlite_orm::make_column("avg_answer_time", &Statistics::avg_answer_time),
                                                sqlite_orm::make_column("total_games", &Statistics::total_games),
                                                sqlite_orm::make_column("score", &Statistics::score),
                                                sqlite_orm::foreign_key(&Statistics::user_id).references(&User::id).on_delete.cascade()
                                                )
    );
}

using DB = decltype(create_tables_storage(""));


class SqliteDatabase : public IDatabase
{
public:
    SqliteDatabase();
    ~SqliteDatabase() override;

    Result<bool> doesUserExist(const std::string& username) const override;
    Result<bool> doesPasswordMatch(const std::string& userName, const std::string& password) const override;
    std::optional<Error> addUser(const std::string& userName, const std::string& password, const std::string& email, const std::string& address, const std::string& phoneNumber, const std::string& birthday, const AvatarColor& avatarColor) override;
    Result<bool> doesEmailExist(const std::string& email) const override;

    Result<std::list<QuestionDb>> getQuestions(unsigned int amount) override;
    Result<std::pair<unsigned int, UserStatistics>>  getUserStatistics(const std::string& username) const override;
    Result<std::vector<Player>> getHighScores() const override;
    Result<UserData> getUserData(const std::string& username) const override;
    std::optional<Error> updateUser(const UpdateUserDataRequest& userData, const std::string& username) override;
    Result<Player> getPlayerByName(const std::string& username) const override;
    std::optional<Error> submitGameStatistics(const GameData& gameData, const std::string &user) override;

    std::optional<Error> removeUser(const std::string &username) override;
    Result<std::string> getUserPasswordByEmail(const std::string &email) override;
private:
    // helper functions
    Result<unsigned int> getQuestionsCount() const;
    std::optional<Error> addQuestion(const QuestionDb& question);
    Result<User> getUser(const std::string& username) const;

    // member variables
    mutable DB _db = create_tables_storage(DATABASE_FILE_PATH);
    mutable std::mutex _dbMutex;

};