#pragma once
constexpr auto DEFAULT_PORT = 8826;
constexpr auto DEFAULT_IP = "0.0.0.0";

// DB Related constants
constexpr auto DATABASE_FILE_PATH = "trivia_db.sqlite";
constexpr auto LOG_FILE_PATH = "trivia.log";
constexpr const char* MAIL_JET_API_URL = "https://api.mailjet.com";
constexpr const char* MAIL_JET_SEND_PATH = "/v3.1/send";
constexpr auto MIN_QUESTIONS_COUNT = 50;

constexpr auto CONFIG_FILE_PATH = "config.json";

// questions fetching API related constants
constexpr auto OPENTDB_BASE_URL = R"(https://opentdb.com)";

// statistics related constants
constexpr auto NUM_OF_TOP_PLAYERS = 50;
constexpr unsigned int CORRECT_ANSWER_POINTS = 30;
constexpr unsigned int INCORRECT_ANSWER_POINTS = 10;
constexpr unsigned int PENALTY_POINTS = 20;