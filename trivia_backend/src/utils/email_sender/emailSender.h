#pragma once


#include <string>
#include <optional>
#include "../../errors/error.h"
#include <nlohmann/json.hpp>

using Json = nlohmann::json;
class EmailSender {
public:
    EmailSender() = delete;
    ~EmailSender() = delete;

    static std::optional<Error> sendEmailVerification(const std::string& recipientEmail, const std::string& verificationCode, const std::string& username);
    static std::optional<Error> sendEmailPasswordRecovery(const std::string& recipientEmail, const std::string& password);
    static std::string generateRandom6DigitCode();
private:
    static std::string createEmailVerificationHtml(const std::string & verificationCode, const std::string& username);
    static std::string createEmailPasswordRecoveryHtml(const std::string & password);
    static Json serializePasswordRecoveryEmailPayload(const std::string& recipientEmail, const std::string& password);
    static Json serializeVerificationEmailPayload(const std::string& recipientEmail, const std::string& verificationCode, const std::string& username);
};