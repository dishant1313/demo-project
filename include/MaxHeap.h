/*
 * MaxHeap.h - Max Heap for Top-K Player Queries
 *
 * Time Complexity:
 *   Insert:       O(log n)
 *   Extract Max:  O(log n)
 *   Get Max:      O(1)
 *   Build Heap:   O(n)
 *
 * Space Complexity: O(n)
 *
 * Used for: Efficiently retrieving the top K players from the leaderboard.
 */

#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include <string>
#include <vector>

struct HeapEntry {
    std::string username;
    double score;

    HeapEntry();
    HeapEntry(const std::string& uname, double sc);
};

class MaxHeap {
private:
    std::vector<HeapEntry> heap;

    void heapifyUp(int index);
    void heapifyDown(int index);
    int parent(int i);
    int leftChild(int i);
    int rightChild(int i);

public:
    MaxHeap();

    void insert(const std::string& username, double score);
    HeapEntry extractMax();
    HeapEntry peekMax() const;
    std::vector<HeapEntry> getTopK(int k);
    bool isEmpty() const;
    int size() const;
    void clear();
    void buildFromVector(const std::vector<std::pair<std::string, double>>& data);
};

#endif // MAX_HEAP_H
