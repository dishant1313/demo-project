/*
 * Leaderboard.h - Advanced Leaderboard Logic
 *
 * Implements:
 * 1. Dynamic scoring formula:
 *    score = 0.5 * recent_performance + 0.3 * consistency + 0.2 * game_difficulty_weight
 *
 * 2. Time decay:
 *    new_score = old_score * decay_factor^(days_elapsed)
 *
 * 3. Multi-game weighting:
 *    Different games have different difficulty multipliers
 *
 * 4. Real-time updates across all data structures
 *
 * This is NOT simple score sorting — it's a research-driven adaptive scoring model.
 */

#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <string>
#include <vector>
#include <unordered_map>
#include "AVLTree.h"
#include "MaxHeap.h"
#include "SegmentTree.h"
#include "Trie.h"
#include "SkipList.h"
#include "RedBlackTree.h"
#include "Treap.h"
#include "DisjointSet.h"

// ──────────────────────────────────────────────
// Game weight configuration
// ──────────────────────────────────────────────
enum GameType {
    GAME_NUMBER_GUESS = 0,
    GAME_TIC_TAC_TOE = 1,
    GAME_SNAKE = 2,
    GAME_QUIZ = 3,
    GAME_RACING = 4,
    GAME_COUNT = 5
};

struct GameConfig {
    std::string name;
    double difficultyWeight;  // Multiplier for game difficulty
};

// ──────────────────────────────────────────────
// Player record
// ──────────────────────────────────────────────
struct PlayerRecord {
    std::string username;
    std::vector<double> recentScores;     // Last N raw game scores
    std::vector<long long> scoreTimestamps; // When each score was earned
    double computedScore;                 // The dynamically computed leaderboard score
    int gamesPlayed;
    double consistency;                   // Standard deviation based consistency metric
    std::unordered_map<int, int> gamePlayCount;   // GameType -> times played
    std::unordered_map<int, double> gameTotalScore; // GameType -> total raw score

    PlayerRecord();
    PlayerRecord(const std::string& uname);
};

// ──────────────────────────────────────────────
// Leaderboard System
// ──────────────────────────────────────────────
class Leaderboard {
private:
    // Core data structures
    AVLTree avlTree;
    MaxHeap maxHeap;
    SegmentTree segTree;
    Trie playerTrie;
    SkipList skipList;
    RedBlackTree rbTree;
    Treap treap;
    DisjointSet dsu;

    // Player data
    std::unordered_map<std::string, PlayerRecord> players;

    // Game configurations
    GameConfig gameConfigs[GAME_COUNT];

    // Scoring parameters
    static const double DECAY_FACTOR;
    static const double RECENT_WEIGHT;
    static const double CONSISTENCY_WEIGHT;
    static const double DIFFICULTY_WEIGHT;
    static const int MAX_RECENT_SCORES = 20;
    static const int SEGMENT_TREE_SIZE = 100;

    // File paths
    std::string leaderboardFilePath;

    // Internal scoring methods
    double computeRecentPerformance(const PlayerRecord& player);
    double computeConsistency(const PlayerRecord& player);
    double computeDifficultyWeight(const PlayerRecord& player);
    double computeDynamicScore(const PlayerRecord& player);
    double applyTimeDecay(double score, long long lastTimestamp);

    // Data structure synchronization
    void syncAllStructures(const std::string& username, double oldScore, double newScore);

    // File handling
    void rebuildSegmentTree();

public:
    Leaderboard(const std::string& filePath = "data/leaderboard.txt");
    ~Leaderboard();

    // Core operations
    void addGameResult(const std::string& username, GameType game, double rawScore);
    void registerPlayer(const std::string& username);

    // Query operations
    std::vector<std::pair<std::string, double>> getGlobalLeaderboard();
    std::vector<std::pair<std::string, double>> getTopKPlayers(int k);
    int getPlayerRank(const std::string& username);
    std::vector<std::string> searchPlayer(const std::string& prefix);
    double getRangeScoreSum(int fromIndex, int toIndex);
    double getRangeScoreMax(int fromIndex, int toIndex);

    // Cluster operations
    void clusterPlayers(const std::string& p1, const std::string& p2);
    std::vector<std::vector<std::string>> getPlayerGroups();
    std::string getPlayerGroup(const std::string& username);
    std::vector<std::string> getGroupMembers(const std::string& username);

    // Player info
    PlayerRecord getPlayerRecord(const std::string& username) const;
    bool playerExists(const std::string& username) const;
    double getPlayerScore(const std::string& username) const;

    // Persistence
    void saveLeaderboard();
    void loadLeaderboard();

    // Game config
    std::string getGameName(GameType type) const;
    double getGameWeight(GameType type) const;

    // Auto-cluster by score range
    void autoClusterByScoreRange();
};

#endif // LEADERBOARD_H
