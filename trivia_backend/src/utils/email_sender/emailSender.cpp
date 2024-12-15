#include "emailSender.h"
#include <httplib.h>

std::string EmailSender::createEmailVerificationHtml(const std::string &verificationCode, const std::string &username) {
    return "<!DOCTYPE html>\n"
           "<html lang=\"en\">\n"
           "<head>\n"
           "    <meta charset=\"UTF-8\">\n"
           "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
           "    <title>Email Verification | Trivia Magshimim</title>\n"
           "    <style>\n"
           "        body {\n"
           "            font-family: 'Poppins', sans-serif;\n"
           "            background: linear-gradient(135deg, #4e80fc, #0082c8, #1ad4eb);\n"
           "            padding: 20px;\n"
           "            margin: 0;\n"
           "        }\n"
           "        .container {\n"
           "            background: rgba(255, 255, 255, 0.85);\n"
           "            padding: 40px;\n"
           "            border-radius: 12px;\n"
           "            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);\n"
           "            text-align: center;\n"
           "            max-width: 600px;\n"
           "            margin: 40px auto;\n"
           "        }\n"
           "        h2 {\n"
           "            margin-bottom: 30px;\n"
           "            color: #00678c;\n"
           "            font-size: 32px;\n"
           "            text-transform: uppercase;\n"
           "            letter-spacing: 2px;\n"
           "        }\n"
           "        .username {\n"
           "            font-size: 24px;\n"
           "            font-weight: 600;\n"
           "            color: #333;\n"
           "            margin-bottom: 10px;\n"
           "        }\n"
           "        p {\n"
           "            font-size: 18px;\n"
           "            color: #555;\n"
           "            margin: 30px 0;\n"
           "            line-height: 1.6;\n"
           "        }\n"
           "        .verification-code {\n"
           "            font-size: 40px;\n"
           "            letter-spacing: 12px;\n"
           "            margin: 30px 0;\n"
           "            color: #333;\n"
           "            font-weight: bold;\n"
           "            background: rgba(255, 255, 255, 0.9);\n"
           "            padding: 20px;\n"
           "            border-radius: 8px;\n"
           "            border: 2px solid #00678c;\n"
           "            display: inline-block;\n"
           "        }\n"
           "        .footer {\n"
           "            margin-top: 40px;\n"
           "            font-size: 16px;\n"
           "            color: #888;\n"
           "        }\n"
           "    </style>\n"
           "</head>\n"
           "<body>\n"
           "    <div class=\"container\">\n"
           "        <h2>Email Verification</h2>\n"
           "        <div class=\"username\">Hello, " + username + "!</div>\n"
                                                                  "        <p>Congratulations! You're one step closer to joining the Trivia Magshimim community.</p>\n"
                                                                  "        <p>To complete your email verification, please use the following 6-digit code:</p>\n"
                                                                  "        <div class=\"verification-code\">" + verificationCode + "</div>\n"
                                                                                                                                   "        <p>If you did not request this code, please ignore this email.</p>\n"
                                                                                                                                   "        <div class=\"footer\">\n"
                                                                                                                                   "            Thank you,<br>\n"
                                                                                                                                   "            Trivia Magshimim\n"
                                                                                                                                   "        </div>\n"
                                                                                                                                   "    </div>\n"
                                                                                                                                   "</body>\n"
                                                                                                                                   "</html>";
}



std::optional<Error> EmailSender::sendEmailVerification(const std::string& recipientEmail, const std::string& verificationCode, const std::string& username) {
    httplib::Result res;
    try {
        // Create a client and Authenticate with Basic Auth (apiKey:secretKey)
        httplib::Client client(MAIL_JET_API_URL);
        client.set_basic_auth(std::getenv("MAILJET_APIKEY"), std::getenv("MAILJET_SECRETKEY"));

        // Send the request
        res = client.Post(MAIL_JET_SEND_PATH,
                          serializeVerificationEmailPayload(recipientEmail, verificationCode, username).dump(), "application/json");
    } catch (const std::exception& e) {
        return Error("Error occurred while sending email" + std::string(e.what()));
    }


    if (res && res->status == httplib::StatusCode::OK_200) {
        // Handle successful response
        return std::nullopt;
    } else if (res && res->status == httplib::StatusCode::Unauthorized_401) {
        // Handle authentication error
        std::cerr << "Authentication error: " << res->status << " " << res->reason << std::endl;
        return Error("Authentication error");
    } else if (res && res->status == httplib::StatusCode::BadRequest_400) {
        // Handle bad request error
        std::cerr << "Bad request error: " << res->status << " " << res->reason << std::endl;
        return Error("Bad request error");
    } else {
        // Handle error
        std::cerr << "Failed to send email: " << res->status << " " << res->reason << std::endl;
        return Error("Failed to send email");
    }
}

Json EmailSender::serializeVerificationEmailPayload(const std::string &recipientEmail, const std::string &verificationCode,
                                                    const std::string &username) {
    Json payload;
    payload["Messages"] = Json::array();
    Json message;
    message["From"] = Json::object();
    message["From"]["Email"] = "trivia.magshi@gmail.com";
    message["From"]["Name"] = "Trivia Magshimim";
    message["To"] = Json::array();
    message["To"].push_back(Json::object());
    message["To"][0]["Email"] = recipientEmail;
    message["To"][0]["Name"] = username;
    message["Subject"] = "Verify Your Email for Trivia Magshimim";
    message["HTMLPart"] = createEmailVerificationHtml(verificationCode, username);
    payload["Messages"].push_back(message);

    return payload;
}

std::string EmailSender::generateRandom6DigitCode() {
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution dis(0, 999999); // Define the range

    int randomNumber = dis(gen); // Generate the random number

    // Convert the number to a string with leading zeros if necessary
    std::ostringstream ss;
    ss << std::setw(6) << std::setfill('0') << randomNumber;
    return ss.str();
}

std::optional<Error>
EmailSender::sendEmailPasswordRecovery(const std::string &recipientEmail, const std::string &password) {
    httplib::Result res;
    try {
        // Create a client and Authenticate with Basic Auth (apiKey:secretKey)
        httplib::Client client(MAIL_JET_API_URL);
        client.set_basic_auth(std::getenv("MAILJET_APIKEY"), std::getenv("MAILJET_SECRETKEY"));

        // Send the request
        res = client.Post(MAIL_JET_SEND_PATH, serializePasswordRecoveryEmailPayload(recipientEmail, password).dump(), "application/json");
    } catch (const std::exception& e) {
        return Error("Error occurred while sending email" + std::string(e.what()));
    }


    if (res && res->status == httplib::StatusCode::OK_200) {
        // Handle successful response
        return std::nullopt;
    } else if (res && res->status == httplib::StatusCode::Unauthorized_401) {
        // Handle authentication error
        std::cerr << "Authentication error: " << res->status << " " << res->reason << std::endl;
        return Error("Authentication error");
    } else if (res && res->status == httplib::StatusCode::BadRequest_400) {
        // Handle bad request error
        std::cerr << "Bad request error: " << res->status << " " << res->reason << std::endl;
        return Error("Bad request error");
    } else {
        // Handle error
        std::cerr << "Failed to send email: " << res->status << " " << res->reason << std::endl;
        return Error("Failed to send email");
    }
}

std::string EmailSender::createEmailPasswordRecoveryHtml(const std::string &password) {
    return "<!DOCTYPE html>\n"
           "<html lang=\"en\">\n"
           "<head>\n"
           "    <meta charset=\"UTF-8\">\n"
           "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
           "    <title>Password Recovery | Trivia Magshimim</title>\n"
           "    <style>\n"
           "        body {\n"
           "            font-family: 'Poppins', sans-serif;\n"
           "            padding: 20px;\n"
           "            margin: 0;\n"
           "        }\n"
           "        .container {\n"
           "            background: rgba(255, 255, 255, 0.85);\n"
           "            padding: 40px;\n"
           "            border-radius: 12px;\n"
           "            box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);\n"
           "            text-align: center;\n"
           "            max-width: 600px;\n"
           "            margin: 40px auto;\n"
           "        }\n"
           "        h2 {\n"
           "            margin-bottom: 30px;\n"
           "            color: #00678c;\n"
           "            font-size: 32px;\n"
           "            text-transform: uppercase;\n"
           "            letter-spacing: 2px;\n"
           "        }\n"
           "        p {\n"
           "            font-size: 18px;\n"
           "            color: #555;\n"
           "            margin: 30px 0;\n"
           "            line-height: 1.6;\n"
           "        }\n"
           "        .recovery-password {\n"
           "            font-size: 40px;\n"
           "            letter-spacing: 2px;\n"
           "            margin: 30px 0;\n"
           "            color: #333;\n"
           "            font-weight: bold;\n"
           "            background: rgba(255, 255, 255, 0.9);\n"
           "            padding: 20px;\n"
           "            border-radius: 8px;\n"
           "            border: 2px solid #00678c;\n"
           "            display: inline-block;\n"
           "        }\n"
           "        .footer {\n"
           "            margin-top: 40px;\n"
           "            font-size: 16px;\n"
           "            color: #888;\n"
           "        }\n"
           "    </style>\n"
           "</head>\n"
           "<body>\n"
           "    <div class=\"container\">\n"
           "        <h2>Password Recovery</h2>\n"
           "        <p>It looks like you requested to recover your password. Please find your password below.</p>\n"
           "        <div class=\"recovery-password\">" + password + "</div>\n"
                                                                            "        <p>If you did not request this, please contact our support team immediately.</p>\n"
                                                                            "        <div class=\"footer\">\n"
                                                                            "            Thank you,<br>\n"
                                                                            "            Trivia Magshimim\n"
                                                                            "        </div>\n"
                                                                            "    </div>\n"
                                                                            "</body>\n"
                                                                            "</html>";
}

Json EmailSender::serializePasswordRecoveryEmailPayload(const std::string &recipientEmail, const std::string &password) {
    Json payload;
    payload["Messages"] = Json::array();
    Json message;
    message["From"] = Json::object();
    message["From"]["Email"] = "triviamagshimim@gmail.com";
    message["From"]["Name"] = "Trivia Magshimim";
    message["To"] = Json::array();
    message["To"].push_back(Json::object());
    message["To"][0]["Email"] = recipientEmail;
    message["To"][0]["Name"] ="";
    message["Subject"] = "Password Recovery for Trivia Magshimim";
    message["HTMLPart"] = createEmailPasswordRecoveryHtml(password);
    payload["Messages"].push_back(message);

    return payload;
}


