#pragma once
#include <list>
#include "../databaseAccess/QuestionDb.h"
#include "../../errors/result.h"
#include "../conversionHelper/conversionHelper.h"


class QuestionsFetcher {
public:
    QuestionsFetcher() = delete;
    ~QuestionsFetcher() = delete;

    static Result<std::list<QuestionDb>> fetchQuestions(unsigned int amount);

private:
    static Result<std::list<QuestionDb>> deserializeQuestions(const Json& json);

    static Result<QuestionDb> deserializeQuestion(const Json& json);
};
