/*
 * RedBlackTree.h - Red-Black Tree for Alternative Balanced Ranking
 *
 * Time Complexity:
 *   Insert:  O(log n)
 *   Delete:  O(log n)
 *   Search:  O(log n)
 *
 * Space Complexity: O(n)
 *
 * Used for: Alternative balancing mechanism for leaderboard data,
 *           demonstrating different self-balancing BST strategies.
 */

#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <string>
#include <vector>

enum Color { RED, BLACK };

struct RBNode {
    std::string username;
    double score;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;

    RBNode(const std::string& uname, double sc);
};

class RedBlackTree {
private:
    RBNode* root;
    RBNode* NIL; // Sentinel node

    void leftRotate(RBNode* x);
    void rightRotate(RBNode* x);
    void insertFixup(RBNode* z);
    void deleteFixup(RBNode* x);
    void transplant(RBNode* u, RBNode* v);
    RBNode* minimum(RBNode* node);

    void inorderDesc(RBNode* node, std::vector<std::pair<std::string, double>>& result);
    void destroyTree(RBNode* node);
    RBNode* search(RBNode* node, const std::string& username, double score);

public:
    RedBlackTree();
    ~RedBlackTree();

    void insert(const std::string& username, double score);
    void remove(const std::string& username, double score);
    bool search(const std::string& username, double score);
    std::vector<std::pair<std::string, double>> getSortedLeaderboard();
    void clear();
};

#endif // RED_BLACK_TREE_H
