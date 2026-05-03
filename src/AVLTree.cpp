/*
 * AVLTree.cpp - AVL Tree Implementation
 *
 * Self-balancing BST used for maintaining sorted leaderboard.
 * Supports order-statistics for O(log n) rank queries.
 * Uses descending order (higher score = better rank).
 */

#include "AVLTree.h"
#include <algorithm>
#include <iostream>

// ──────────────────────────────────────────────
// AVLNode Constructor
// ──────────────────────────────────────────────
AVLNode::AVLNode(const std::string& uname, double sc)
    : username(uname), score(sc), height(1), subtreeSize(1),
      left(nullptr), right(nullptr) {}

// ──────────────────────────────────────────────
// AVLTree Public Interface
// ──────────────────────────────────────────────
AVLTree::AVLTree() : root(nullptr) {}

AVLTree::~AVLTree() {
    destroyTree(root);
}

// ──────────────────────────────────────────────
// Helper functions
// ──────────────────────────────────────────────
int AVLTree::height(AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::getSize(AVLNode* node) {
    return node ? node->subtreeSize : 0;
}

int AVLTree::getBalance(AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

void AVLTree::updateHeight(AVLNode* node) {
    if (node)
        node->height = 1 + std::max(height(node->left), height(node->right));
}

void AVLTree::updateSize(AVLNode* node) {
    if (node)
        node->subtreeSize = 1 + getSize(node->left) + getSize(node->right);
}

// ──────────────────────────────────────────────
// Rotations - O(1) each
// ──────────────────────────────────────────────
AVLNode* AVLTree::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    updateHeight(y);
    updateSize(y);
    updateHeight(x);
    updateSize(x);

    return x;
}

AVLNode* AVLTree::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateSize(x);
    updateHeight(y);
    updateSize(y);

    return y;
}

// ──────────────────────────────────────────────
// Insert - O(log n)
// Sorted by score descending; ties broken by username ascending
// ──────────────────────────────────────────────
AVLNode* AVLTree::insert(AVLNode* node, const std::string& username, double score) {
    if (!node)
        return new AVLNode(username, score);

    // Descending order: higher scores go left (for rank calculation)
    if (score > node->score || (score == node->score && username < node->username))
        node->left = insert(node->left, username, score);
    else if (score < node->score || (score == node->score && username > node->username))
        node->right = insert(node->right, username, score);
    else
        return node; // Duplicate entry

    updateHeight(node);
    updateSize(node);

    int balance = getBalance(node);

    // Left-Left
    if (balance > 1 && (score > node->left->score ||
        (score == node->left->score && username < node->left->username)))
        return rightRotate(node);

    // Right-Right
    if (balance < -1 && (score < node->right->score ||
        (score == node->right->score && username > node->right->username)))
        return leftRotate(node);

    // Left-Right
    if (balance > 1 && (score < node->left->score ||
        (score == node->left->score && username > node->left->username))) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right-Left
    if (balance < -1 && (score > node->right->score ||
        (score == node->right->score && username < node->right->username))) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void AVLTree::insert(const std::string& username, double score) {
    root = insert(root, username, score);
}

// ──────────────────────────────────────────────
// Remove - O(log n)
// ──────────────────────────────────────────────
AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current->left)
        current = current->left;
    return current;
}

AVLNode* AVLTree::remove(AVLNode* node, const std::string& username, double score) {
    if (!node)
        return nullptr;

    if (score > node->score || (score == node->score && username < node->username))
        node->left = remove(node->left, username, score);
    else if (score < node->score || (score == node->score && username > node->username))
        node->right = remove(node->right, username, score);
    else {
        // Found the node to delete
        if (!node->left || !node->right) {
            AVLNode* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = nullptr;
            } else {
                *node = *temp;
            }
            delete temp;
        } else {
            // In-order successor (smallest in right subtree)
            AVLNode* temp = minValueNode(node->right);
            node->username = temp->username;
            node->score = temp->score;
            node->right = remove(node->right, temp->username, temp->score);
        }
    }

    if (!node) return nullptr;

    updateHeight(node);
    updateSize(node);

    int balance = getBalance(node);

    if (balance > 1 && getBalance(node->left) >= 0)
        return rightRotate(node);
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0)
        return leftRotate(node);
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void AVLTree::remove(const std::string& username, double score) {
    root = remove(root, username, score);
}

// ──────────────────────────────────────────────
// getRank - O(log n) using order statistics
// Returns 1-based rank (1 = highest score)
// ──────────────────────────────────────────────
int AVLTree::getRankHelper(AVLNode* node, const std::string& username, double score) {
    if (!node) return -1;

    if (score > node->score || (score == node->score && username < node->username)) {
        return getRankHelper(node->left, username, score);
    } else if (score < node->score || (score == node->score && username > node->username)) {
        int rightRank = getRankHelper(node->right, username, score);
        if (rightRank == -1) return -1;
        return getSize(node->left) + 1 + rightRank;
    } else {
        return getSize(node->left) + 1;
    }
}

int AVLTree::getRank(const std::string& username, double score) {
    return getRankHelper(root, username, score);
}

// ──────────────────────────────────────────────
// In-order traversal (descending) - O(n)
// ──────────────────────────────────────────────
void AVLTree::inorderDesc(AVLNode* node, std::vector<std::pair<std::string, double>>& result) {
    if (!node) return;
    inorderDesc(node->left, result);
    result.push_back({node->username, node->score});
    inorderDesc(node->right, result);
}

std::vector<std::pair<std::string, double>> AVLTree::getSortedLeaderboard() {
    std::vector<std::pair<std::string, double>> result;
    inorderDesc(root, result);
    return result;
}

// ──────────────────────────────────────────────
// Top K players - O(k log n) using in-order
// ──────────────────────────────────────────────
void AVLTree::getTopK(AVLNode* node, int k, std::vector<std::pair<std::string, double>>& result) {
    if (!node || (int)result.size() >= k) return;
    getTopK(node->left, k, result);
    if ((int)result.size() < k)
        result.push_back({node->username, node->score});
    getTopK(node->right, k, result);
}

std::vector<std::pair<std::string, double>> AVLTree::getTopK(int k) {
    std::vector<std::pair<std::string, double>> result;
    getTopK(root, k, result);
    return result;
}

bool AVLTree::isEmpty() const {
    return root == nullptr;
}

void AVLTree::clear() {
    destroyTree(root);
    root = nullptr;
}

void AVLTree::destroyTree(AVLNode* node) {
    if (!node) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}
