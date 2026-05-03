/*
 * Trie.h - Trie (Prefix Tree) for Username Search/Autocomplete
 *
 * Time Complexity:
 *   Insert:     O(m) where m = length of string
 *   Search:     O(m)
 *   Prefix:     O(m + k) where k = number of matches
 *   Delete:     O(m)
 *
 * Space Complexity: O(ALPHABET_SIZE * m * n) for n strings of avg length m
 *
 * Used for: Fast prefix-based username search and autocomplete.
 */

#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>

const int ALPHABET_SIZE = 128; // ASCII characters

struct TrieNode {
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;
    std::string fullUsername; // Store full username at leaf

    TrieNode();
};

class Trie {
private:
    TrieNode* root;

    void collectAll(TrieNode* node, std::vector<std::string>& results);
    void destroyTrie(TrieNode* node);
    bool removeHelper(TrieNode* node, const std::string& word, int depth);
    bool hasChildren(TrieNode* node);

public:
    Trie();
    ~Trie();

    void insert(const std::string& word);
    bool search(const std::string& word);
    bool startsWith(const std::string& prefix);
    std::vector<std::string> autocomplete(const std::string& prefix);
    bool remove(const std::string& word);
    void clear();
};

#endif // TRIE_H
