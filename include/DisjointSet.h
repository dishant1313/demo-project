/*
 * DisjointSet.h - Union-Find (Disjoint Set Union) for Player Clustering
 *
 * Time Complexity:
 *   Find:   O(α(n)) ≈ O(1) amortized (with path compression)
 *   Union:  O(α(n)) ≈ O(1) amortized (with union by rank)
 *
 * Space Complexity: O(n)
 *
 * Used for: Grouping players into clusters/teams/leagues.
 *           Supports finding if two players are in the same group
 *           and merging groups efficiently.
 */

#ifndef DISJOINT_SET_H
#define DISJOINT_SET_H

#include <string>
#include <vector>
#include <unordered_map>

class DisjointSet {
private:
    std::unordered_map<std::string, std::string> parent;
    std::unordered_map<std::string, int> rank;
    std::unordered_map<std::string, std::string> groupName; // Cluster name

public:
    DisjointSet();

    void makeSet(const std::string& player);
    std::string find(const std::string& player);
    bool unionSets(const std::string& player1, const std::string& player2);
    bool isConnected(const std::string& player1, const std::string& player2);
    void setGroupName(const std::string& player, const std::string& name);
    std::string getGroupName(const std::string& player);
    std::vector<std::vector<std::string>> getAllGroups();
    int getGroupCount();
    std::vector<std::string> getGroupMembers(const std::string& player);
    bool exists(const std::string& player);
};

#endif // DISJOINT_SET_H
