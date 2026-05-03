/*
 * Trie.cpp - Trie Implementation
 *
 * Prefix tree supporting insert, exact search, prefix search,
 * autocomplete, and deletion. Uses fixed 128-char alphabet for ASCII.
 */

#include "Trie.h"
#include <cstring>

// ──────────────────────────────────────────────
// TrieNode Constructor
// ──────────────────────────────────────────────
TrieNode::TrieNode() : isEndOfWord(false), fullUsername("") {
    for (int i = 0; i < ALPHABET_SIZE; i++)
        children[i] = nullptr;
}

// ──────────────────────────────────────────────
// Trie Constructor / Destructor
// ──────────────────────────────────────────────
Trie::Trie() {
    root = new TrieNode();
}

Trie::~Trie() {
    destroyTrie(root);
}

// ──────────────────────────────────────────────
// Insert - O(m) where m = word length
// ──────────────────────────────────────────────
void Trie::insert(const std::string& word) {
    TrieNode* current = root;
    for (char c : word) {
        int index = (int)c;
        if (index < 0 || index >= ALPHABET_SIZE) continue;
        if (!current->children[index])
            current->children[index] = new TrieNode();
        current = current->children[index];
    }
    current->isEndOfWord = true;
    current->fullUsername = word;
}

// ──────────────────────────────────────────────
// Search - O(m) exact match
// ──────────────────────────────────────────────
bool Trie::search(const std::string& word) {
    TrieNode* current = root;
    for (char c : word) {
        int index = (int)c;
        if (index < 0 || index >= ALPHABET_SIZE) return false;
        if (!current->children[index])
            return false;
        current = current->children[index];
    }
    return current->isEndOfWord;
}

// ──────────────────────────────────────────────
// StartsWith - O(m) prefix check
// ──────────────────────────────────────────────
bool Trie::startsWith(const std::string& prefix) {
    TrieNode* current = root;
    for (char c : prefix) {
        int index = (int)c;
        if (index < 0 || index >= ALPHABET_SIZE) return false;
        if (!current->children[index])
            return false;
        current = current->children[index];
    }
    return true;
}

// ──────────────────────────────────────────────
// Autocomplete - O(m + k) 
// Returns all words with given prefix
// ──────────────────────────────────────────────
void Trie::collectAll(TrieNode* node, std::vector<std::string>& results) {
    if (!node) return;
    if (node->isEndOfWord)
        results.push_back(node->fullUsername);
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i])
            collectAll(node->children[i], results);
    }
}

std::vector<std::string> Trie::autocomplete(const std::string& prefix) {
    std::vector<std::string> results;
    TrieNode* current = root;
    for (char c : prefix) {
        int index = (int)c;
        if (index < 0 || index >= ALPHABET_SIZE) return results;
        if (!current->children[index])
            return results;
        current = current->children[index];
    }
    collectAll(current, results);
    return results;
}

// ──────────────────────────────────────────────
// Remove - O(m)
// ──────────────────────────────────────────────
bool Trie::hasChildren(TrieNode* node) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) return true;
    }
    return false;
}

bool Trie::removeHelper(TrieNode* node, const std::string& word, int depth) {
    if (!node) return false;

    if (depth == (int)word.size()) {
        if (!node->isEndOfWord) return false;
        node->isEndOfWord = false;
        node->fullUsername = "";
        return !hasChildren(node);
    }

    int index = (int)word[depth];
    if (index < 0 || index >= ALPHABET_SIZE) return false;

    if (removeHelper(node->children[index], word, depth + 1)) {
        delete node->children[index];
        node->children[index] = nullptr;
        return !node->isEndOfWord && !hasChildren(node);
    }

    return false;
}

bool Trie::remove(const std::string& word) {
    if (!search(word)) return false;
    removeHelper(root, word, 0);
    return true;
}

void Trie::clear() {
    destroyTrie(root);
    root = new TrieNode();
}

void Trie::destroyTrie(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        destroyTrie(node->children[i]);
    }
    delete node;
}
