#include "validator.h"
#include "../../errors/result.h"
#include <regex>
#include <nlohmann/json.hpp>
#include <date/date.h>

using Json = nlohmann::json;

std::pair<bool, std::string> Validator::isValidUsername(const std::string &username) {
    if (username.length() >= MIN_USERNAME_LENGTH)
        return {true, ""};
    else
        return {false, "Username must be at least " + std::to_string(MIN_USERNAME_LENGTH) + " characters long"};
}

std::pair<bool, std::string> Validator::isValidPassword(const std::string &password) {
    if (password.length() < MIN_PASSWORD_LENGTH)
        return {false, "Password must be at least " + std::to_string(MIN_PASSWORD_LENGTH) + " characters long"};

    // check password min requirements using regex
    std::regex passwordRegex(PASSWORD_REGEX_VALIDATOR);
    if (!std::regex_match(password, passwordRegex))
        return {false,
                "Password must contain at least one uppercase, one lowercase, one number and one special character"};
    return {true, ""};
}

std::pair<bool, std::string> Validator::isValidEmail(const std::string &email) {
    std::regex emailRegex(EMAIL_REGEX_VALIDATOR);
    std::string emailLower(email);
    std::transform(emailLower.begin(), emailLower.end(), emailLower.begin(), ::tolower);
    if (std::regex_match(emailLower, emailRegex))
        return {true, ""};
    else
        return {false, "Invalid email format. Should be a valid email address"};
}

std::pair<bool, std::string> Validator::isValidAddress(const std::string &address) {
    std::regex addressRegex(ADDRESS_REGEX_VALIDATOR);
    if (std::regex_match(address, addressRegex))
        return {true, ""};
    else
        return {false,
                "Invalid address format. Should be Street, Apt, City. Street and City must contain only upper and lower case letters and space, Apt - only a number"};
}

std::pair<bool, std::string> Validator::isValidPhoneNumber(const std::string &phoneNumber) {
    std::regex phoneNumberRegex(PHONE_NUMBER_REGEX_VALIDATOR);
    if (std::regex_match(phoneNumber, phoneNumberRegex))
        return {true, ""};
    else
        return {false, "Invalid phone number format. Should be a legal Israeli phone number"};
}

std::pair<bool, std::string> Validator::isValidBirthday(const std::string &birthday) {
    // Define regular expressions for the accepted date formats
    std::regex dateRegex1(R"(^(\d{1,2})\.(\d{1,2})\.(\d{4})$)");
    std::regex dateRegex2(R"(^(\d{1,2})/(\d{1,2})/(\d{4})$)");
    std::regex dateRegex3(R"(^(\d{1,2})-(\d{1,2})-(\d{4})$)");

    const auto isValidBirthdayMatch1 = isValidBirthdayMatch(dateRegex1, birthday);
    const auto isValidBirthdayMatch2 = isValidBirthdayMatch(dateRegex2, birthday);
    const auto isValidBirthdayMatch3 = isValidBirthdayMatch(dateRegex3, birthday);

    std::string errorMessage;
    if (isValidBirthdayMatch1.first)
    {
        if (isValidBirthdayMatch1.second.first)
            return {true, ""};

        errorMessage = isValidBirthdayMatch1.second.second;
    }
    else if (isValidBirthdayMatch2.first)
    {
        if (isValidBirthdayMatch2.second.first)
            return {true, ""};

        errorMessage = isValidBirthdayMatch2.second.second;
    }
    else if (isValidBirthdayMatch3.first)
    {
        if (isValidBirthdayMatch3.second.first)
            return {true, ""};

        errorMessage = isValidBirthdayMatch3.second.second;
    }
    else
        errorMessage = "Invalid birthday format. Should be d.m.yyyy, d/m/yyyy or d-m-yyyy";

    return {false, errorMessage};
}

// returns: <isMatched, <isBirthdayValid, message>>
std::pair<bool, std::pair<bool, std::string>> Validator::isValidBirthdayMatch(const std::regex &birthdayRegex, const std::string &birthday) {
    auto today = date::floor<date::days>(std::chrono::system_clock::now());
    std::smatch match;

    if (std::regex_match(birthday, match, birthdayRegex)) {
        if (!match[1].matched || !match[2].matched || !match[3].matched)
            return {false, {false, "Invalid birthday format. Should be d.m.yyyy, d/m/yyyy or d-m-yyyy."}};

        unsigned int day = std::stoi(match[1]);
        unsigned int month = std::stoi(match[2]);
        int year = std::stoi(match[3]);

        const auto birthdayDate = date::year{year} / date::month{month} / date::day{day};

        // check if the birthday is valid and in the past
        if (!birthdayDate.ok())
            return {true, {false, "Invalid birthday. Date should be a legal date."}};
        else if (birthdayDate > today - date::days{365} * 16)
            return {true, {false, "Invalid birthday. Must be older than 16 years."}};
        else if (birthdayDate < today - date::days {365} * 150)
            return {true, {false, "Invalid birthday. Must be younger than 150 years."}};
        else
            return {true, {true, ""}};
    }

    return {false, {false, "Invalid birthday format. Should be d.m.yyyy, d/m/yyyy or d-m-yyyy."}};
}
