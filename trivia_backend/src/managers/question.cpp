#include <random>
#include <algorithm>
#include "question.h"

Question::Question(const QuestionDb &questionDb) {
    _possibleAnswers.push_back(questionDb.correct_answer);
    _possibleAnswers.push_back(questionDb.incorrect_answer1);
    _possibleAnswers.push_back(questionDb.incorrect_answer2);
    _possibleAnswers.push_back(questionDb.incorrect_answer3);
    _question = questionDb.question;

    //shuffle the possible answers
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(_possibleAnswers.begin(), _possibleAnswers.end(), g);
    _correctAnswerIndex = static_cast<int>(std::distance(_possibleAnswers.begin(), std::find(_possibleAnswers.begin(), _possibleAnswers.end(), questionDb.correct_answer)));
}