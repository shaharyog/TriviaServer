#include "sqliteDatabase.h"
#include "../questionsFetcher/questionsFetcher.h"

using namespace sqlite_orm;

SqliteDatabase::SqliteDatabase() {
    try {
        std::lock_guard guard(_dbMutex);
        _db.sync_schema();  // sync schema and create tables if they don't exist
    } catch (const std::exception &e) {
        return;
    }

    const auto questionsCount = getQuestionsCount();
    if (questionsCount.isError())
        return;

    if (questionsCount.value() >= MIN_QUESTIONS_COUNT)
        return;

    const auto questions = QuestionsFetcher::fetchQuestions(MIN_QUESTIONS_COUNT - questionsCount.value());

    if (questions.isError())
        return;

    for (const auto &question: questions.value())
        if (addQuestion(question).has_value()) return;  // stop adding questions if there is an error
}

SqliteDatabase::~SqliteDatabase() {
    try {
        std::lock_guard guard(_dbMutex);
        _db.sync_schema();  // save all changes to disk before exiting
    } catch (const std::exception &e) {
        return;
    }
}


Result<bool> SqliteDatabase::doesUserExist(const std::string &username) const {
    try {
        std::lock_guard sharedLock(_dbMutex);
        const auto users = _db.select(&User::id, where(c(&User::username) == username));
        return !users.empty();
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to check if User exists");
    }
}


Result<bool> SqliteDatabase::doesPasswordMatch(const std::string &userName, const std::string &password) const {
    const auto user = getUser(userName);

    if (user.isError())
        return user.error();

    return user.value().password == password;
}

Result<User> SqliteDatabase::getUser(const std::string &username) const {
    try {
        std::lock_guard sharedLock(_dbMutex);
        const auto user = _db.get_all<User>(where(c(&User::username) == username), limit(1));
        if (user.empty())
            return Error(ErrorType::NotFound, "User not found");
        return user.front();
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get User");
    }
}

std::optional<Error>
SqliteDatabase::addUser(const std::string &userName, const std::string &password, const std::string &email,
                        const std::string &address, const std::string &phoneNumber, const std::string &birthday,
                        const AvatarColor &avatarColor) {
    User user;
    user.username = userName;
    user.password = password;
    user.email = email;
    user.address = address;
    user.phone_number = phoneNumber;
    user.birthday = birthday;
    user.avatar_color = (uint8_t) avatarColor;
    user.member_since = std::time(nullptr);

    const auto doesUserExistRes = doesUserExist(userName);

    if (doesUserExistRes.isError())
        return doesUserExistRes.error();

    if (doesUserExistRes.value())
        return Error(ErrorType::AlreadyExists, "User already exists");

    const auto doesEmailExistRes = doesEmailExist(email);
    if (doesEmailExistRes.isError())
        return doesEmailExistRes.error();

    if (doesEmailExistRes.value())
        return Error(ErrorType::AlreadyExists, "Email already taken");


    std::lock_guard guard(_dbMutex);
    try {
        _db.begin_transaction();  // start transaction
        unsigned int id = _db.insert(user);   // insert new User
        _db.insert(Statistics{id, 0,0,0,0}); // insert new Statistics
        _db.commit();         // commit transaction
        _db.sync_schema();     // save all changes to disk
        return std::nullopt;
    } catch (const std::exception &e) {
        _db.rollback();  // rollback transaction (if user was inserted and statistics was not inserted)
        return Error(ErrorType::Database, "Could not add User to database");
    }
}

Result<bool> SqliteDatabase::doesEmailExist(const std::string &email) const {
    try {
        std::lock_guard sharedLock(_dbMutex);
        const auto usersWithEmail = _db.select(&User::id, where(c(&User::email) == email));
        return !usersWithEmail.empty();
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to check if email exists");
    }
}

Result<std::list<QuestionDb>> SqliteDatabase::getQuestions(unsigned int amount) {

    const auto questionsCount = getQuestionsCount();
    if (questionsCount.isError())
        return questionsCount.error();

    const int minExtraQuestions = 20;
    // always make sure that there are at least 20 questions more than amount to ensure randomness
    if (amount <= questionsCount.value() - minExtraQuestions) {
        try {
            std::lock_guard sharedLock(_dbMutex);
            const auto questions = _db.get_all<QuestionDb>(order_by(sqlite_orm::random()), limit(amount));
            if (questions.size() != amount)
                return Error(ErrorType::Database, "Failed to get questions");

            return std::list<QuestionDb>(questions.begin(), questions.end());
        } catch (const std::exception &e) {
            return Error(ErrorType::Database, "Failed to get questions");
        }
    }

    // if there are not enough questions in the database
    const unsigned int questionsToAdd = amount - (questionsCount.value() - minExtraQuestions);
    const auto questions = QuestionsFetcher::fetchQuestions(questionsToAdd);  // +10 extra questions
    if (questions.isError())
        return questions.error();

    for (const auto &question: questions.value())
        addQuestion(question);

    // try one more time to get enough questions
    // this function could have been recursively called, but to ensure fast responses we try just one more time
    try {
        std::lock_guard sharedLock(_dbMutex);
        const auto random_questions = _db.get_all<QuestionDb>(order_by(sqlite_orm::random()), limit(amount));
        if (random_questions.size() != amount)
            return Error(ErrorType::Database, "Failed to get questions");

        return std::list<QuestionDb>(random_questions.begin(), random_questions.end());
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get questions");
    }
}

Result<unsigned int> SqliteDatabase::getQuestionsCount() const {
    try {
        std::lock_guard sharedLock(_dbMutex);
        return static_cast<unsigned int>(_db.count(&QuestionDb::id));
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get questions count");
    }
}

std::optional<Error> SqliteDatabase::addQuestion(const QuestionDb &question) {
    try {
        std::lock_guard guard(_dbMutex);
        _db.insert(question);
        _db.sync_schema();
        return std::nullopt;
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to add question");
    }
}


Result<std::vector<Player>> SqliteDatabase::getHighScores() const {
    try {
        std::lock_guard sharedLock(_dbMutex);

        // Retrieve all usernames
        const auto players = _db.select(columns(&User::username, &User::avatar_color, &Statistics::score),
                                        inner_join<Statistics>(on(c(&Statistics::user_id) == &User::id)),
                                        order_by(&Statistics::score).desc(), limit(NUM_OF_TOP_PLAYERS));
        std::vector<Player> highScores;
        // transform players to highScores
        std::transform(players.begin(), players.end(), std::back_inserter(highScores), [](const auto &player) {
            const auto colorName = ConversionHelper::avatarColorToString(get<1>(player));
            return Player{get<0>(player), colorName, get<2>(player)};
        });
        return highScores;
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get high scores");
    }

}

std::optional<AvatarColor> colorNameToColor(const std::string &colorName) {
    if (colorName == "Blue")
        return AvatarColor::BLUE;
    else if (colorName == "Green")
        return AvatarColor::GREEN;
    else if (colorName == "Red")
        return AvatarColor::RED;
    else if (colorName == "Purple")
        return AvatarColor::PURPLE;
    else if (colorName == "Orange")
        return AvatarColor::ORANGE;
    else if (colorName == "Cyan")
        return AvatarColor::CYAN;
    else if (colorName == "Teal")
        return AvatarColor::TEAL;
    else if (colorName == "Pink")
        return AvatarColor::PINK;
    else
        return std::nullopt;
}

std::optional<Error> SqliteDatabase::updateUser(const UpdateUserDataRequest &userData, const std::string &username) {
    const auto avatarColor = colorNameToColor(userData.avatarColor);
    if (avatarColor == std::nullopt)
        return Error(ErrorType::InvalidRequest, "Invalid avatar color");

    const auto avatarColorValue = static_cast<uint8_t>(avatarColor.value());

    // update user data lambda that updates the user data or the user data and password
    auto update_user_data = [this, userData, avatarColorValue, username]() {
        if (userData.password.has_value()) {
            _db.update_all(set( c(&User::address) = userData.address,
                               c(&User::phone_number) = userData.phoneNumber,
                               c(&User::password) = userData.password.value(),
                               c(&User::avatar_color) = avatarColorValue),
                           where(c(&User::username) == username));
        } else {
            _db.update_all(set( c(&User::address) = userData.address,
                               c(&User::phone_number) = userData.phoneNumber,
                               c(&User::avatar_color) = avatarColorValue),
                           where(c(&User::username) == username));
        }
    };

    try {
        std::lock_guard guard(_dbMutex);
        update_user_data();
        _db.sync_schema();
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, fmt::format("Failed to update user"));
    }

    return std::nullopt;
}

Result<UserData> SqliteDatabase::getUserData(const std::string &username) const {
    try {
        std::lock_guard sharedLock(_dbMutex);
        const auto userRes = _db.select(columns(&User::username, &User::email, &User::address, &User::phone_number,
                                                &User::birthday, &User::avatar_color, &User::member_since),
                                        where(c(&User::username) == username));

        if (userRes.size() != 1)
            return Error(ErrorType::Database, "Failed to get user data");

        const auto &user = userRes.front();
        const auto colorName = ConversionHelper::avatarColorToString(get<5>(user));
        return UserData{get<0>(user), get<1>(user), get<2>(user), get<3>(user), get<4>(user), colorName, get<6>(user)};
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get user data");
    }
}

Result<Player> SqliteDatabase::getPlayerByName(const std::string &username) const {
    const auto user = getUser(username);
    if (user.isError())
        return user.error();
    try {
        std::lock_guard sharedLock(_dbMutex);
        const auto playerRes = _db.select(columns(&User::username, &User::avatar_color, &Statistics::score),
                                          inner_join<Statistics>(on(c(&Statistics::user_id) == &User::id)),
                                          where(c(&User::id) == user.value().id), limit(1));
        if (playerRes.empty())
            return Error(ErrorType::NotFound, "Player not found");

        const auto &player = playerRes.front();
        const auto colorName = ConversionHelper::avatarColorToString(get<1>(player));
        return Player{get<0>(player), colorName, get<2>(player)};

    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get player by name");
    }
}

Result<std::pair<unsigned int, UserStatistics>> SqliteDatabase::getUserStatistics(const std::string &username) const {
    try {
        std::lock_guard sharedLock(_dbMutex);
        const auto &userStatsRes = _db.select(
                columns(&Statistics::total_games, &Statistics::correct_answers, &Statistics::wrong_answers,
                        &Statistics::avg_answer_time, &Statistics::score, &User::id),
                inner_join<User>(on(c(&User::id) == &Statistics::user_id)), where(c(&User::username) == username),
                limit(1));
        if (userStatsRes.empty())
            return Error(ErrorType::NotFound, "User '" + username + "' has no statistics in database");

        const auto &stats = userStatsRes.front();
        std::optional<unsigned int> avgAnswerTime = std::nullopt;
        if (get<3>(stats) > 0)
            avgAnswerTime = get<3>(stats);

        return std::make_pair(get<5>(stats), UserStatistics{avgAnswerTime,
                                                            get<1>(stats), get<2>(stats), get<0>(stats),
                                                            get<4>(stats)});
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get user statistics");
    }
}

std::optional<Error>
SqliteDatabase::submitGameStatistics(const GameData &gameData,
                                     const std::string &user) {
    const auto userStats = getUserStatistics(user);
    if (userStats.isError())
        return userStats.error();

    unsigned int avgAnswerTime;
    if (userStats.value().second.averageAnswerTime.has_value()) {
        avgAnswerTime = static_cast<unsigned int>(std::round(
                (userStats.value().second.averageAnswerTime.value() * userStats.value().second.numOfTotalAnswers +
                 gameData.getAverageAnswerTime() *
                 (gameData.getNumOfCorrectAnswers() + gameData.getNumOfWrongAnswers())) /
                (userStats.value().second.numOfTotalAnswers +
                 (gameData.getNumOfCorrectAnswers() + gameData.getNumOfWrongAnswers()))));
    } else {
        avgAnswerTime = gameData.getAverageAnswerTime();
    }

    long long int scoreChange = gameData.scoreChange();

    unsigned int newScore = 0;
    if ((static_cast<long long int>(userStats.value().second.userScore) + scoreChange) >= 0)
        newScore = static_cast<unsigned int>(userStats.value().second.userScore + scoreChange);

    try {
        std::lock_guard guard(_dbMutex);

        _db.update_all(set(c(&Statistics::total_games) = userStats.value().second.numOfTotalGames + 1,
                           c(&Statistics::correct_answers) =
                                   userStats.value().second.numOfCorrectAnswers + gameData.getNumOfCorrectAnswers(),
                           c(&Statistics::wrong_answers) =
                                   userStats.value().second.numOfWrongAnswers + gameData.getNumOfWrongAnswers(),
                           c(&Statistics::avg_answer_time) =
                                   avgAnswerTime,
                           c(&Statistics::score) = newScore),
                       where(c(&Statistics::user_id) == userStats.value().first));
        _db.sync_schema();
    }
    catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to submit game statistics");
    }
    return std::nullopt;
}

std::optional<Error> SqliteDatabase::removeUser(const std::string &username) {
    const auto user = getUser(username);
    try {
        std::lock_guard guard(_dbMutex);
        _db.remove<User>(user.value().id);
        _db.sync_schema();
        return std::nullopt;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return Error(ErrorType::Database, "Failed to remove user");
    }
}

Result<std::string> SqliteDatabase::getUserPasswordByEmail(const std::string &email) {
    try {
        std::lock_guard guard(_dbMutex);
        const auto user = _db.get_all<User>(where(c(&User::email) == email), limit(1));
        if (user.empty())
            return Error(ErrorType::NotFound, "User not found");
        return user.front().password;
    } catch (const std::exception &e) {
        return Error(ErrorType::Database, "Failed to get user password");
    }
}

