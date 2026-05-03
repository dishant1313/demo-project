/*
 * Treap.h - Treap (Tree + Heap) Randomized BST
 *
 * Time Complexity (expected):
 *   Insert:  O(log n)
 *   Delete:  O(log n)
 *   Search:  O(log n)
 *
 * Space Complexity: O(n)
 *
 * Used for: Randomized balanced BST as an alternative ranking structure.
 *           Combines BST ordering by score with heap ordering by random priority.
 */

#ifndef TREAP_H
#define TREAP_H

#include <string>
#include <vector>

struct TreapNode {
    std::string username;
    double score;
    int priority;   // Random priority for heap property
    int subtreeSize;
    TreapNode* left;
    TreapNode* right;

    TreapNode(const std::string& uname, double sc, int prio);
};

class Treap {
private:
    TreapNode* root;

    int getSize(TreapNode* node);
    void updateSize(TreapNode* node);

    TreapNode* rotateRight(TreapNode* node);
    TreapNode* rotateLeft(TreapNode* node);

    TreapNode* insert(TreapNode* node, const std::string& username, double score);
    TreapNode* remove(TreapNode* node, const std::string& username, double score);
    TreapNode* search(TreapNode* node, const std::string& username, double score);

    void inorderDesc(TreapNode* node, std::vector<std::pair<std::string, double>>& result);
    void destroyTree(TreapNode* node);

    int generatePriority();

public:
    Treap();
    ~Treap();

    void insert(const std::string& username, double score);
    void remove(const std::string& username, double score);
    bool search(const std::string& username, double score);
    std::vector<std::pair<std::string, double>> getSortedLeaderboard();
    int getRank(const std::string& username, double score);
    void clear();
};

#endif // TREAP_H
