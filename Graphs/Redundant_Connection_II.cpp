/**
 * ==========================================================================================
 * Problem: 685. Redundant Connection II
 * Difficulty: Hard
 * Tags: Graph, Union-Find (DSU), Directed Graph, Tree Properties
 * Source: LeetCode
 * Link: https://leetcode.com/problems/redundant-connection-ii/
 * ==========================================================================================
 *
 * PROBLEM SUMMARY:
 * You are given a directed graph that started as a valid rooted tree, but ONE extra 
 * directed edge was added. Find and return that extra edge. If there are multiple 
 * valid answers, return the one that appears last in the input.
 *
 * A valid rooted tree has:
 * 1. Exactly one root node with in-degree 0.
 * 2. Every other node has exactly one parent (in-degree 1).
 * 3. Absolutely no cycles.
 *
 * ==========================================================================================
 * CONCEPTUAL CLARITY: WHY DFS OR DSU ALONE IS NOT ENOUGH
 * ==========================================================================================
 * * ❌ Why DFS alone isn't sufficient:
 * DFS is great for finding cycles. However, the violation here isn't *just* a cycle. 
 * The extra edge might simply give a node two parents without creating a cycle (a cross-edge). 
 * Trying to write a DFS that simultaneously tracks in-degrees, identifies back-edges vs. 
 * cross-edges, and decides which specific edge to remove becomes a messy nightmare of edge cases.
 *
 * ❌ Why DSU alone isn't sufficient:
 * DSU is designed for UNDIRECTED graphs. It only tells you "Are these nodes connected?".
 * If node C has two parents, A and B, a standard DSU just groups A, B, and C together. 
 * It completely ignores the directed property (in-degree = 2), which is a fatal violation 
 * of a rooted tree.
 *
 * ✅ The Hybrid Solution:
 * We use an In-degree Array to check for the "Two Parents" violation (directed property), 
 * AND we use DSU to check for the "Cycle" violation (undirected connectivity).
 *
 * ==========================================================================================
 * THE 3 STRUCTURAL CASES
 * ==========================================================================================
 * Adding one edge to a rooted tree results in exactly one of these three cases:
 *
 * 
 * CASE 1: Cycle Only (No node has 2 parents)
 * - The extra edge pointed back to the root. Every node now has exactly 1 parent.
 * - Fix: Standard DSU. The edge that forms the cycle is the culprit.
 *
 * 
 * CASE 2: Two Parents Only (No cycle)
 * - A node `V` has two parents, `U1` and `U2`. 
 * - Fix: The tree is completely valid if we remove one of these edges. As per the rules, 
 * we return the one that appears later in the input (`candidate2`).
 *
 * 
 * CASE 3: Cycle AND Two Parents
 * - A node `V` has two parents, `U1` and `U2`, AND there is a cycle in the graph.
 * - This happens when a node points to one of its own ancestors.
 * - Fix: If we temporarily ignore `candidate2` and a cycle STILL exists, it means 
 * `candidate2` was innocent. `candidate1` must be the edge that caused the cycle!
 *
 * ==========================================================================================
 */

#include <iostream>
#include <vector>

using namespace std;

// ==========================================================================================
// Disjoint Set Union (DSU) Structure
// ==========================================================================================
class DSU {
    vector<int> parent;
public:
    DSU(int n) {
        parent.resize(n + 1);
        for (int i = 0; i <= n; i++) {
            parent[i] = i; // Every node is its own parent initially
        }
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]); // Path compression
        }
        return parent[x];
    }

    bool unite(int u, int v) {
        int rootU = find(u);
        int rootV = find(v);
        
        if (rootU == rootV) {
            return false; // Cycle detected! They are already in the same set.
        }
        
        parent[rootU] = rootV;
        return true;
    }
};

class Solution {
public:
    vector<int> findRedundantDirectedConnection(vector<vector<int>>& edges) {
        int n = edges.size();
        
        // Array to track the parent of each node to detect in-degree 2
        vector<int> parent(n + 1, 0);
        
        vector<int> candidate1; // The earlier edge causing a 2-parent scenario
        vector<int> candidate2; // The later edge causing a 2-parent scenario

        // STEP 1: Scan for a node with two parents
        for (auto& edge : edges) {
            int u = edge[0];
            int v = edge[1];

            if (parent[v] == 0) {
                parent[v] = u; // Assign parent
            } else {
                // Node 'v' already has a parent! We found our two candidates.
                candidate1 = {parent[v], v};
                candidate2 = {u, v};
                
                // Temporarily invalidate candidate2 to see if the graph is fixed
                edge[1] = 0; 
            }
        }

        // STEP 2: Use DSU to check for cycles with the remaining valid edges
        DSU dsu(n);
        
        for (const auto& edge : edges) {
            int u = edge[0];
            int v = edge[1];
            
            if (v == 0) continue; // Skip the temporarily invalidated candidate2

            // If union fails, a cycle is detected
            if (!dsu.unite(u, v)) {
                
                // If we NEVER found a 2-parent node, then we are in CASE 1.
                // The edge that closed the cycle is the answer.
                if (candidate1.empty()) {
                    return edge;
                }
                
                // If we DID find a 2-parent node, but removing candidate2 didn't fix 
                // the cycle, then we are in CASE 3. candidate1 is the culprit.
                return candidate1;
            }
        }

        // STEP 3: No cycles detected. 
        // We are in CASE 2. Removing candidate2 perfectly fixed the tree.
        return candidate2;
    }
};

/**
 * ==================================================================================
 * DRIVER CODE (For Local Testing)
 * ==================================================================================
 */
int main() {
    Solution sol;
    
    // Case 1: Cycle only (Root gets an in-degree)
    vector<vector<int>> edges1 = {{1, 2}, {1, 3}, {2, 3}};
    vector<int> ans1 = sol.findRedundantDirectedConnection(edges1);
    cout << "Test 1 (Expected [2, 3]): [" << ans1[0] << ", " << ans1[1] << "]\n";

    // Case 3: Cycle AND Two Parents
    vector<vector<int>> edges2 = {{1, 2}, {2, 3}, {3, 4}, {4, 1}, {1, 5}};
    vector<int> ans2 = sol.findRedundantDirectedConnection(edges2);
    cout << "Test 2 (Expected [4, 1]): [" << ans2[0] << ", " << ans2[1] << "]\n";

    return 0;
}
