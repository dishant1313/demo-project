/*
 * NumberGuess.h - Number Guessing Game with Binary Search Logic
 *
 * The AI picks a number, and the player uses a binary-search-like
 * approach to guess it. Score is based on:
 * - Number of guesses (fewer = better)
 * - Time taken
 * - Difficulty level (range size)
 */

#ifndef NUMBER_GUESS_H
#define NUMBER_GUESS_H

#include <string>

class NumberGuessGame {
private:
    int minRange;
    int maxRange;
    int secretNumber;
    int maxGuesses;
    int difficulty; // 1=Easy, 2=Medium, 3=Hard

    int optimalGuesses(); // lg(range) - the theoretically optimal binary search count

public:
    NumberGuessGame();

    void setDifficulty(int level);
    double play(); // Returns score
    std::string getName() const;
};

#endif // NUMBER_GUESS_H
