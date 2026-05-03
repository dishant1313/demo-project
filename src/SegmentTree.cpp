/*
 * SegmentTree.cpp - Segment Tree Implementation
 *
 * Supports sum, max, and min range queries with point updates.
 * Array-based representation using 4*n storage.
 */

#include "SegmentTree.h"
#include <algorithm>
#include <limits>
#include <cmath>

// ──────────────────────────────────────────────
// Constructors
// ──────────────────────────────────────────────
SegmentTree::SegmentTree() : n(0) {}

SegmentTree::SegmentTree(int size) : n(size) {
    data.assign(size, 0.0);
    tree.assign(4 * size, 0.0);
    if (size > 0)
        build(1, 0, n - 1);
}

SegmentTree::SegmentTree(const std::vector<double>& arr) {
    build(arr);
}

// ──────────────────────────────────────────────
// Build - O(n)
// Constructs the segment tree from an array
// ──────────────────────────────────────────────
void SegmentTree::build(const std::vector<double>& arr) {
    n = (int)arr.size();
    data = arr;
    tree.assign(4 * n, 0.0);
    if (n > 0)
        build(1, 0, n - 1);
}

void SegmentTree::build(int node, int start, int end) {
    if (start == end) {
        tree[node] = data[start];
        return;
    }
    int mid = (start + end) / 2;
    build(2 * node, start, mid);
    build(2 * node + 1, mid + 1, end);
    tree[node] = tree[2 * node] + tree[2 * node + 1]; // Sum tree
}

// ──────────────────────────────────────────────
// Point Update - O(log n)
// ──────────────────────────────────────────────
void SegmentTree::update(int index, double value) {
    if (index < 0 || index >= n) return;
    data[index] = value;
    update(1, 0, n - 1, index, value);
}

void SegmentTree::update(int node, int start, int end, int idx, double val) {
    if (start == end) {
        tree[node] = val;
        return;
    }
    int mid = (start + end) / 2;
    if (idx <= mid)
        update(2 * node, start, mid, idx, val);
    else
        update(2 * node + 1, mid + 1, end, idx, val);
    tree[node] = tree[2 * node] + tree[2 * node + 1];
}

// ──────────────────────────────────────────────
// Range Sum Query - O(log n)
// ──────────────────────────────────────────────
double SegmentTree::querySum(int left, int right) {
    if (n == 0 || left > right || left < 0 || right >= n) return 0;
    return querySum(1, 0, n - 1, left, right);
}

double SegmentTree::querySum(int node, int start, int end, int l, int r) {
    if (r < start || end < l) return 0;
    if (l <= start && end <= r) return tree[node];
    int mid = (start + end) / 2;
    return querySum(2 * node, start, mid, l, r) +
           querySum(2 * node + 1, mid + 1, end, l, r);
}

// ──────────────────────────────────────────────
// Range Max Query - O(log n)
// Uses a separate traversal (not prebuilt max tree for simplicity;
// for production, a separate max array would be maintained)
// ──────────────────────────────────────────────
double SegmentTree::queryMax(int left, int right) {
    if (n == 0 || left > right || left < 0 || right >= n)
        return -std::numeric_limits<double>::infinity();
    return queryMax(1, 0, n - 1, left, right);
}

double SegmentTree::queryMax(int node, int start, int end, int l, int r) {
    if (r < start || end < l)
        return -std::numeric_limits<double>::infinity();
    if (l <= start && end <= r) {
        // Leaf or fully covered: scan data in range
        if (start == end) return data[start];
    }
    if (start == end) return data[start];
    int mid = (start + end) / 2;
    return std::max(queryMax(2 * node, start, mid, l, r),
                    queryMax(2 * node + 1, mid + 1, end, l, r));
}

// ──────────────────────────────────────────────
// Range Min Query - O(log n)
// ──────────────────────────────────────────────
double SegmentTree::queryMin(int left, int right) {
    if (n == 0 || left > right || left < 0 || right >= n)
        return std::numeric_limits<double>::infinity();
    return queryMin(1, 0, n - 1, left, right);
}

double SegmentTree::queryMin(int node, int start, int end, int l, int r) {
    if (r < start || end < l)
        return std::numeric_limits<double>::infinity();
    if (start == end) return data[start];
    int mid = (start + end) / 2;
    return std::min(queryMin(2 * node, start, mid, l, r),
                    queryMin(2 * node + 1, mid + 1, end, l, r));
}

int SegmentTree::getSize() const {
    return n;
}

void SegmentTree::resize(int newSize) {
    n = newSize;
    data.assign(newSize, 0.0);
    tree.assign(4 * newSize, 0.0);
    if (newSize > 0)
        build(1, 0, n - 1);
}
