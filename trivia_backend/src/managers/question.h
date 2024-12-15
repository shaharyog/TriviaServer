#pragma once

#include <string>
#include <vector>
#include "../utils/databaseAccess/QuestionDb.h"

class Question {

private:
    std::string _question;
    std::vector<std::string> _possibleAnswers;
    unsigned int _correctAnswerIndex;

public:
    explicit Question(const QuestionDb& questionDb);
    [[nodiscard]] std::string getQuestion() const { return _question; }

    [[nodiscard]] std::vector<std::string> getPossibleAnswers() const { return _possibleAnswers; }

    [[nodiscard]] unsigned int getCorrectAnswerIndex() const { return _correctAnswerIndex; }

};