#pragma once


#include <utility>
#include <vector>
#include <map>
#include <shared_mutex>
#include <condition_variable>
#include <numeric>
#include "question.h"
#include "loggedUser.h"
#include "../errors/result.h"
#include "../constants.h"

struct GameData {
    bool isScoreSubmittedToDB = false;
    bool isPunished = false;
    unsigned int timePerQuestion;
    // vector of <question, <answer index, answer time>>
    std::vector<std::pair<Question, std::pair<unsigned int, unsigned int>>> answers;
    explicit GameData(const std::vector<Question>& questions, const unsigned int& timePerQuestion) : timePerQuestion(timePerQuestion) {
        for (auto &question : questions)
            // by default, the player is wrong, and answered at the max time
            answers.emplace_back(question, std::make_pair(5, timePerQuestion));
    }

    [[nodiscard]] unsigned int getNumOfCorrectAnswers() const {
        return std::count_if(answers.cbegin(), answers.cend(), [](const auto &answer) { return answer.second.first == answer.first.getCorrectAnswerIndex(); });
    }

    [[nodiscard]] unsigned int getNumOfWrongAnswers() const {
        return answers.size() - getNumOfCorrectAnswers();
    }

    [[nodiscard]] unsigned int getAverageAnswerTime() const {
        const auto sum = std::accumulate(answers.cbegin(), answers.cend(), 0, [](const auto &sum, const auto &answer) { return sum + answer.second.second; });
        return static_cast<unsigned int>(std::round(sum / answers.size()));
    }

    void submitAnswer(unsigned int answerIndex, unsigned int questionIndex, unsigned int answerTime) {
        answers[questionIndex].second.first = answerIndex;
        answers[questionIndex].second.second = answerTime;
    }

    [[nodiscard]] long long int scoreChange() const {
        long long int addToScore = 0;

        // calculate score to add
        addToScore += static_cast<long long int>(std::round(static_cast<float>(getNumOfCorrectAnswers() * CORRECT_ANSWER_POINTS) /
                                                            (static_cast<float>(getAverageAnswerTime()) / static_cast<float>(timePerQuestion)))) -
                      getNumOfWrongAnswers() * INCORRECT_ANSWER_POINTS;

        // deduct extra points for punishment
        if (isPunished)
            addToScore -= PENALTY_POINTS;   // remove extra points for punishment

        return addToScore;
    }
};


class Game {
public:
    Game(const std::vector<Question>& questions, std::vector<LoggedUser> players, std::string uuid, unsigned int timePerQuestion);
    ~Game() = default;


    std::pair<unsigned int, Question> getCurrentQuestion() const;
    Result<unsigned int> submitAnswer(const LoggedUser& user, unsigned int answerIndex, unsigned int questionIndex);
    void removePlayer(const LoggedUser& user);
    std::map<LoggedUser, GameData> getPlayersResults() const;
    std::vector<LoggedUser> getOnlinePlayers() const;
    bool operator==(const Game& other) const;
    void punishPlayer(const LoggedUser& user);

    std::string getUuid() const { return _uuid; }
    unsigned int getTimePerQuestion() const { return _timePerQuestion; }
    void markUserResultsAsSubmittedToDB(const LoggedUser& user);
    bool isFinished() const;

private:
    std::vector<Question> _questions;
    std::vector<LoggedUser> _onlinePlayers;
    mutable std::shared_mutex _onlinePlayersMutex;
    std::map<LoggedUser, GameData> _players;
    mutable std::shared_mutex _playersMutex;
    unsigned int _timePerQuestion;
    std::string _uuid;

    std::chrono::steady_clock::time_point  _gameStartTime;
};