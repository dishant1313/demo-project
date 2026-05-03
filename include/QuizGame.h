/*
 * QuizGame.h - Quiz Challenge with Trie-based Answer Validation
 *
 * Uses a Trie to store valid answers and perform
 * prefix-based fuzzy matching for partial credit.
 */

#ifndef QUIZ_GAME_H
#define QUIZ_GAME_H

#include <string>
#include <vector>
#include "Trie.h"

struct QuizQuestion {
    std::string question;
    std::string correctAnswer;
    std::vector<std::string> acceptableAnswers; // Synonyms/alternatives
    int points;
    std::string hint;
};

class QuizGame {
private:
    std::vector<QuizQuestion> questions;
    Trie answerTrie; // For fast answer validation and prefix matching

    void loadQuestions();
    bool validateAnswer(const std::string& userAnswer, const QuizQuestion& q);
    std::string toLower(const std::string& str);

public:
    QuizGame();
    double play();
    std::string getName() const;
};

#endif // QUIZ_GAME_H
