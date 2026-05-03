/*
 * Leaderboard.cpp - Advanced Leaderboard Implementation
 *
 * RESEARCH-DRIVEN SCORING MODEL:
 *
 * Dynamic Score = 0.5 * RecentPerformance + 0.3 * Consistency + 0.2 * DifficultyWeight
 *
 * Where:
 *   RecentPerformance = weighted average of last N scores (more recent = higher weight)
 *   Consistency = 100 - normalized_stddev (lower variance = higher consistency)
 *   DifficultyWeight = weighted average of game difficulty multipliers
 *
 * Time Decay: score *= 0.95^(days_since_last_game)
 *
 * Every score update synchronizes across ALL data structures:
 *   AVL Tree, Red-Black Tree, Max Heap, Skip List, Treap, Segment Tree, Trie, DSU
 */

#include "Leaderboard.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <iomanip>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

// Static member definitions
const double Leaderboard::DECAY_FACTOR = 0.95;
const double Leaderboard::RECENT_WEIGHT = 0.5;
const double Leaderboard::CONSISTENCY_WEIGHT = 0.3;
const double Leaderboard::DIFFICULTY_WEIGHT = 0.2;
const int Leaderboard::MAX_RECENT_SCORES;
const int Leaderboard::SEGMENT_TREE_SIZE;

// ──────────────────────────────────────────────
// PlayerRecord
// ──────────────────────────────────────────────
PlayerRecord::PlayerRecord()
    : username(""), computedScore(0), gamesPlayed(0), consistency(100.0) {}

PlayerRecord::PlayerRecord(const std::string& uname)
    : username(uname), computedScore(0), gamesPlayed(0), consistency(100.0) {}

// ──────────────────────────────────────────────
// Leaderboard Constructor
// ──────────────────────────────────────────────
Leaderboard::Leaderboard(const std::string& filePath)
    : leaderboardFilePath(filePath) {

    MKDIR("data");

    // Configure game difficulty weights
    gameConfigs[GAME_NUMBER_GUESS] = {"Number Guess",     0.6};
    gameConfigs[GAME_TIC_TAC_TOE]  = {"Tic Tac Toe",      0.9};
    gameConfigs[GAME_SNAKE]        = {"Snake",             1.0};
    gameConfigs[GAME_QUIZ]         = {"Quiz Challenge",    0.8};
    gameConfigs[GAME_RACING]       = {"Flappy Bird",       0.85};

    // Initialize segment tree
    segTree.resize(SEGMENT_TREE_SIZE);

    // Load existing data
    loadLeaderboard();
}

Leaderboard::~Leaderboard() {
    saveLeaderboard();
}

// ══════════════════════════════════════════════
// SCORING ALGORITHMS
// ══════════════════════════════════════════════

/*
 * Compute Recent Performance
 * Uses exponential weighting: more recent scores have higher weight.
 * Formula: Σ(score_i * weight_i) / Σ(weight_i)
 * where weight_i = 2^(i/N) for position i in chronological order
 */
double Leaderboard::computeRecentPerformance(const PlayerRecord& player) {
    if (player.recentScores.empty()) return 0.0;

    double weightedSum = 0.0;
    double weightSum = 0.0;
    int n = (int)player.recentScores.size();

    for (int i = 0; i < n; i++) {
        // Exponential weight: most recent scores get highest weight
        double weight = pow(2.0, (double)i / n);
        weightedSum += player.recentScores[i] * weight;
        weightSum += weight;
    }

    return (weightSum > 0) ? (weightedSum / weightSum) : 0.0;
}

/*
 * Compute Consistency
 * Based on coefficient of variation (CV) of recent scores.
 * Lower CV = higher consistency.
 * Returns value in [0, 100] range.
 */
double Leaderboard::computeConsistency(const PlayerRecord& player) {
    if (player.recentScores.size() < 2) return 100.0; // Perfect consistency with 1 game

    double mean = 0.0;
    for (double s : player.recentScores) mean += s;
    mean /= player.recentScores.size();

    if (mean == 0) return 50.0;

    double variance = 0.0;
    for (double s : player.recentScores) {
        variance += (s - mean) * (s - mean);
    }
    variance /= player.recentScores.size();

    double stddev = sqrt(variance);
    double cv = (stddev / mean) * 100.0; // Coefficient of variation

    // Map CV to consistency: CV=0 → 100, CV≥100 → 0
    double consistency = std::max(0.0, 100.0 - cv);
    return consistency;
}

/*
 * Compute Difficulty Weight
 * Weighted average of difficulty multipliers across all games played.
 * Players who play harder games get a bonus.
 */
double Leaderboard::computeDifficultyWeight(const PlayerRecord& player) {
    if (player.gamesPlayed == 0) return 0.0;

    double totalWeight = 0.0;
    int totalGames = 0;

    for (auto& pair : player.gamePlayCount) {
        int gameType = pair.first;
        int count = pair.second;
        if (gameType >= 0 && gameType < GAME_COUNT) {
            totalWeight += gameConfigs[gameType].difficultyWeight * count;
            totalGames += count;
        }
    }

    return (totalGames > 0) ? (totalWeight / totalGames) * 100.0 : 50.0;
}

/*
 * Compute Dynamic Score
 * Combines all three components with time decay.
 *
 * Formula:
 *   base = 0.5 * recent_performance + 0.3 * consistency + 0.2 * difficulty_weight
 *   final = base * decay_factor^(days_since_last_game)
 */
double Leaderboard::computeDynamicScore(const PlayerRecord& player) {
    double recent = computeRecentPerformance(player);
    double consistency = computeConsistency(player);
    double difficulty = computeDifficultyWeight(player);

    double baseScore = RECENT_WEIGHT * recent +
                       CONSISTENCY_WEIGHT * consistency +
                       DIFFICULTY_WEIGHT * difficulty;

    // Apply time decay
    if (!player.scoreTimestamps.empty()) {
        baseScore = applyTimeDecay(baseScore, player.scoreTimestamps.back());
    }

    return baseScore;
}

/*
 * Time Decay Function
 * Reduces score based on time elapsed since last activity.
 * new_score = old_score * 0.95^(days_elapsed)
 */
double Leaderboard::applyTimeDecay(double score, long long lastTimestamp) {
    long long now = (long long)time(nullptr);
    double daysDiff = (double)(now - lastTimestamp) / 86400.0;

    if (daysDiff < 0) daysDiff = 0;
    if (daysDiff > 365) daysDiff = 365; // Cap at 1 year

    return score * pow(DECAY_FACTOR, daysDiff);
}

// ══════════════════════════════════════════════
// DATA STRUCTURE SYNCHRONIZATION
// ══════════════════════════════════════════════

/*
 * Syncs all data structures when a player's score changes.
 * Removes old entry and inserts new entry in:
 * AVL Tree, Red-Black Tree, Max Heap, Skip List, Treap
 */
void Leaderboard::syncAllStructures(const std::string& username,
                                      double oldScore, double newScore) {
    // Remove old entries
    if (oldScore > 0) {
        avlTree.remove(username, oldScore);
        rbTree.remove(username, oldScore);
        skipList.remove(username, oldScore);
        treap.remove(username, oldScore);
    }

    // Insert new entries
    avlTree.insert(username, newScore);
    rbTree.insert(username, newScore);
    skipList.insert(username, newScore);
    treap.insert(username, newScore);

    // Rebuild heap from current data
    maxHeap.clear();
    for (auto& pair : players) {
        maxHeap.insert(pair.first, pair.second.computedScore);
    }
}

void Leaderboard::rebuildSegmentTree() {
    // Build segment tree from all player scores
    auto leaderboard = avlTree.getSortedLeaderboard();
    if (leaderboard.empty()) return;

    int sz = std::min((int)leaderboard.size(), SEGMENT_TREE_SIZE);
    std::vector<double> scores(sz, 0.0);
    for (int i = 0; i < sz; i++) {
        scores[i] = leaderboard[i].second;
    }
    segTree.build(scores);
}

// ══════════════════════════════════════════════
// CORE OPERATIONS
// ══════════════════════════════════════════════

void Leaderboard::registerPlayer(const std::string& username) {
    if (players.find(username) != players.end()) return;

    players[username] = PlayerRecord(username);
    playerTrie.insert(username);
    dsu.makeSet(username);

    // Immediately persist new player registration
    saveLeaderboard();
}

void Leaderboard::addGameResult(const std::string& username, GameType game, double rawScore) {
    // Ensure player exists
    registerPlayer(username);

    PlayerRecord& player = players[username];
    double oldScore = player.computedScore;

    // Update game statistics
    player.gamesPlayed++;
    player.gamePlayCount[game]++;
    player.gameTotalScore[game] += rawScore;

    // Apply game difficulty multiplier to raw score
    double adjustedScore = rawScore * gameConfigs[game].difficultyWeight;

    // Add to recent scores (sliding window)
    player.recentScores.push_back(adjustedScore);
    player.scoreTimestamps.push_back((long long)time(nullptr));

    if ((int)player.recentScores.size() > MAX_RECENT_SCORES) {
        player.recentScores.erase(player.recentScores.begin());
        player.scoreTimestamps.erase(player.scoreTimestamps.begin());
    }

    // Recompute dynamic score
    double newScore = computeDynamicScore(player);
    player.computedScore = newScore;
    player.consistency = computeConsistency(player);

    // Synchronize all data structures
    syncAllStructures(username, oldScore, newScore);
    rebuildSegmentTree();

    std::cout << "[SCORE] " << username << " played " << gameConfigs[game].name
              << " | Raw: " << std::fixed << std::setprecision(1) << rawScore
              << " | Dynamic Score: " << std::setprecision(2) << newScore << "\n";

    // Immediately persist to file so data is never lost
    saveLeaderboard();
}

// ══════════════════════════════════════════════
// QUERY OPERATIONS
// ══════════════════════════════════════════════

std::vector<std::pair<std::string, double>> Leaderboard::getGlobalLeaderboard() {
    return avlTree.getSortedLeaderboard();
}

std::vector<std::pair<std::string, double>> Leaderboard::getTopKPlayers(int k) {
    auto heapResult = maxHeap.getTopK(k);
    std::vector<std::pair<std::string, double>> result;
    for (auto& entry : heapResult) {
        result.push_back({entry.username, entry.score});
    }
    return result;
}

int Leaderboard::getPlayerRank(const std::string& username) {
    if (players.find(username) == players.end()) return -1;
    return avlTree.getRank(username, players[username].computedScore);
}

std::vector<std::string> Leaderboard::searchPlayer(const std::string& prefix) {
    return playerTrie.autocomplete(prefix);
}

double Leaderboard::getRangeScoreSum(int fromIndex, int toIndex) {
    return segTree.querySum(fromIndex, toIndex);
}

double Leaderboard::getRangeScoreMax(int fromIndex, int toIndex) {
    return segTree.queryMax(fromIndex, toIndex);
}

// ══════════════════════════════════════════════
// CLUSTER OPERATIONS
// ══════════════════════════════════════════════

void Leaderboard::clusterPlayers(const std::string& p1, const std::string& p2) {
    dsu.makeSet(p1);
    dsu.makeSet(p2);
    dsu.unionSets(p1, p2);
}

std::vector<std::vector<std::string>> Leaderboard::getPlayerGroups() {
    return dsu.getAllGroups();
}

std::string Leaderboard::getPlayerGroup(const std::string& username) {
    return dsu.getGroupName(username);
}

std::vector<std::string> Leaderboard::getGroupMembers(const std::string& username) {
    return dsu.getGroupMembers(username);
}

// ══════════════════════════════════════════════
// AUTO CLUSTERING
// Groups players into tiers based on score ranges
// ══════════════════════════════════════════════
void Leaderboard::autoClusterByScoreRange() {
    auto leaderboard = getGlobalLeaderboard();
    if (leaderboard.empty()) return;

    // Define tiers
    for (auto& entry : leaderboard) {
        dsu.makeSet(entry.first);
    }

    // Cluster adjacent players with similar scores (within 20% range)
    for (size_t i = 1; i < leaderboard.size(); i++) {
        double prev = leaderboard[i-1].second;
        double curr = leaderboard[i].second;
        if (prev > 0 && curr / prev >= 0.8) {
            dsu.unionSets(leaderboard[i-1].first, leaderboard[i].first);
        }
    }

    // Name groups by tier
    std::vector<std::vector<std::string>> groups = dsu.getAllGroups();
    int tier = 1;
    for (auto& group : groups) {
        if (!group.empty()) {
            std::string tierName;
            if (tier == 1) tierName = "Diamond";
            else if (tier == 2) tierName = "Gold";
            else if (tier == 3) tierName = "Silver";
            else tierName = "Bronze";
            dsu.setGroupName(group[0], tierName + " Tier");
            tier++;
        }
    }

    // Immediately persist cluster changes
    saveLeaderboard();
}

// ══════════════════════════════════════════════
// PLAYER INFO
// ══════════════════════════════════════════════

PlayerRecord Leaderboard::getPlayerRecord(const std::string& username) const {
    auto it = players.find(username);
    if (it != players.end()) return it->second;
    return PlayerRecord();
}

bool Leaderboard::playerExists(const std::string& username) const {
    return players.find(username) != players.end();
}

double Leaderboard::getPlayerScore(const std::string& username) const {
    auto it = players.find(username);
    if (it != players.end()) return it->second.computedScore;
    return 0.0;
}

std::string Leaderboard::getGameName(GameType type) const {
    if (type >= 0 && type < GAME_COUNT) return gameConfigs[type].name;
    return "Unknown";
}

double Leaderboard::getGameWeight(GameType type) const {
    if (type >= 0 && type < GAME_COUNT) return gameConfigs[type].difficultyWeight;
    return 1.0;
}

// ══════════════════════════════════════════════
// FILE PERSISTENCE
// Format: username|computedScore|gamesPlayed|consistency|recentScores...|timestamps...
// ══════════════════════════════════════════════

void Leaderboard::saveLeaderboard() {
    std::ofstream file(leaderboardFilePath, std::ios::trunc);
    if (!file.is_open()) return;

    for (auto& pair : players) {
        const PlayerRecord& p = pair.second;
        file << p.username << "|"
             << std::fixed << std::setprecision(4) << p.computedScore << "|"
             << p.gamesPlayed << "|"
             << p.consistency;

        // Save recent scores
        file << "|";
        for (size_t i = 0; i < p.recentScores.size(); i++) {
            if (i > 0) file << ",";
            file << p.recentScores[i];
        }

        // Save timestamps
        file << "|";
        for (size_t i = 0; i < p.scoreTimestamps.size(); i++) {
            if (i > 0) file << ",";
            file << p.scoreTimestamps[i];
        }

        // Save game play counts
        file << "|";
        bool first = true;
        for (auto& gc : p.gamePlayCount) {
            if (!first) file << ",";
            file << gc.first << ":" << gc.second;
            first = false;
        }

        // Save game total scores
        file << "|";
        first = true;
        for (auto& gs : p.gameTotalScore) {
            if (!first) file << ",";
            file << gs.first << ":" << gs.second;
            first = false;
        }

        file << "\n";
    }
    file.close();
}

void Leaderboard::loadLeaderboard() {
    std::ifstream file(leaderboardFilePath);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(iss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() < 4) continue;

        PlayerRecord player(tokens[0]);
        player.computedScore = std::stod(tokens[1]);
        player.gamesPlayed = std::stoi(tokens[2]);
        player.consistency = std::stod(tokens[3]);

        // Parse recent scores
        if (tokens.size() > 4 && !tokens[4].empty()) {
            std::istringstream scoreStream(tokens[4]);
            std::string scoreStr;
            while (std::getline(scoreStream, scoreStr, ',')) {
                if (!scoreStr.empty())
                    player.recentScores.push_back(std::stod(scoreStr));
            }
        }

        // Parse timestamps
        if (tokens.size() > 5 && !tokens[5].empty()) {
            std::istringstream tsStream(tokens[5]);
            std::string tsStr;
            while (std::getline(tsStream, tsStr, ',')) {
                if (!tsStr.empty())
                    player.scoreTimestamps.push_back(std::stoll(tsStr));
            }
        }

        // Parse game play counts
        if (tokens.size() > 6 && !tokens[6].empty()) {
            std::istringstream gcStream(tokens[6]);
            std::string gcStr;
            while (std::getline(gcStream, gcStr, ',')) {
                size_t colonPos = gcStr.find(':');
                if (colonPos != std::string::npos) {
                    int gameType = std::stoi(gcStr.substr(0, colonPos));
                    int count = std::stoi(gcStr.substr(colonPos + 1));
                    player.gamePlayCount[gameType] = count;
                }
            }
        }

        // Parse game total scores
        if (tokens.size() > 7 && !tokens[7].empty()) {
            std::istringstream gsStream(tokens[7]);
            std::string gsStr;
            while (std::getline(gsStream, gsStr, ',')) {
                size_t colonPos = gsStr.find(':');
                if (colonPos != std::string::npos) {
                    int gameType = std::stoi(gsStr.substr(0, colonPos));
                    double totalScore = std::stod(gsStr.substr(colonPos + 1));
                    player.gameTotalScore[gameType] = totalScore;
                }
            }
        }

        // Register player in all structures
        players[player.username] = player;
        playerTrie.insert(player.username);
        dsu.makeSet(player.username);

        if (player.computedScore > 0) {
            avlTree.insert(player.username, player.computedScore);
            rbTree.insert(player.username, player.computedScore);
            skipList.insert(player.username, player.computedScore);
            treap.insert(player.username, player.computedScore);
            maxHeap.insert(player.username, player.computedScore);
        }
    }

    file.close();
    rebuildSegmentTree();
}
