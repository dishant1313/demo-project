/*
 * SkipList.cpp - Skip List Implementation
 *
 * Multi-level linked list with probabilistic balancing.
 * Each insertion randomly determines the level of the new node.
 * Sorted in descending score order for leaderboard use.
 */

#include "SkipList.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

// ──────────────────────────────────────────────
// SkipListNode Constructor
// ──────────────────────────────────────────────
SkipListNode::SkipListNode(const std::string& uname, double sc, int level)
    : username(uname), score(sc), forward(level + 1, nullptr) {}

// ──────────────────────────────────────────────
// SkipList Constructor / Destructor
// ──────────────────────────────────────────────
SkipList::SkipList() : currentLevel(0), nodeCount(0) {
    header = new SkipListNode("", 1e18, SKIP_LIST_MAX_LEVEL);
    srand((unsigned int)time(nullptr));
}

SkipList::~SkipList() {
    clear();
    delete header;
}

// ──────────────────────────────────────────────
// Random Level Generation
// Probability 0.5 for each additional level
// ──────────────────────────────────────────────
int SkipList::randomLevel() {
    int level = 0;
    while (rand() % 2 == 0 && level < SKIP_LIST_MAX_LEVEL)
        level++;
    return level;
}

// ──────────────────────────────────────────────
// Insert - Expected O(log n)
// Descending order: higher score comes first
// ──────────────────────────────────────────────
void SkipList::insert(const std::string& username, double score) {
    std::vector<SkipListNode*> update(SKIP_LIST_MAX_LEVEL + 1, nullptr);
    SkipListNode* current = header;

    // Traverse from top level down (descending order)
    for (int i = currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr &&
               (current->forward[i]->score > score ||
                (current->forward[i]->score == score && current->forward[i]->username < username))) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    // Check for duplicate
    if (current != nullptr && current->score == score && current->username == username) {
        return; // Already exists
    }

    int newLevel = randomLevel();
    if (newLevel > currentLevel) {
        for (int i = currentLevel + 1; i <= newLevel; i++) {
            update[i] = header;
        }
        currentLevel = newLevel;
    }

    SkipListNode* newNode = new SkipListNode(username, score, newLevel);
    for (int i = 0; i <= newLevel; i++) {
        newNode->forward[i] = update[i]->forward[i];
        update[i]->forward[i] = newNode;
    }

    nodeCount++;
}

// ──────────────────────────────────────────────
// Remove - Expected O(log n)
// ──────────────────────────────────────────────
bool SkipList::remove(const std::string& username, double score) {
    std::vector<SkipListNode*> update(SKIP_LIST_MAX_LEVEL + 1, nullptr);
    SkipListNode* current = header;

    for (int i = currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr &&
               (current->forward[i]->score > score ||
                (current->forward[i]->score == score && current->forward[i]->username < username))) {
            current = current->forward[i];
        }
        update[i] = current;
    }

    current = current->forward[0];

    if (current != nullptr && current->score == score && current->username == username) {
        for (int i = 0; i <= currentLevel; i++) {
            if (update[i]->forward[i] != current)
                break;
            update[i]->forward[i] = current->forward[i];
        }

        delete current;
        nodeCount--;

        while (currentLevel > 0 && header->forward[currentLevel] == nullptr)
            currentLevel--;

        return true;
    }

    return false;
}

// ──────────────────────────────────────────────
// Search - Expected O(log n)
// ──────────────────────────────────────────────
bool SkipList::search(const std::string& username, double score) {
    SkipListNode* current = header;
    for (int i = currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr &&
               (current->forward[i]->score > score ||
                (current->forward[i]->score == score && current->forward[i]->username < username))) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    return (current != nullptr && current->score == score && current->username == username);
}

// ──────────────────────────────────────────────
// Get sorted leaderboard - O(n)
// Already stored in descending order
// ──────────────────────────────────────────────
std::vector<std::pair<std::string, double>> SkipList::getSortedLeaderboard() {
    std::vector<std::pair<std::string, double>> result;
    SkipListNode* current = header->forward[0];
    while (current != nullptr) {
        result.push_back({current->username, current->score});
        current = current->forward[0];
    }
    return result;
}

std::vector<std::pair<std::string, double>> SkipList::getTopK(int k) {
    std::vector<std::pair<std::string, double>> result;
    SkipListNode* current = header->forward[0];
    int count = 0;
    while (current != nullptr && count < k) {
        result.push_back({current->username, current->score});
        current = current->forward[0];
        count++;
    }
    return result;
}

int SkipList::size() const {
    return nodeCount;
}

void SkipList::clear() {
    SkipListNode* current = header->forward[0];
    while (current != nullptr) {
        SkipListNode* next = current->forward[0];
        delete current;
        current = next;
    }
    for (int i = 0; i <= SKIP_LIST_MAX_LEVEL; i++) {
        header->forward[i] = nullptr;
    }
    currentLevel = 0;
    nodeCount = 0;
}
