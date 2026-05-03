/*
 * QuizGame.cpp - Quiz Challenge Implementation
 *
 * Trie Integration:
 * - All acceptable answers for each question are inserted into a Trie
 * - User input is validated via Trie search (exact match = full points)
 * - Prefix matching gives partial credit
 * - Case-insensitive comparison
 *
 * Questions cover CS/DSA topics for thematic consistency.
 */

#include "QuizGame.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <iomanip>

QuizGame::QuizGame() {
    loadQuestions();
}

std::string QuizGame::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void QuizGame::loadQuestions() {
    questions = {
        {
            "What data structure uses LIFO (Last In, First Out)?",
            "stack",
            {"stack", "stacks"},
            20,
            "Think of a pile of plates..."
        },
        {
            "What is the time complexity of binary search?",
            "o(log n)",
            {"o(log n)", "log n", "o(logn)", "logn", "logarithmic"},
            25,
            "Each step halves the search space..."
        },
        {
            "What self-balancing BST uses red and black node colors?",
            "red black tree",
            {"red black tree", "red-black tree", "rb tree", "rbtree"},
            30,
            "Named after colors..."
        },
        {
            "What algorithm finds the shortest path in a weighted graph?",
            "dijkstra",
            {"dijkstra", "dijkstras", "dijkstra's"},
            25,
            "Named after a Dutch computer scientist..."
        },
        {
            "What data structure is used for prefix-based string searches?",
            "trie",
            {"trie", "prefix tree", "digital tree"},
            20,
            "Used in autocomplete..."
        },
        {
            "What technique divides a problem into smaller subproblems and caches results?",
            "dynamic programming",
            {"dynamic programming", "dp", "memoization"},
            30,
            "Not greedy, not brute force..."
        },
        {
            "What is the worst-case time complexity of QuickSort?",
            "o(n^2)",
            {"o(n^2)", "n squared", "o(n2)", "n^2", "quadratic"},
            25,
            "Happens with already sorted input and bad pivot..."
        },
        {
            "What data structure uses both tree and heap properties?",
            "treap",
            {"treap", "tree heap"},
            30,
            "Combination of tree and heap..."
        },
        {
            "What is the optimal data structure for finding the K-th smallest element?",
            "heap",
            {"heap", "min heap", "max heap", "priority queue"},
            20,
            "A partially ordered tree..."
        },
        {
            "What graph traversal algorithm uses a queue?",
            "bfs",
            {"bfs", "breadth first search", "breadth-first search"},
            20,
            "Level by level exploration..."
        }
    };
}

bool QuizGame::validateAnswer(const std::string& userAnswer, const QuizQuestion& q) {
    // Build a temporary trie with all acceptable answers
    Trie tempTrie;
    for (const auto& ans : q.acceptableAnswers) {
        tempTrie.insert(toLower(ans));
    }

    std::string lowerInput = toLower(userAnswer);

    // Exact match via Trie search
    if (tempTrie.search(lowerInput)) return true;

    // Check if user's answer is a valid prefix of any acceptable answer
    auto matches = tempTrie.autocomplete(lowerInput);
    return !matches.empty();
}

double QuizGame::play() {
    std::cout << "\n==========================================\n";
    std::cout << "       QUIZ CHALLENGE\n";
    std::cout << "==========================================\n";
    std::cout << "  Trie-based Answer Validation\n";
    std::cout << "  Type your answer or a prefix!\n";
    std::cout << "  Press 'h' for hints\n";
    std::cout << "==========================================\n";

    // Shuffle and pick 5 questions
    std::vector<int> indices;
    for (int i = 0; i < (int)questions.size(); i++) indices.push_back(i);
    srand((unsigned int)time(nullptr));
    for (int i = (int)indices.size() - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        std::swap(indices[i], indices[j]);
    }

    int numQuestions = std::min(5, (int)questions.size());
    double totalScore = 0;
    int correct = 0;
    int partial = 0;

    for (int q = 0; q < numQuestions; q++) {
        const QuizQuestion& question = questions[indices[q]];

        std::cout << "\n  --- Question " << (q + 1) << "/" << numQuestions
                  << " (" << question.points << " pts) ---\n";
        std::cout << "  " << question.question << "\n";
        std::cout << "  Answer (or 'h' for hint): ";

        std::string answer;
        std::getline(std::cin, answer);

        // Hint system
        if (toLower(answer) == "h") {
            std::cout << "  Hint: " << question.hint << "\n";
            std::cout << "  Answer: ";
            std::getline(std::cin, answer);
        }

        std::string lowerAnswer = toLower(answer);

        // Build trie and validate
        Trie validationTrie;
        for (const auto& acceptable : question.acceptableAnswers) {
            validationTrie.insert(toLower(acceptable));
        }

        if (validationTrie.search(lowerAnswer)) {
            // Exact match - full points
            std::cout << "  CORRECT! +" << question.points << " points\n";
            totalScore += question.points;
            correct++;
        } else {
            // Check prefix match for partial credit
            auto matches = validationTrie.autocomplete(lowerAnswer);
            if (!matches.empty() && lowerAnswer.length() >= 3) {
                int partialPoints = question.points / 2;
                std::cout << "  PARTIAL! Your answer was close. +"
                          << partialPoints << " points\n";
                std::cout << "  Full answer: " << question.correctAnswer << "\n";
                totalScore += partialPoints;
                partial++;
            } else {
                std::cout << "  WRONG! Answer: " << question.correctAnswer << "\n";
            }
        }
    }

    std::cout << "\n  --- Quiz Results ---\n";
    std::cout << "  Correct:  " << correct << "/" << numQuestions << "\n";
    std::cout << "  Partial:  " << partial << "\n";
    std::cout << "  Score:    " << std::fixed << std::setprecision(0)
              << totalScore << "\n";
    std::cout << "  ---------------------\n";

    return totalScore;
}

std::string QuizGame::getName() const {
    return "Quiz Challenge";
}
