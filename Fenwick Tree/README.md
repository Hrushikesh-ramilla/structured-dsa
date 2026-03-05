# Fenwick Tree (Binary Indexed Tree)

The Fenwick Tree (or Binary Indexed Tree) is a data structure designed for efficient
prefix queries and point updates on an array.

It provides a simpler and more memory-efficient alternative to Segment Trees for many
frequency and prefix aggregation problems.

Typical complexity:

| Operation | Time Complexity |
|----------|----------------|
Point Update | O(log N) |
Prefix Query | O(log N) |
Range Query | O(log N) |

Space Complexity: **O(N)**

---

# Core Idea

A Fenwick Tree stores **partial prefix sums** in a way that allows efficient updates.

Each index `i` is responsible for a range determined by the lowest set bit of `i`.
