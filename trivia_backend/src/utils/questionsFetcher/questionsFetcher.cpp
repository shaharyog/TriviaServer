#include "questionsFetcher.h"
#include "../../constants.h"
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <string>

using Json = nlohmann::json;

Result<std::list<QuestionDb>> QuestionsFetcher::fetchQuestions(unsigned int amount) {
    logServerProgress<QuestionsFetcher>(__func__, "Fetching " + std::to_string(amount) + " questions from OpenTDB...");

    httplib::Client client(OPENTDB_BASE_URL);
    const std::string path = "/api.php?amount=" + std::to_string(amount) + "&type=multiple&difficulty=easy&encode=url3986";
    auto res = client.Get(path);

    if (res && res->status == 200) {
        Json j = Json::parse(res->body, nullptr, false);
        if (j.is_discarded()) {
            logServerResult(false);
            return Error(ErrorType::DeserializationError, "Invalid JSON format");
        }

        const auto questions = deserializeQuestions(j);

        if (questions.isError()) {
            logServerResult(false);
            return questions.error();
        }

        if (questions.value().size() != amount) {
            logServerResult(false);
            return Error(ErrorType::DeserializationError, "Not enough questions available");
        }

        logServerResult(true);
        return questions.value();
    } else {
        logServerResult(false);
        return Error(ErrorType::HttpError, "Failed to fetch questions");
    }
}


Result<std::list<QuestionDb>> QuestionsFetcher::deserializeQuestions(const Json &json) {

    std::list<QuestionDb> questions;

    if (!json.contains("results") || !json["results"].is_array())
        return Error(ErrorType::DeserializationError, "Invalid JSON format");

    for (const auto &question_json: json["results"]) {
        const auto question = deserializeQuestion(question_json);
        if (question.isError())
            return question.error();

        questions.emplace_back(question.value());
    }

    return questions;
}

Result<QuestionDb> QuestionsFetcher::deserializeQuestion(const Json &question) {

    if (!question.contains("question") || !question["question"].is_string() ||
        !question.contains("correct_answer") || !question["correct_answer"].is_string() ||
        !question.contains("incorrect_answers") || !question["incorrect_answers"].is_array())
        return Error(ErrorType::DeserializationError, "Invalid JSON format");

    if (question["incorrect_answers"].size() != 3)
        return Error(ErrorType::DeserializationError, "Invalid JSON format");

    if (!question["incorrect_answers"][0].is_string() || !question["incorrect_answers"][1].is_string() ||
        !question["incorrect_answers"][2].is_string())
        return Error(ErrorType::DeserializationError, "Invalid JSON format");

    return QuestionDb(
            httplib::detail::decode_url(question["question"].get<std::string>(), false),
            httplib::detail::decode_url(question["correct_answer"].get<std::string>(), false),
            httplib::detail::decode_url(question["incorrect_answers"][0], false),
            httplib::detail::decode_url(question["incorrect_answers"][1], false),
            httplib::detail::decode_url(question["incorrect_answers"][2], false)
    );
}