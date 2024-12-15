#pragma once
#include <string>
#include <utility>
#include <vector>

struct QuestionDb {
    QuestionDb() = default;
    QuestionDb(std::string  question, std::string  correct_answer, std::string  incorrect_answer1,
               std::string  incorrect_answer2, std::string  incorrect_answer3) : question(std::move(question)),
             correct_answer(std::move(correct_answer)), incorrect_answer1(std::move(incorrect_answer1)), incorrect_answer2(std::move(incorrect_answer2)),
             incorrect_answer3(std::move(incorrect_answer3)) {}

    unsigned int id = 0;
    std::string question;
    std::string correct_answer;
    std::string incorrect_answer1;
    std::string incorrect_answer2;
    std::string incorrect_answer3;
};