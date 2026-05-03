/*
 * Treap.cpp - Treap Implementation
 *
 * Randomized BST that maintains BST property by score and
 * max-heap property by randomly assigned priorities.
 * Expected O(log n) for all operations due to random structure.
 */

#include "Treap.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

// ──────────────────────────────────────────────
// TreapNode Constructor
// ──────────────────────────────────────────────
TreapNode::TreapNode(const std::string& uname, double sc, int prio)
    : username(uname), score(sc), priority(prio), subtreeSize(1),
      left(nullptr), right(nullptr) {}

// ──────────────────────────────────────────────
// Treap Constructor / Destructor
// ──────────────────────────────────────────────
Treap::Treap() : root(nullptr) {
    srand((unsigned int)time(nullptr));
}

Treap::~Treap() {
    destroyTree(root);
}

int Treap::generatePriority() {
    return rand();
}

int Treap::getSize(TreapNode* node) {
    return node ? node->subtreeSize : 0;
}

void Treap::updateSize(TreapNode* node) {
    if (node)
        node->subtreeSize = 1 + getSize(node->left) + getSize(node->right);
}

// ──────────────────────────────────────────────
// Rotations - O(1)
// ──────────────────────────────────────────────
TreapNode* Treap::rotateRight(TreapNode* node) {
    TreapNode* leftChild = node->left;
    node->left = leftChild->right;
    leftChild->right = node;
    updateSize(node);
    updateSize(leftChild);
    return leftChild;
}

TreapNode* Treap::rotateLeft(TreapNode* node) {
    TreapNode* rightChild = node->right;
    node->right = rightChild->left;
    rightChild->left = node;
    updateSize(node);
    updateSize(rightChild);
    return rightChild;
}

// ──────────────────────────────────────────────
// Insert - Expected O(log n)
// BST insert then rotate up to maintain heap property
// ──────────────────────────────────────────────
TreapNode* Treap::insert(TreapNode* node, const std::string& username, double score) {
    if (!node)
        return new TreapNode(username, score, generatePriority());

    if (score > node->score || (score == node->score && username < node->username)) {
        node->left = insert(node->left, username, score);
        if (node->left->priority > node->priority)
            node = rotateRight(node);
    } else if (score < node->score || (score == node->score && username > node->username)) {
        node->right = insert(node->right, username, score);
        if (node->right->priority > node->priority)
            node = rotateLeft(node);
    }
    // Duplicate: do nothing

    updateSize(node);
    return node;
}

void Treap::insert(const std::string& username, double score) {
    root = insert(root, username, score);
}

// ──────────────────────────────────────────────
// Remove - Expected O(log n)
// Rotate node down until it becomes a leaf, then delete
// ──────────────────────────────────────────────
TreapNode* Treap::remove(TreapNode* node, const std::string& username, double score) {
    if (!node) return nullptr;

    if (score > node->score || (score == node->score && username < node->username)) {
        node->left = remove(node->left, username, score);
    } else if (score < node->score || (score == node->score && username > node->username)) {
        node->right = remove(node->right, username, score);
    } else {
        // Found the node
        if (!node->left && !node->right) {
            delete node;
            return nullptr;
        } else if (!node->left) {
            node = rotateLeft(node);
            node->left = remove(node->left, username, score);
        } else if (!node->right) {
            node = rotateRight(node);
            node->right = remove(node->right, username, score);
        } else {
            if (node->left->priority > node->right->priority) {
                node = rotateRight(node);
                node->right = remove(node->right, username, score);
            } else {
                node = rotateLeft(node);
                node->left = remove(node->left, username, score);
            }
        }
    }

    if (node) updateSize(node);
    return node;
}

void Treap::remove(const std::string& username, double score) {
    root = remove(root, username, score);
}

// ──────────────────────────────────────────────
// Search - Expected O(log n)
// ──────────────────────────────────────────────
TreapNode* Treap::search(TreapNode* node, const std::string& username, double score) {
    if (!node) return nullptr;

    if (score > node->score || (score == node->score && username < node->username))
        return search(node->left, username, score);
    else if (score < node->score || (score == node->score && username > node->username))
        return search(node->right, username, score);
    else
        return node;
}

bool Treap::search(const std::string& username, double score) {
    return search(root, username, score) != nullptr;
}

// ──────────────────────────────────────────────
// In-order Descending - O(n)
// ──────────────────────────────────────────────
void Treap::inorderDesc(TreapNode* node, std::vector<std::pair<std::string, double>>& result) {
    if (!node) return;
    inorderDesc(node->left, result);
    result.push_back({node->username, node->score});
    inorderDesc(node->right, result);
}

std::vector<std::pair<std::string, double>> Treap::getSortedLeaderboard() {
    std::vector<std::pair<std::string, double>> result;
    inorderDesc(root, result);
    return result;
}

// ──────────────────────────────────────────────
// getRank - Expected O(log n) using subtree sizes
// ──────────────────────────────────────────────
int Treap::getRank(const std::string& username, double score) {
    TreapNode* node = root;
    int rank = 0;
    while (node) {
        if (score > node->score || (score == node->score && username < node->username)) {
            node = node->left;
        } else if (score < node->score || (score == node->score && username > node->username)) {
            rank += getSize(node->left) + 1;
            node = node->right;
        } else {
            return rank + getSize(node->left) + 1;
        }
    }
    return -1; // Not found
}

void Treap::clear() {
    destroyTree(root);
    root = nullptr;
}

void Treap::destroyTree(TreapNode* node) {
    if (!node) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}
