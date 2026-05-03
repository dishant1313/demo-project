/*
 * SkipList.h - Skip List for Fast Probabilistic Leaderboard
 *
 * Time Complexity (expected):
 *   Insert:  O(log n)
 *   Delete:  O(log n)
 *   Search:  O(log n)
 *
 * Space Complexity: O(n log n) expected
 *
 * Used for: Fast probabilistic structure for ranked data,
 *           offering simpler implementation than balanced BSTs
 *           with comparable performance guarantees.
 */

#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#include <string>
#include <vector>
#include <climits>

const int SKIP_LIST_MAX_LEVEL = 16;

struct SkipListNode {
    std::string username;
    double score;
    std::vector<SkipListNode*> forward;

    SkipListNode(const std::string& uname, double sc, int level);
};

class SkipList {
private:
    SkipListNode* header;
    int currentLevel;
    int nodeCount;

    int randomLevel();

public:
    SkipList();
    ~SkipList();

    void insert(const std::string& username, double score);
    bool remove(const std::string& username, double score);
    bool search(const std::string& username, double score);
    std::vector<std::pair<std::string, double>> getSortedLeaderboard();
    std::vector<std::pair<std::string, double>> getTopK(int k);
    int size() const;
    void clear();
};

#endif // SKIP_LIST_H
