/*
 * NumberGuess.cpp - Number Guessing Game Implementation
 *
 * Game Logic:
 * - Player selects difficulty (Easy/Medium/Hard)
 * - AI generates a random number in [min, max]
 * - Player guesses with hints (higher/lower) - mirrors binary search
 * - Score calculated based on efficiency vs. optimal binary search count
 *
 * Scoring: score = base * (optimal / actual_guesses) * time_bonus
 */

#include "NumberGuess.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>
#include <iomanip>

NumberGuessGame::NumberGuessGame()
    : minRange(1), maxRange(100), secretNumber(0), maxGuesses(10), difficulty(1) {
    srand((unsigned int)time(nullptr));
}

void NumberGuessGame::setDifficulty(int level) {
    difficulty = level;
    switch (level) {
        case 1: // Easy
            minRange = 1; maxRange = 50; maxGuesses = 10;
            break;
        case 2: // Medium
            minRange = 1; maxRange = 200; maxGuesses = 10;
            break;
        case 3: // Hard
            minRange = 1; maxRange = 1000; maxGuesses = 12;
            break;
        default:
            minRange = 1; maxRange = 100; maxGuesses = 10;
    }
}

int NumberGuessGame::optimalGuesses() {
    // Optimal binary search: ceil(log2(range))
    return (int)ceil(log2((double)(maxRange - minRange + 1)));
}

double NumberGuessGame::play() {
    std::cout << "\n==========================================\n";
    std::cout << "       NUMBER GUESSING GAME\n";
    std::cout << "==========================================\n";
    std::cout << "  Uses Binary Search Logic\n\n";

    // Select difficulty
    std::cout << "  Select Difficulty:\n";
    std::cout << "  1. Easy   (1-50,   10 guesses)\n";
    std::cout << "  2. Medium (1-200,  10 guesses)\n";
    std::cout << "  3. Hard   (1-1000, 12 guesses)\n";
    std::cout << "  Choice: ";

    std::string input;
    std::getline(std::cin, input);
    int diff = 1;
    if (input == "2") diff = 2;
    else if (input == "3") diff = 3;
    setDifficulty(diff);

    secretNumber = minRange + rand() % (maxRange - minRange + 1);
    int optimal = optimalGuesses();

    std::cout << "\n  I'm thinking of a number between "
              << minRange << " and " << maxRange << ".\n";
    std::cout << "  Optimal (binary search): " << optimal << " guesses\n";
    std::cout << "  You have " << maxGuesses << " attempts.\n\n";

    // Track the effective binary search range the player should use
    int lo = minRange, hi = maxRange;
    int guessCount = 0;
    bool won = false;

    time_t startTime = time(nullptr);

    while (guessCount < maxGuesses) {
        int suggestedMid = (lo + hi) / 2;
        std::cout << "  [Hint: Binary search suggests " << suggestedMid << "]\n";
        std::cout << "  Guess #" << (guessCount + 1) << ": ";

        std::getline(std::cin, input);
        int guess;
        try {
            guess = std::stoi(input);
        } catch (...) {
            std::cout << "  Invalid input! Enter a number.\n";
            continue;
        }

        guessCount++;

        if (guess == secretNumber) {
            std::cout << "\n  CORRECT! You found the number in "
                      << guessCount << " guesses!\n";
            won = true;
            break;
        } else if (guess < secretNumber) {
            std::cout << "  >> Higher!\n";
            lo = guess + 1;
        } else {
            std::cout << "  >> Lower!\n";
            hi = guess - 1;
        }

        std::cout << "  Remaining: " << (maxGuesses - guessCount) << " guesses\n\n";
    }

    time_t endTime = time(nullptr);
    double timeTaken = difftime(endTime, startTime);

    if (!won) {
        std::cout << "\n  Out of guesses! The number was " << secretNumber << "\n";
    }

    // Score Calculation
    double baseScore = 100.0 * difficulty;
    double efficiencyBonus = (won) ? ((double)optimal / guessCount) : 0.0;
    double timeBonus = (timeTaken < 30) ? 1.2 : (timeTaken < 60) ? 1.0 : 0.8;
    double score = baseScore * efficiencyBonus * timeBonus;

    if (!won) score = baseScore * 0.1; // Consolation points

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "\n  --- Score Breakdown ---\n";
    std::cout << "  Base:       " << baseScore << "\n";
    std::cout << "  Efficiency: x" << efficiencyBonus << "\n";
    std::cout << "  Time Bonus: x" << timeBonus << "\n";
    std::cout << "  Final:      " << score << "\n";
    std::cout << "  -----------------------\n";

    return score;
}

std::string NumberGuessGame::getName() const {
    return "Number Guess";
}
