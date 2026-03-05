#  Fenwick Tree (Binary Indexed Tree)

The Fenwick Tree is a data structure that provides an elegant "middle ground" between a simple array and a complex Segment Tree. It allows us to perform both **point updates** and **prefix sum queries** in logarithmic time.

---

## 1. The Core Motivation

In competitive programming, we often face a trade-off:

| Data Structure | Query `sum(1..i)` | Update `A[i]` | Space |
| --- | --- | --- | --- |
| **Simple Array** | $O(N)$ | $O(1)$ | $O(N)$ |
| **Prefix Sum Array** | $O(1)$ | $O(N)$ | $O(N)$ |
| **Fenwick Tree** | **$O(\log N)$** | **$O(\log N)$** | **$O(N)$** |

**The Goal:** We want a structure where updating one element doesn't force us to update the entire prefix array.

---

## 2. The Mental Model: "Responsibility"

Instead of every index $i$ storing the sum from $1 \dots i$, each index in a Fenwick Tree is **responsible** for a specific "chunk" of the array.

### How big is the chunk?

The size of the range managed by index `i` is determined by the **Value of its Lowest Set Bit (LSB)**.

> **Rule:** Index `i` stores the sum of the range:
> `(i - LSB(i) + 1) to i`

### Examples (N=8):

* **Index 3 ($011_2$):** LSB is $1$. It stores $1$ element: `[3, 3]`.
* **Index 6 ($110_2$):** LSB is $2$. It stores $2$ elements: `[5, 6]`.
* **Index 4 ($100_2$):** LSB is $4$. It stores $4$ elements: `[1, 4]`.
* **Index 8 ($1000_2$):** LSB is $8$. It stores $8$ elements: `[1, 8]`.

---

## 3. Operations: The Bit-Magic

### A. The Prefix Query (Going Backward)

To find the sum from $1 \dots 7$:

1. Look at `tree[7]` (Covers index 7).
2. Jump backward by removing the LSB: $7 - 1 = 6$.
3. Look at `tree[6]` (Covers indices 5–6).
4. Jump backward: $6 - 2 = 4$.
5. Look at `tree[4]` (Covers indices 1–4).
6. Stop at 0.

**Total sum = `tree[7] + tree[6] + tree[4]**`

### B. The Update (Going Forward)

If we update index $3$, we must update every "boss" that covers it:

1. Update `tree[3]`.
2. Jump forward by adding the LSB: $3 + 1 = 4$.
3. Update `tree[4]`.
4. Jump forward: $4 + 4 = 8$.
5. Update `tree[8]`.

---

## 4. Implementation (C++17)

```cpp
#include <vector>

struct FenwickTree {
    int n;
    std::vector<long long> bit;

    FenwickTree(int n) : n(n), bit(n + 1, 0) {}

    // Add 'val' to the element at index 'i' (1-based)
    void update(int i, long long val) {
        for (; i <= n; i += i & -i) {
            bit[i] += val;
        }
    }

    // Returns prefix sum from 1 to i
    long long query(int i) {
        long long sum = 0;
        for (; i > 0; i -= i & -i) {
            sum += bit[i];
        }
        return sum;
    }

    // Returns range sum from [l, r]
    long long rangeQuery(int l, int r) {
        if (l > r) return 0;
        return query(r) - query(l - 1);
    }
};

```

---

## 5. Use Cases & Patterns

* **Dynamic Prefix Sums:** Standard point update, range query.
* **Frequency Tables:** `update(val, 1)` to count occurrences. `query(val)` tells you how many numbers are $\le$ val.
* **Inversion Count:** Process the array and use the tree to count how many numbers larger than the current one have already been seen.
* **Coordinate Compression:** If the values in the array are very large (e.g., $10^9$), map them to their ranks ($1 \dots N$) before using them as indices in the Fenwick Tree.

---

## 6. Pro-Tip: The `i & -i` Trick

In computer architecture, `-i` is represented as the **Two's Complement** of `i`.
Performing `i & -i` is a bitwise shortcut that isolates the rightmost set bit.

* Example: $6$ is `...0110`
* `-6` is `...1010`
* `6 & -6` is `...0010` (which is $2$).

---

---

### Would you like me to generate a specific manual test case simulation for the Inversion Count problem to add to this README?
