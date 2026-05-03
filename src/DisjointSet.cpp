/*
 * DisjointSet.cpp - Union-Find Implementation
 *
 * Uses path compression + union by rank for near-constant amortized operations.
 * Maps players (strings) to groups, supporting named clusters.
 */

#include "DisjointSet.h"
#include <algorithm>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────
DisjointSet::DisjointSet() {}

// ──────────────────────────────────────────────
// makeSet - O(1)
// Creates a new singleton set for a player
// ──────────────────────────────────────────────
void DisjointSet::makeSet(const std::string& player) {
    if (parent.find(player) == parent.end()) {
        parent[player] = player;
        rank[player] = 0;
        groupName[player] = ""; // No group name initially
    }
}

// ──────────────────────────────────────────────
// find - O(α(n)) amortized with path compression
// ──────────────────────────────────────────────
std::string DisjointSet::find(const std::string& player) {
    if (parent.find(player) == parent.end())
        return "";

    if (parent[player] != player)
        parent[player] = find(parent[player]); // Path compression
    return parent[player];
}

// ──────────────────────────────────────────────
// unionSets - O(α(n)) amortized with union by rank
// ──────────────────────────────────────────────
bool DisjointSet::unionSets(const std::string& player1, const std::string& player2) {
    std::string root1 = find(player1);
    std::string root2 = find(player2);

    if (root1.empty() || root2.empty()) return false;
    if (root1 == root2) return false; // Already in same set

    // Union by rank
    if (rank[root1] < rank[root2]) {
        parent[root1] = root2;
    } else if (rank[root1] > rank[root2]) {
        parent[root2] = root1;
    } else {
        parent[root2] = root1;
        rank[root1]++;
    }

    return true;
}

// ──────────────────────────────────────────────
// isConnected - O(α(n))
// ──────────────────────────────────────────────
bool DisjointSet::isConnected(const std::string& player1, const std::string& player2) {
    std::string root1 = find(player1);
    std::string root2 = find(player2);
    return !root1.empty() && !root2.empty() && root1 == root2;
}

// ──────────────────────────────────────────────
// Group naming
// ──────────────────────────────────────────────
void DisjointSet::setGroupName(const std::string& player, const std::string& name) {
    std::string root = find(player);
    if (!root.empty())
        groupName[root] = name;
}

std::string DisjointSet::getGroupName(const std::string& player) {
    std::string root = find(player);
    if (root.empty()) return "";
    if (groupName.find(root) != groupName.end() && !groupName[root].empty())
        return groupName[root];
    return "Group(" + root + ")";
}

// ──────────────────────────────────────────────
// getAllGroups - O(n * α(n))
// Returns all groups as vectors of members
// ──────────────────────────────────────────────
std::vector<std::vector<std::string>> DisjointSet::getAllGroups() {
    std::unordered_map<std::string, std::vector<std::string>> groups;
    for (auto& p : parent) {
        std::string root = find(p.first);
        groups[root].push_back(p.first);
    }

    std::vector<std::vector<std::string>> result;
    for (auto& g : groups) {
        std::sort(g.second.begin(), g.second.end());
        result.push_back(g.second);
    }
    return result;
}

int DisjointSet::getGroupCount() {
    std::unordered_map<std::string, bool> roots;
    for (auto& p : parent) {
        roots[find(p.first)] = true;
    }
    return (int)roots.size();
}

std::vector<std::string> DisjointSet::getGroupMembers(const std::string& player) {
    std::string root = find(player);
    std::vector<std::string> members;
    if (root.empty()) return members;

    for (auto& p : parent) {
        if (find(p.first) == root)
            members.push_back(p.first);
    }
    std::sort(members.begin(), members.end());
    return members;
}

bool DisjointSet::exists(const std::string& player) {
    return parent.find(player) != parent.end();
}
