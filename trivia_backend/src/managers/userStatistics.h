#pragma once

#include <string>
#include <optional>

struct UserStatistics {
    UserStatistics(std::optional<unsigned> averageAnswerTime, unsigned int numOfCorrectAnswers,
                   unsigned int numOfWrongAnswers, unsigned int numOfTotalGames, unsigned int userScore) :
            averageAnswerTime(averageAnswerTime),
            numOfCorrectAnswers(numOfCorrectAnswers),
            numOfWrongAnswers(numOfWrongAnswers),
            numOfTotalAnswers(numOfCorrectAnswers + numOfWrongAnswers),
            numOfTotalGames(numOfTotalGames),
            userScore(userScore) {}

    std::optional<unsigned int > averageAnswerTime;
    unsigned int numOfCorrectAnswers;
    unsigned int numOfWrongAnswers;
    unsigned int numOfTotalAnswers;
    unsigned int numOfTotalGames;
    unsigned int userScore;
};