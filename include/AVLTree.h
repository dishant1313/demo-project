/*
 * AVLTree.h - Self-balancing AVL Tree for Leaderboard Ranking
 *
 * Time Complexity:
 *   Insert:  O(log n)
 *   Delete:  O(log n)
 *   Search:  O(log n)
 *   Rank:    O(log n) with order statistics
 *
 * Space Complexity: O(n)
 *
 * Used for: Maintaining a dynamically sorted leaderboard with O(log n) rank queries.
 */

#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <string>
#include <vector>
#include <functional>

struct AVLNode {
    std::string username;
    double score;
    int height;
    int subtreeSize; // For order-statistics (rank queries)
    AVLNode* left;
    AVLNode* right;

    AVLNode(const std::string& uname, double sc);
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* node);
    int getSize(AVLNode* node);
    int getBalance(AVLNode* node);
    void updateHeight(AVLNode* node);
    void updateSize(AVLNode* node);

    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);

    AVLNode* insert(AVLNode* node, const std::string& username, double score);
    AVLNode* remove(AVLNode* node, const std::string& username, double score);
    AVLNode* minValueNode(AVLNode* node);

    void inorderDesc(AVLNode* node, std::vector<std::pair<std::string, double>>& result);
    void destroyTree(AVLNode* node);
    int getRankHelper(AVLNode* node, const std::string& username, double score);
    void getTopK(AVLNode* node, int k, std::vector<std::pair<std::string, double>>& result);

public:
    AVLTree();
    ~AVLTree();

    void insert(const std::string& username, double score);
    void remove(const std::string& username, double score);
    int getRank(const std::string& username, double score);
    std::vector<std::pair<std::string, double>> getSortedLeaderboard();
    std::vector<std::pair<std::string, double>> getTopK(int k);
    bool isEmpty() const;
    void clear();
};

#endif // AVL_TREE_H
