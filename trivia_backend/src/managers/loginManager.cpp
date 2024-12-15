#include <algorithm>
#include "loginManager.h"
#include "../utils/validator/validator.h"
#include "../utils/email_sender/emailSender.h"
#include <random>

Result<std::pair<bool, std::string>>
LoginManager::signup(const std::string &username, const std::string &password, const std::string &email,
                     const std::string &address, const std::string &phoneNumber, const std::string &birthday) {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database);

    const auto isValidSignupCredentialsRes = isValidSignupCredentials(username, password, email, address, phoneNumber,
                                                                      birthday);
    if (!isValidSignupCredentialsRes.first)
        return std::make_pair(false, isValidSignupCredentialsRes.second);

    const AvatarColor avatarColor = getRandomAvatarColor();
    // The addUser function checks and ensures that the User does not already exist
    const auto addUser = db_ptr->addUser(username, password, email, address, phoneNumber, birthday, avatarColor);
    if (addUser.has_value())    // error
    {
        if (addUser.value().type != ErrorType::Database)
            return std::make_pair(false, addUser.value().message);

        return addUser.value();
    }

    return login(username, password);    // success. log him in
}


Result<std::pair<bool, std::string>> LoginManager::login(const std::string &username, const std::string &password) {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database, "Database Not Initialized");

    const auto doesUserExist = db_ptr->doesUserExist(username);
    if (doesUserExist.isError())
        return doesUserExist.error();
    if (!doesUserExist.value())
        return std::make_pair(false, "User does not exist");

    const auto doesPasswordMatch = db_ptr->doesPasswordMatch(username, password);
    if (doesPasswordMatch.isError())
        return doesPasswordMatch.error();
    if (!doesPasswordMatch.value())
        return std::make_pair(false, "Password does not match");

    // lock the logged users list
    std::lock_guard guard(_loggedUsersMutex);

    const auto doesUserAlreadyLoggedIn = [username](const LoggedUser &user) { return user.username == username; };
    if (std::any_of(_loggedUsers.cbegin(), _loggedUsers.cend(), doesUserAlreadyLoggedIn))
        return std::make_pair(false, "User already logged in");

    _loggedUsers.push_back({username});

    return std::make_pair(true, "");    // success
}

void LoginManager::logout(const std::string &username) {
    std::lock_guard guard(_loggedUsersMutex);

    _loggedUsers.erase(std::remove_if(_loggedUsers.begin(), _loggedUsers.end(),
                                      [username](const LoggedUser &user) { return user.username == username; }),
                       _loggedUsers.end());
}

std::pair<bool, std::string>
LoginManager::isValidSignupCredentials(const std::string &username, const std::string &password,
                                       const std::string &email, const std::string &address,
                                       const std::string &phoneNumber, const std::string &birthday) {
    // validate everything
    const auto isValidUsername = Validator::isValidUsername(username);
    if (!isValidUsername.first)
        return {false, isValidUsername.second};

    const auto isValidEmail = Validator::isValidEmail(email);
    if (!isValidEmail.first)
        return {false, isValidEmail.second};

    const auto isValidAddress = Validator::isValidAddress(address);
    if (!isValidAddress.first)
        return {false, isValidAddress.second};

    const auto isValidPhoneNumber = Validator::isValidPhoneNumber(phoneNumber);
    if (!isValidPhoneNumber.first)
        return {false, isValidPhoneNumber.second};

    const auto isValidBirthday = Validator::isValidBirthday(birthday);
    if (!isValidBirthday.first)
        return {false, isValidBirthday.second};

    // check password lastly, because it uses the web to check strength and require more resources
    const auto isValidPassword = Validator::isValidPassword(password);
    if (!isValidPassword.first)
        return {false, isValidPassword.second};

    return {true, ""};
}

AvatarColor LoginManager::getRandomAvatarColor() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int randomIndex =
            std::rand() % (static_cast <int>(AvatarColor::PINK) + 1); // generate a random number between 0 and PINK

    auto randomColor = static_cast<AvatarColor>(randomIndex);

    return randomColor;
}

Result<bool> LoginManager::forgotPassword(const std::string &email) {
    const auto db_ptr = _database.lock();
    if (db_ptr == nullptr)
        return Error(ErrorType::Database);

    const auto password = db_ptr->getUserPasswordByEmail(email);
    if (password.isError())
        return false;

    const auto emailRes = EmailSender::sendEmailPasswordRecovery(email, password.value());
    if (emailRes.has_value())
        return emailRes.value();

    return true;
}
