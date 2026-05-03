/*
 * MaxHeap.cpp - Max Heap Implementation
 *
 * Array-based binary max heap.
 * Used for efficiently extracting top K players.
 */

#include "MaxHeap.h"
#include <stdexcept>
#include <algorithm>

// ──────────────────────────────────────────────
// HeapEntry
// ──────────────────────────────────────────────
HeapEntry::HeapEntry() : username(""), score(0) {}
HeapEntry::HeapEntry(const std::string& uname, double sc) : username(uname), score(sc) {}

// ──────────────────────────────────────────────
// MaxHeap Constructor
// ──────────────────────────────────────────────
MaxHeap::MaxHeap() {}

int MaxHeap::parent(int i) { return (i - 1) / 2; }
int MaxHeap::leftChild(int i) { return 2 * i + 1; }
int MaxHeap::rightChild(int i) { return 2 * i + 2; }

// ──────────────────────────────────────────────
// Heapify Up - O(log n)
// Maintains max-heap property after insertion
// ──────────────────────────────────────────────
void MaxHeap::heapifyUp(int index) {
    while (index > 0 && heap[parent(index)].score < heap[index].score) {
        std::swap(heap[parent(index)], heap[index]);
        index = parent(index);
    }
}

// ──────────────────────────────────────────────
// Heapify Down - O(log n)
// Maintains max-heap property after extraction
// ──────────────────────────────────────────────
void MaxHeap::heapifyDown(int index) {
    int largest = index;
    int l = leftChild(index);
    int r = rightChild(index);

    if (l < (int)heap.size() && heap[l].score > heap[largest].score)
        largest = l;
    if (r < (int)heap.size() && heap[r].score > heap[largest].score)
        largest = r;

    if (largest != index) {
        std::swap(heap[index], heap[largest]);
        heapifyDown(largest);
    }
}

// ──────────────────────────────────────────────
// Insert - O(log n)
// ──────────────────────────────────────────────
void MaxHeap::insert(const std::string& username, double score) {
    heap.push_back(HeapEntry(username, score));
    heapifyUp((int)heap.size() - 1);
}

// ──────────────────────────────────────────────
// Extract Max - O(log n)
// ──────────────────────────────────────────────
HeapEntry MaxHeap::extractMax() {
    if (heap.empty())
        throw std::runtime_error("Heap is empty");

    HeapEntry maxEntry = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    if (!heap.empty())
        heapifyDown(0);

    return maxEntry;
}

// ──────────────────────────────────────────────
// Peek Max - O(1)
// ──────────────────────────────────────────────
HeapEntry MaxHeap::peekMax() const {
    if (heap.empty())
        throw std::runtime_error("Heap is empty");
    return heap[0];
}

// ──────────────────────────────────────────────
// Get Top K - O(k log n)
// Extracts top k, then re-inserts them
// ──────────────────────────────────────────────
std::vector<HeapEntry> MaxHeap::getTopK(int k) {
    std::vector<HeapEntry> topK;
    std::vector<HeapEntry> temp;

    int count = std::min(k, (int)heap.size());
    for (int i = 0; i < count; i++) {
        HeapEntry entry = extractMax();
        topK.push_back(entry);
        temp.push_back(entry);
    }

    // Re-insert extracted elements
    for (auto& entry : temp) {
        insert(entry.username, entry.score);
    }

    return topK;
}

bool MaxHeap::isEmpty() const {
    return heap.empty();
}

int MaxHeap::size() const {
    return (int)heap.size();
}

void MaxHeap::clear() {
    heap.clear();
}

// ──────────────────────────────────────────────
// Build from vector - O(n)
// Uses bottom-up heapify for linear-time construction
// ──────────────────────────────────────────────
void MaxHeap::buildFromVector(const std::vector<std::pair<std::string, double>>& data) {
    heap.clear();
    for (auto& p : data) {
        heap.push_back(HeapEntry(p.first, p.second));
    }
    // Bottom-up heapify
    for (int i = (int)heap.size() / 2 - 1; i >= 0; i--) {
        heapifyDown(i);
    }
}
