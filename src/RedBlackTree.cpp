/*
 * RedBlackTree.cpp - Red-Black Tree Implementation
 *
 * A self-balancing BST where each node has a color (RED/BLACK).
 * Guarantees O(log n) operations through color-based rebalancing rules.
 * Uses a sentinel NIL node to simplify boundary conditions.
 */

#include "RedBlackTree.h"
#include <iostream>

// ──────────────────────────────────────────────
// RBNode Constructor
// ──────────────────────────────────────────────
RBNode::RBNode(const std::string& uname, double sc)
    : username(uname), score(sc), color(RED),
      left(nullptr), right(nullptr), parent(nullptr) {}

// ──────────────────────────────────────────────
// RedBlackTree Constructor / Destructor
// ──────────────────────────────────────────────
RedBlackTree::RedBlackTree() {
    NIL = new RBNode("", 0);
    NIL->color = BLACK;
    NIL->left = NIL->right = NIL->parent = nullptr;
    root = NIL;
}

RedBlackTree::~RedBlackTree() {
    destroyTree(root);
    delete NIL;
}

// ──────────────────────────────────────────────
// Left Rotate - O(1)
// ──────────────────────────────────────────────
void RedBlackTree::leftRotate(RBNode* x) {
    RBNode* y = x->right;
    x->right = y->left;

    if (y->left != NIL)
        y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == nullptr)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    y->left = x;
    x->parent = y;
}

// ──────────────────────────────────────────────
// Right Rotate - O(1)
// ──────────────────────────────────────────────
void RedBlackTree::rightRotate(RBNode* x) {
    RBNode* y = x->left;
    x->left = y->right;

    if (y->right != NIL)
        y->right->parent = x;

    y->parent = x->parent;

    if (x->parent == nullptr)
        root = y;
    else if (x == x->parent->right)
        x->parent->right = y;
    else
        x->parent->left = y;

    y->right = x;
    x->parent = y;
}

// ──────────────────────────────────────────────
// Insert + Fixup - O(log n)
// ──────────────────────────────────────────────
void RedBlackTree::insert(const std::string& username, double score) {
    RBNode* z = new RBNode(username, score);
    z->left = NIL;
    z->right = NIL;

    RBNode* y = nullptr;
    RBNode* x = root;

    while (x != NIL) {
        y = x;
        if (score > x->score || (score == x->score && username < x->username))
            x = x->left;
        else if (score < x->score || (score == x->score && username > x->username))
            x = x->right;
        else {
            // Duplicate - don't insert
            delete z;
            return;
        }
    }

    z->parent = y;
    if (y == nullptr)
        root = z;
    else if (score > y->score || (score == y->score && username < y->username))
        y->left = z;
    else
        y->right = z;

    if (z->parent == nullptr) {
        z->color = BLACK;
        return;
    }

    if (z->parent->parent == nullptr)
        return;

    insertFixup(z);
}

void RedBlackTree::insertFixup(RBNode* z) {
    while (z->parent && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            RBNode* uncle = z->parent->parent->right;
            if (uncle->color == RED) {
                // Case 1: Uncle is red
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->right) {
                    // Case 2: z is right child
                    z = z->parent;
                    leftRotate(z);
                }
                // Case 3: z is left child
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(z->parent->parent);
            }
        } else {
            // Mirror cases
            RBNode* uncle = z->parent->parent->left;
            if (uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(z->parent->parent);
            }
        }
        if (z == root) break;
    }
    root->color = BLACK;
}

// ──────────────────────────────────────────────
// Transplant helper
// ──────────────────────────────────────────────
void RedBlackTree::transplant(RBNode* u, RBNode* v) {
    if (u->parent == nullptr)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    v->parent = u->parent;
}

RBNode* RedBlackTree::minimum(RBNode* node) {
    while (node->left != NIL)
        node = node->left;
    return node;
}

// ──────────────────────────────────────────────
// Delete + Fixup - O(log n)
// ──────────────────────────────────────────────
void RedBlackTree::remove(const std::string& username, double score) {
    RBNode* z = search(root, username, score);
    if (z == NIL) return;

    RBNode* y = z;
    RBNode* x;
    Color yOrigColor = y->color;

    if (z->left == NIL) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == NIL) {
        x = z->left;
        transplant(z, z->left);
    } else {
        y = minimum(z->right);
        yOrigColor = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            transplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }

    delete z;

    if (yOrigColor == BLACK)
        deleteFixup(x);
}

void RedBlackTree::deleteFixup(RBNode* x) {
    while (x != root && x->color == BLACK) {
        if (x == x->parent->left) {
            RBNode* w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotate(x->parent);
                x = root;
            }
        } else {
            RBNode* w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

// ──────────────────────────────────────────────
// Search - O(log n)
// ──────────────────────────────────────────────
RBNode* RedBlackTree::search(RBNode* node, const std::string& username, double score) {
    if (node == NIL) return NIL;

    if (score > node->score || (score == node->score && username < node->username))
        return search(node->left, username, score);
    else if (score < node->score || (score == node->score && username > node->username))
        return search(node->right, username, score);
    else
        return node;
}

bool RedBlackTree::search(const std::string& username, double score) {
    return search(root, username, score) != NIL;
}

// ──────────────────────────────────────────────
// In-order traversal (descending) - O(n)
// ──────────────────────────────────────────────
void RedBlackTree::inorderDesc(RBNode* node, std::vector<std::pair<std::string, double>>& result) {
    if (node == NIL) return;
    inorderDesc(node->left, result);
    result.push_back({node->username, node->score});
    inorderDesc(node->right, result);
}

std::vector<std::pair<std::string, double>> RedBlackTree::getSortedLeaderboard() {
    std::vector<std::pair<std::string, double>> result;
    inorderDesc(root, result);
    return result;
}

void RedBlackTree::clear() {
    destroyTree(root);
    root = NIL;
}

void RedBlackTree::destroyTree(RBNode* node) {
    if (node == NIL || node == nullptr) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}
