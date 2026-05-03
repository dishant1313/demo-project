/*
 * main.cpp - Adaptive Multi-Game Multiplayer Leaderboard System
 *
 * SYSTEM ARCHITECTURE:
 * +----------------------------------------------+
 * |                 main.cpp                      |
 * | +----------+  +------------+  +-----------+  |
 * | |   Auth   |  | Leaderboard|  |   Games   |  |
 * | | (Login/  |  | (Dynamic   |  | (5 Games) |  |
 * | |  Signup) |  |  Scoring)  |  |           |  |
 * | +----------+  +------------+  +-----------+  |
 * |        |              |               |       |
 * | +----------------------------------------+   |
 * | |        8 Data Structures               |   |
 * | | AVL | RBTree | Heap | Trie | SegTree   |   |
 * | | Treap | SkipList | DisjointSet         |   |
 * | +----------------------------------------+   |
 * |        |              |                       |
 * | +-------------+  +--------------+             |
 * | |  users.txt  |  |leaderboard.txt|            |
 * | +-------------+  +--------------+             |
 * +----------------------------------------------+
 *
 * COMPILE COMMAND:
 *   g++ -std=c++17 -I include
 *       src/AVLTree.cpp src/RedBlackTree.cpp src/MaxHeap.cpp
 *       src/Trie.cpp src/SegmentTree.cpp src/Treap.cpp
 *       src/SkipList.cpp src/DisjointSet.cpp src/Leaderboard.cpp
 *       auth/Auth.cpp
 *       games/NumberGuess.cpp games/TicTacToe.cpp games/SnakeGame.cpp
 *       games/QuizGame.cpp games/FlappyBird.cpp
 *       main.cpp -o leaderboard_system
 *
 * RUN: ./leaderboard_system
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>

// System modules
#include "Auth.h"
#include "Leaderboard.h"

// Games
#include "NumberGuess.h"
#include "TicTacToe.h"
#include "SnakeGame.h"
#include "QuizGame.h"
#include "FlappyBird.h"

// ══════════════════════════════════════════════
// TERMINAL UI HELPERS
// ══════════════════════════════════════════════
namespace UI {
    void clearScreen() {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
    }

    void printHeader() {
        std::cout << "\n";
        std::cout << "  ==========================================================\n";
        std::cout << "                                                              \n";
        std::cout << "     ADAPTIVE MULTI-GAME MULTIPLAYER LEADERBOARD SYSTEM      \n";
        std::cout << "              Using Advanced Data Structures                  \n";
        std::cout << "                                                              \n";
        std::cout << "   Data Structures: AVL Tree | RB Tree | Max Heap            \n";
        std::cout << "   Trie | Segment Tree | Treap | Skip List | DSU             \n";
        std::cout << "                                                              \n";
        std::cout << "   Scoring: Dynamic (Recent + Consistency + Difficulty)       \n";
        std::cout << "   Features: Time Decay | Multi-Game Weights | Clustering    \n";
        std::cout << "                                                              \n";
        std::cout << "  ==========================================================\n";
    }

    void printMainMenu(const std::string& username) {
        std::cout << "\n";
        std::cout << "  =========================================\n";
        std::cout << "   Logged in as: " << username << "\n";
        std::cout << "  =========================================\n";
        std::cout << "   GAMES\n";
        std::cout << "   1. Number Guess (Binary Search)\n";
        std::cout << "   2. Tic Tac Toe (Minimax AI)\n";
        std::cout << "   3. Snake (Grid + Deque)\n";
        std::cout << "   4. Quiz Challenge (Trie)\n";
        std::cout << "   5. Flappy Bird (PQ)\n";
        std::cout << "  -----------------------------------------\n";
        std::cout << "   LEADERBOARD\n";
        std::cout << "   6. View Global Leaderboard (AVL)\n";
        std::cout << "   7. View Top K Players (Heap)\n";
        std::cout << "   8. Search Player (Trie)\n";
        std::cout << "   9. Get Player Rank (AVL)\n";
        std::cout << "  10. Range Query (Segment Tree)\n";
        std::cout << "  11. View Player Groups (DSU)\n";
        std::cout << "  12. View My Stats\n";
        std::cout << "  -----------------------------------------\n";
        std::cout << "  13. Auto-Cluster Players\n";
        std::cout << "   0. Logout\n";
        std::cout << "  =========================================\n";
        std::cout << "  Choice: ";
    }

    void pressEnter() {
        std::cout << "\n  Press ENTER to continue...";
        std::string dummy;
        std::getline(std::cin, dummy);
    }

    void printSeparator() {
        std::cout << "  ========================================\n";
    }
}

// ══════════════════════════════════════════════
// DISPLAY FUNCTIONS
// ══════════════════════════════════════════════

void displayGlobalLeaderboard(Leaderboard& lb) {
    auto board = lb.getGlobalLeaderboard();

    std::cout << "\n  ==============================================\n";
    std::cout << "          GLOBAL LEADERBOARD\n";
    std::cout << "      (AVL Tree - O(n) in-order traversal)\n";
    std::cout << "  ==============================================\n\n";

    if (board.empty()) {
        std::cout << "  No players on the leaderboard yet.\n";
        return;
    }

    std::cout << "  +------+--------------------+--------------+\n";
    std::cout << "  | Rank | Player             | Score        |\n";
    std::cout << "  +------+--------------------+--------------+\n";

    int rank = 1;
    for (auto& entry : board) {
        std::cout << "  | " << std::setw(4) << rank << " | "
                  << std::setw(18) << std::left << entry.first << " | "
                  << std::setw(12) << std::right << std::fixed << std::setprecision(2)
                  << entry.second << " |\n";
        rank++;
    }
    std::cout << "  +------+--------------------+--------------+\n";
    std::cout << "  Total players: " << board.size() << "\n";
}

void displayTopK(Leaderboard& lb) {
    std::cout << "\n  How many top players? (K): ";
    std::string input;
    std::getline(std::cin, input);
    int k = 5;
    try { k = std::stoi(input); } catch (...) {}

    auto topK = lb.getTopKPlayers(k);

    std::cout << "\n  == Top " << k << " Players (Max Heap - O(k log n)) ==\n\n";

    if (topK.empty()) {
        std::cout << "  No players found.\n";
        return;
    }

    int rank = 1;
    for (auto& entry : topK) {
        std::cout << "  " << rank << ". " << std::setw(15) << std::left
                  << entry.first << "  Score: " << std::fixed
                  << std::setprecision(2) << entry.second << "\n";
        rank++;
    }
}

void searchPlayerMenu(Leaderboard& lb) {
    std::cout << "\n  Enter username or prefix to search: ";
    std::string prefix;
    std::getline(std::cin, prefix);

    auto results = lb.searchPlayer(prefix);

    std::cout << "\n  == Search Results (Trie - O(m + k)) ==\n";
    std::cout << "  Query: \"" << prefix << "\"\n\n";

    if (results.empty()) {
        std::cout << "  No matching players found.\n";
    } else {
        for (auto& name : results) {
            double score = lb.getPlayerScore(name);
            int rank = lb.getPlayerRank(name);
            std::cout << "  * " << name
                      << "  Score: " << std::fixed << std::setprecision(2) << score
                      << "  Rank: " << rank << "\n";
        }
        std::cout << "\n  Found " << results.size() << " matching player(s).\n";
    }
}

void getPlayerRank(Leaderboard& lb) {
    std::cout << "\n  Enter username: ";
    std::string username;
    std::getline(std::cin, username);

    if (!lb.playerExists(username)) {
        std::cout << "  Player not found.\n";
        return;
    }

    int rank = lb.getPlayerRank(username);
    double score = lb.getPlayerScore(username);

    std::cout << "\n  == Player Rank (AVL Tree - O(log n)) ==\n";
    std::cout << "  Player: " << username << "\n";
    std::cout << "  Score:  " << std::fixed << std::setprecision(2) << score << "\n";
    std::cout << "  Rank:   #" << rank << "\n";
}

void rangeQuery(Leaderboard& lb) {
    std::cout << "\n  == Range Query (Segment Tree - O(log n)) ==\n";
    std::cout << "  Enter start index (0-based): ";
    std::string input;
    std::getline(std::cin, input);
    int from = 0;
    try { from = std::stoi(input); } catch (...) {}

    std::cout << "  Enter end index: ";
    std::getline(std::cin, input);
    int to = from;
    try { to = std::stoi(input); } catch (...) {}

    double sum = lb.getRangeScoreSum(from, to);
    double maxVal = lb.getRangeScoreMax(from, to);

    std::cout << "\n  Range [" << from << ", " << to << "]:\n";
    std::cout << "  Sum of scores: " << std::fixed << std::setprecision(2) << sum << "\n";
    std::cout << "  Max score:     " << maxVal << "\n";
    std::cout << "  Average:       " << (to >= from ? sum / (to - from + 1) : 0) << "\n";
}

void displayGroups(Leaderboard& lb) {
    auto groups = lb.getPlayerGroups();

    std::cout << "\n  == Player Groups/Clusters (DSU - O(a(n))) ==\n\n";

    if (groups.empty()) {
        std::cout << "  No groups formed yet.\n";
        return;
    }

    int groupNum = 1;
    for (auto& group : groups) {
        std::string groupName = lb.getPlayerGroup(group[0]);
        std::cout << "  Group " << groupNum << ": " << groupName << "\n";

        for (auto& member : group) {
            double score = lb.getPlayerScore(member);
            std::cout << "    - " << member << " (Score: "
                      << std::fixed << std::setprecision(2) << score << ")\n";
        }
        std::cout << "\n";
        groupNum++;
    }
}

void displayPlayerStats(Leaderboard& lb, const std::string& username) {
    PlayerRecord record = lb.getPlayerRecord(username);

    std::cout << "\n  == Player Statistics ==\n";
    std::cout << "  +--------------------------------------+\n";
    std::cout << "  | Player:      " << std::setw(22) << std::left << record.username << " |\n";
    std::cout << "  | Total Games: " << std::setw(22) << record.gamesPlayed << " |\n";
    std::cout << "  | Score:       " << std::setw(22) << std::fixed << std::setprecision(2)
              << record.computedScore << " |\n";
    std::cout << "  | Consistency: " << std::setw(22) << std::setprecision(1)
              << record.consistency << "%" << " |\n";
    std::cout << "  | Rank:        " << std::setw(22) << lb.getPlayerRank(username) << " |\n";
    std::cout << "  +--------------------------------------+\n";
    std::cout << "  | Game Breakdown:                      |\n";

    for (int g = 0; g < GAME_COUNT; g++) {
        auto it = record.gamePlayCount.find(g);
        int count = (it != record.gamePlayCount.end()) ? it->second : 0;
        auto ts = record.gameTotalScore.find(g);
        double total = (ts != record.gameTotalScore.end()) ? ts->second : 0;

        std::cout << "  |  " << std::setw(20) << std::left << lb.getGameName((GameType)g)
                  << " " << std::setw(3) << count << " games";
        if (count > 0)
            std::cout << " (avg: " << std::setprecision(1) << total / count << ")";
        std::cout << "\n";
    }

    std::cout << "  +--------------------------------------+\n";
    std::cout << "  | Scoring Formula:                     |\n";
    std::cout << "  |  0.5 x RecentPerformance             |\n";
    std::cout << "  |  + 0.3 x Consistency                 |\n";
    std::cout << "  |  + 0.2 x GameDifficultyWeight        |\n";
    std::cout << "  |  x TimeDecay(0.95^days)              |\n";
    std::cout << "  +--------------------------------------+\n";

    // Show group
    std::string group = lb.getPlayerGroup(username);
    if (!group.empty()) {
        std::cout << "  Group: " << group << "\n";
    }
}

// ══════════════════════════════════════════════
// MAIN FUNCTION
// ══════════════════════════════════════════════

int main() {
    Auth auth("data/users.txt");
    Leaderboard leaderboard("data/leaderboard.txt");

    // Game instances
    NumberGuessGame numberGuess;
    TicTacToeGame ticTacToe;
    SnakeGame snake;
    QuizGame quiz;
    FlappyBirdGame flappyBird;

    UI::printHeader();

    // -- Authentication Loop --
    while (!auth.isLoggedIn()) {
        int choice = auth.authMenu();

        if (choice == 0) {
            std::cout << "\n  Goodbye!\n";
            return 0;
        } else if (choice == 1) {
            auth.interactiveLogin();
        } else if (choice == 2) {
            auth.interactiveSignup();
        }
    }

    // Register player in leaderboard system
    leaderboard.registerPlayer(auth.getCurrentUser());

    // -- Main Game Loop --
    bool running = true;
    while (running) {
        UI::printMainMenu(auth.getCurrentUser());

        std::string input;
        std::getline(std::cin, input);
        int choice = 0;
        try { choice = std::stoi(input); } catch (...) { continue; }

        double score = 0;
        GameType gameType;

        switch (choice) {
            // -- GAMES --
            case 1: {
                score = numberGuess.play();
                gameType = GAME_NUMBER_GUESS;
                leaderboard.addGameResult(auth.getCurrentUser(), gameType, score);
                UI::pressEnter();
                break;
            }
            case 2: {
                score = ticTacToe.play();
                gameType = GAME_TIC_TAC_TOE;
                leaderboard.addGameResult(auth.getCurrentUser(), gameType, score);
                UI::pressEnter();
                break;
            }
            case 3: {
                score = snake.play();
                gameType = GAME_SNAKE;
                leaderboard.addGameResult(auth.getCurrentUser(), gameType, score);
                UI::pressEnter();
                break;
            }
            case 4: {
                score = quiz.play();
                gameType = GAME_QUIZ;
                leaderboard.addGameResult(auth.getCurrentUser(), gameType, score);
                UI::pressEnter();
                break;
            }
            case 5: {
                score = flappyBird.play();
                gameType = GAME_RACING;
                leaderboard.addGameResult(auth.getCurrentUser(), gameType, score);
                UI::pressEnter();
                break;
            }

            // -- LEADERBOARD FEATURES --
            case 6:
                displayGlobalLeaderboard(leaderboard);
                UI::pressEnter();
                break;

            case 7:
                displayTopK(leaderboard);
                UI::pressEnter();
                break;

            case 8:
                searchPlayerMenu(leaderboard);
                UI::pressEnter();
                break;

            case 9:
                getPlayerRank(leaderboard);
                UI::pressEnter();
                break;

            case 10:
                rangeQuery(leaderboard);
                UI::pressEnter();
                break;

            case 11:
                displayGroups(leaderboard);
                UI::pressEnter();
                break;

            case 12:
                displayPlayerStats(leaderboard, auth.getCurrentUser());
                UI::pressEnter();
                break;

            case 13:
                std::cout << "\n  Auto-clustering players by score tiers...\n";
                leaderboard.autoClusterByScoreRange();
                std::cout << "  Done! Players grouped into tiers.\n";
                displayGroups(leaderboard);
                UI::pressEnter();
                break;

            case 0:
                leaderboard.saveLeaderboard();
                auth.logout();
                running = false;
                break;

            default:
                std::cout << "  Invalid choice. Try again.\n";
        }
    }

    std::cout << "\n  == Thank you for playing! ==\n";
    std::cout << "  Data saved to data/leaderboard.txt\n\n";

    return 0;
}
