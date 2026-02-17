/**
 * ==========================================================================================
 * Problem: 329. Longest Increasing Path in a Matrix
 * Difficulty: Hard
 * Tags: Graph, DFS, Memoization (Top-Down DP), Topological Sort (Bottom-Up DP)
 * Source: LeetCode
 * Link: https://leetcode.com/problems/longest-increasing-path-in-a-matrix/
 * ==========================================================================================
 *
 * PROBLEM SUMMARY:
 * Given an m x n integer matrix, return the length of the longest strictly increasing path.
 * You can move in 4 directions (up, down, left, right). No diagonal movement.
 *
 * ==========================================================================================
 * HOW TO READ THIS FILE & EVOLUTION OF THOUGHT
 * ==========================================================================================
 *
 * 1. THE CORE OBSERVATION: No Cycles Allowed.
 * Because the path must be *strictly* increasing, it is impossible to visit the same
 * cell twice (you can't go 3 -> 4 -> 3). 
 * This means the matrix naturally forms a Directed Acyclic Graph (DAG) where directed
 * edges point from smaller cells to larger neighbors.
 *
 * 2. APPROACH 1: Brute DFS
 * - Idea: Start a DFS from every single cell. Explore all valid increasing neighbors.
 * - Why it fails: Recomputes the same paths repeatedly. If a path from cell 'A' goes
 * through cell 'B', we compute the longest path from 'B'. Later, if we start a new
 * DFS from cell 'C' that also reaches 'B', we compute 'B' all over again.
 * - Complexity: Exponential worst-case.
 *
 * 3. APPROACH 2: DFS + Memoization (The Standard Interview Solution)
 * - Idea: Cache the results. `dp[i][j]` = the longest increasing path starting from `(i, j)`.
 * - If we reach a cell we've already calculated, just return the cached value.
 * - Complexity: O(M * N) Time and Space. Every cell and edge is processed once.
 *
 * 4. APPROACH 3: DAG + Topological Sort (The Graph Theory Solution)
 * - Idea: Since it's a DAG, we can find the longest path by processing nodes in
 * Topological Order (using Kahn's Algorithm).
 * - Start with all nodes that have in-degree 0 (local minimums; no smaller neighbors).
 * - As we process a node, we "relax" its outgoing edges, updating the longest path
 * to its neighbors, just like traversing a layered DP table.
 * - Complexity: O(M * N) Time and Space.
 * ==========================================================================================
 */

#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>

using namespace std;

class Solution {
public:
    /**
     * ==================================================================================
     * APPROACH 1: Brute DFS (Conceptual Baseline)
     * Time Complexity: O(4^(M*N)) in the worst case
     * Space Complexity: O(M*N) for the recursion stack
     * ==================================================================================
     * Notes: Will trigger Time Limit Exceeded (TLE) on LeetCode.
     */
    int dfs_brute(const vector<vector<int>>& matrix, int r, int c) {
        int n = matrix.size();
        int m = matrix[0].size();
        int maxLen = 1;

        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            if (nr >= 0 && nr < n && nc >= 0 && nc < m && matrix[nr][nc] > matrix[r][c]) {
                maxLen = max(maxLen, 1 + dfs_brute(matrix, nr, nc));
            }
        }
        return maxLen;
    }

    int longestIncreasingPath_Brute(vector<vector<int>>& matrix) {
        if (matrix.empty()) return 0;
        int max_path = 0;
        for (int i = 0; i < matrix.size(); ++i) {
            for (int j = 0; j < matrix[0].size(); ++j) {
                max_path = max(max_path, dfs_brute(matrix, i, j));
            }
        }
        return max_path;
    }

    /**
     * ==================================================================================
     * APPROACH 2: DFS + Memoization (Top-Down DP)
     * Time Complexity: O(M * N) - Each cell is evaluated fully exactly once.
     * Space Complexity: O(M * N) - For the memoization table and recursion stack.
     * ==================================================================================
     * Notes: This is the most common and easiest-to-write optimal solution in interviews.
     */
    int dfs_memo(const vector<vector<int>>& matrix, vector<vector<int>>& memo, int r, int c) {
        // Return cached result if already computed
        if (memo[r][c] != 0) return memo[r][c];

        int n = matrix.size();
        int m = matrix[0].size();
        int maxLen = 1;

        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i];
            int nc = c + dc[i];

            // Only move to strictly strictly greater neighbors
            if (nr >= 0 && nr < n && nc >= 0 && nc < m && matrix[nr][nc] > matrix[r][c]) {
                maxLen = max(maxLen, 1 + dfs_memo(matrix, memo, nr, nc));
            }
        }

        // Cache the result before returning
        memo[r][c] = maxLen;
        return maxLen;
    }

    int longestIncreasingPath_Memoization(vector<vector<int>>& matrix) {
        if (matrix.empty()) return 0;
        int n = matrix.size();
        int m = matrix[0].size();
        
        vector<vector<int>> memo(n, vector<int>(m, 0));
        int max_path = 0;

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                max_path = max(max_path, dfs_memo(matrix, memo, i, j));
            }
        }
        return max_path;
    }

    /**
     * ==================================================================================
     * APPROACH 3: DAG + Topological Sort (Bottom-Up Kahn's Algorithm)
     * Time Complexity: O(M * N) - Graph construction and sorting both take linear time.
     * Space Complexity: O(M * N) - To store the explicit graph and in-degrees.
     * ==================================================================================
     * Notes: This perfectly models the problem as a "Longest Path in a DAG" scenario.
     */
    int longestIncreasingPath(vector<vector<int>>& matrix) {
        if (matrix.empty()) return 0;
        int n = matrix.size();
        int m = matrix[0].size();
        int total_nodes = n * m;

        // Flatten the 2D grid into a 1D graph (0 to n*m - 1)
        vector<vector<int>> graph(total_nodes);
        vector<int> indegree(total_nodes, 0);

        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

        // 1. Build the DAG and compute in-degrees
        for (int r = 0; r < n; ++r) {
            for (int c = 0; c < m; ++c) {
                int u = r * m + c; // Flattened 1D index

                for (int i = 0; i < 4; ++i) {
                    int nr = r + dr[i];
                    int nc = c + dc[i];

                    if (nr >= 0 && nr < n && nc >= 0 && nc < m && matrix[nr][nc] > matrix[r][c]) {
                        int v = nr * m + nc;
                        graph[u].push_back(v); // Directed edge from smaller to larger
                        indegree[v]++;
                    }
                }
            }
        }

        // 2. Initialize Queue with nodes having 0 in-degree (Local Minimums)
        queue<int> q;
        for (int i = 0; i < total_nodes; ++i) {
            if (indegree[i] == 0) {
                q.push(i);
            }
        }

        // DP array to store the longest path ending at each node. 
        // Initialized to 1 because every cell itself is a path of length 1.
        vector<int> dp(total_nodes, 1);
        int max_path = 1;

        // 3. Process nodes in Topological Order
        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int v : graph[u]) {
                // Edge Relaxation: The longest path ending at 'v' is at least
                // the longest path ending at 'u' + 1.
                dp[v] = max(dp[v], dp[u] + 1);
                max_path = max(max_path, dp[v]);

                indegree[v]--;
                // If all dependencies of 'v' have been processed, add it to queue
                if (indegree[v] == 0) {
                    q.push(v);
                }
            }
        }

        return max_path;
    }
};

/**
 * ==================================================================================
 * DRIVER CODE (For Local Testing)
 * ==================================================================================
 */
int main() {
    Solution sol;
    
    // Test Case 1
    vector<vector<int>> matrix1 = {
        {9, 9, 4},
        {6, 6, 8},
        {2, 1, 1}
    };
    // Expected: 4 (The longest increasing path is [1, 2, 6, 9])
    cout << "Test Case 1 (Expected 4): " << sol.longestIncreasingPath(matrix1) << "\n";

    // Test Case 2
    vector<vector<int>> matrix2 = {
        {3, 4, 5},
        {3, 2, 6},
        {2, 2, 1}
    };
    // Expected: 4 (The longest increasing path is [3, 4, 5, 6]. 
    // Moving from [1,2] -> [2,2] is not strictly increasing)
    cout << "Test Case 2 (Expected 4): " << sol.longestIncreasingPath(matrix2) << "\n";

    return 0;
}
