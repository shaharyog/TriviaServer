#pragma once

#include <string>
#include <regex>

class Validator {
public:
    Validator() = delete;    // Prevent construction
    ~Validator() = delete;   // Prevent destruction

    static std::pair<bool, std::string> isValidUsername(const std::string &username);

    static std::pair<bool, std::string> isValidPassword(const std::string &password);

    static std::pair<bool, std::string> isValidEmail(const std::string &email);

    static std::pair<bool, std::string> isValidAddress(const std::string &address);

    static std::pair<bool, std::string> isValidPhoneNumber(const std::string &phoneNumber);

    static std::pair<bool, std::string> isValidBirthday(const std::string &birthday);

private:
    static std::pair<bool, std::pair<bool, std::string>> isValidBirthdayMatch(const std::regex &birthdayRegex, const std::string &birthday);

    // password regex: must be at least 8 chars, contain at least one uppercase, one lowercase, one number and one special character
    static constexpr auto PASSWORD_REGEX_VALIDATOR = R"((?=.*[A-Z])(?=.*[a-z])(?=.*[0-9])(?=.*[*&^%$#@!]).{8,})";
    // some crazy email regex
    static constexpr auto EMAIL_REGEX_VALIDATOR = R"((?:[a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*|"(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21\x23-\x5b\x5d-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])*")@(?:(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])?|\[(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?|[a-z0-9-]*[a-z0-9]:(?:[\x01-\x08\x0b\x0c\x0e-\x1f\x21-\x5a\x53-\x7f]|\\[\x01-\x09\x0b\x0c\x0e-\x7f])+)\]))";
    // address (Street, Apt, City). Street - must contain only upper and lower case letters and space, Apt - only a number, City - only upper and lower case letters and space
    static constexpr auto ADDRESS_REGEX_VALIDATOR = R"(^[a-zA-Z\s]+, \d+, [a-zA-Z\s]+$)";
    // Israeli phone number only
    static constexpr auto PHONE_NUMBER_REGEX_VALIDATOR = R"(^(?:(?:(\+?972|\(\+?972\)|\+?\(972\))(?:\s|\.|-)?([1-9]\d?))|(0[23489]{1})|(0[57]{1}[0-9]))(?:\s|\.|-)?([^0\D]{1}\d{2}(?:\s|\.|-)?\d{4})$)";
    // dd.mm.yyyy Or dd/mm/yyyy Or dd-mm-yyyy
    static constexpr auto BIRTHDAY_REGEX_VALIDATOR = R"(^(?:(?:31(\/|-|\.)(?:0?[13578]|1[02]))\1|(?:(?:29|30)(\/|-|\.)(?:0?[13-9]|1[0-2])\2))(?:(?:1[6-9]|[2-9]\d)?\d{2})$|^(?:29(\/|-|\.)0?2\3(?:(?:(?:1[6-9]|[2-9]\d)?(?:0[48]|[2468][048]|[13579][26])|(?:(?:16|[2468][048]|[3579][26])00))))$|^(?:0?[1-9]|1\d|2[0-8])(\/|-|\.)(?:(?:0?[1-9])|(?:1[0-2]))\4(?:(?:1[6-9]|[2-9]\d)?\d{2})$)";

    // min password length
    static constexpr auto MIN_PASSWORD_LENGTH = 8;
    static constexpr auto MIN_USERNAME_LENGTH = 3;
};