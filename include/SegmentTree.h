/*
 * SegmentTree.h - Segment Tree for Range Score Queries
 *
 * Time Complexity:
 *   Build:       O(n)
 *   Point Update: O(log n)
 *   Range Query:  O(log n) - sum, min, max over a range
 *
 * Space Complexity: O(4n) ≈ O(n)
 *
 * Used for: Range queries on score history (e.g., sum of scores
 *           in a time window, max score in last K games).
 */

#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H

#include <vector>
#include <string>

class SegmentTree {
private:
    std::vector<double> tree;
    std::vector<double> data;
    int n;

    void build(int node, int start, int end);
    void update(int node, int start, int end, int idx, double val);
    double querySum(int node, int start, int end, int l, int r);
    double queryMax(int node, int start, int end, int l, int r);
    double queryMin(int node, int start, int end, int l, int r);

public:
    SegmentTree();
    SegmentTree(int size);
    SegmentTree(const std::vector<double>& arr);

    void build(const std::vector<double>& arr);
    void update(int index, double value);
    double querySum(int left, int right);
    double queryMax(int left, int right);
    double queryMin(int left, int right);
    int getSize() const;
    void resize(int newSize);
};

#endif // SEGMENT_TREE_H
