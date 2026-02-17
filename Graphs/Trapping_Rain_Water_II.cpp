/**
 * ==========================================================================================
 * Problem: 407. Trapping Rain Water II
 * Difficulty: Hard
 * Tags: Graph, BFS, Dijkstra, Priority Queue, Minimax Path
 * Source: LeetCode
 * Link: https://leetcode.com/problems/trapping-rain-water-ii/
 * ==========================================================================================
 *
 * PROBLEM SUMMARY:
 * Given an m x n grid of elevations, compute the total volume of water trapped inside 
 * the grid after raining.
 * - Water flows from higher cells to lower cells.
 * - Water drains off if it reaches the boundary.
 * - We need to find the "steady state" water level for every cell.
 *
 * CONSTRAINTS:
 * 1 <= m, n <= 200
 * 0 <= heightMap[i][j] <= 2 * 10^4
 * Time Complexity Limit: ~O(MN log MN) is acceptable.
 *
 * ==========================================================================================
 * ANALYSIS & EVOLUTION OF THOUGHT
 * ==========================================================================================
 *
 * 1. WHY 1D LOGIC FAILS (The "Trap"):
 * In 1D Trapping Rain Water, a cell's water level is determined by min(max_left, max_right).
 * In 2D (Matrix), one might think: min(max_row_left, max_row_right, max_col_up, max_col_down).
 * THIS IS WRONG. Water can leak "diagonally" or through a winding path.
 * The water level is determined by the *shortest/lowest* boundary on *any* path to the edge.
 *
 * 2. THE CORRECT MENTAL MODEL: "Sea Level Rise"
 * - Imagine the grid is an island. The boundary is the ocean.
 * - The ocean level rises. It will flood the island starting from the lowest point on the shore.
 * - Once water enters a "valley" (a cell lower than the current water level), 
 * it fills that valley up to the current level.
 * - That filled valley effectively becomes a new "solid ground" of water.
 *
 * 3. MODELING AS A GRAPH PROBLEM (Dijkstra Variant):
 * - We are looking for the "lowest possible maximum height" on a path from the boundary.
 * - This is a Minimax path problem.
 * - Algorithm: Multi-Source Dijkstra (using a Min-Heap).
 *
 * 4. THE INVARIANTS (Crucial for Proof):
 * a) Min-Heap Property: We always process the boundary cell with the *lowest* effective height.
 * Why? Because water always leaks from the lowest point first.
 *
 * b) Visited Logic: The first time we pop a cell from the heap, we have reached it via the
 * lowest possible boundary path. (Standard Dijkstra correctness).
 *
 * c) The Max Operator: When pushing a neighbor `(nr, nc)` from `(r, c)`:
 * new_height = max(height[nr][nc], effective_height[r][c])
 * Why? 
 * - If neighbor is TALLER: It blocks water. It becomes a new wall of its own height.
 * - If neighbor is SHORTER: It gets flooded. The water surface becomes the new "floor".
 * Its effective height rises to match the water level.
 *
 * ==========================================================================================
 */

#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
#include <iostream>

using namespace std;

// CP Macros for cleaner implementation
using ll = long long;
using vi = vector<int>;
using vvi = vector<vi>;
using pii = pair<int, int>;
using tiii = tuple<int, int, int>; // {height, row, col}

// Min-Heap Definition: Stores {height, row, col}, ordered by height ascending
using min_pq = priority_queue<tiii, vector<tiii>, greater<tiii>>;

class Solution {
public:
    int trapRainWater(vector<vector<int>>& heightMap) {
        int n = heightMap.size();
        int m = heightMap[0].size();
        
        // Edge case: 1xN or Nx1 grids cannot trap water
        if (n <= 2 || m <= 2) return 0;

        // Min-Heap stores {effective_height, r, c}
        // "effective_height" is the water level boundary reaching this cell
        min_pq pq;
        
        // Visited array to ensure each cell is processed exactly once (Dijkstra style)
        vector<vector<bool>> vis(n, vector<bool>(m, false));

        // ==================================================================================
        // STEP 1: INITIALIZATION (Push Boundaries)
        // ==================================================================================
        // The boundary of the grid is the initial "container wall".
        // Water can never be trapped ON the boundary (it spills out).
        
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (i == 0 || i == n - 1 || j == 0 || j == m - 1) {
                    pq.push({heightMap[i][j], i, j});
                    vis[i][j] = true;
                }
            }
        }

        int waterTrapped = 0;
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

        // ==================================================================================
        // STEP 2: FLOOD FILL FROM LOWEST BOUNDARY (Dijkstra)
        // ==================================================================================
        while (!pq.empty()) {
            auto [h, r, c] = pq.top();
            pq.pop();

            // 'h' is the bottleneck height that allowed us to reach (r, c).
            // It represents the current water level at this boundary.

            // Explore all 4 neighbors
            for (int i = 0; i < 4; i++) {
                int nr = r + dr[i];
                int nc = c + dc[i];

                // Bounds check
                if (nr < 0 || nr >= n || nc < 0 || nc >= m) continue;
                
                // If already visited, it means we reached it via a lower or equal boundary before.
                // Dijkstra guarantees the first visit is optimal.
                if (vis[nr][nc]) continue;

                // ==========================================================================
                // CORE LOGIC: TRAPPING WATER
                // ==========================================================================
                // We are at boundary height 'h'. We look at neighbor 'nh'.
                int nh = heightMap[nr][nc];

                // Case 1: nh < h
                // The neighbor is lower than our current water wall. 
                // Water spills from (r,c) into (nr,nc) filling it up to 'h'.
                if (nh < h) {
                    waterTrapped += (h - nh);
                }

                // ==========================================================================
                // CORE LOGIC: PUSHING TO HEAP
                // ==========================================================================
                // What is the effective height of (nr, nc) now?
                // It is max(nh, h).
                // - If nh < h: It is filled with water to level 'h'. Effectively a wall of height 'h'.
                // - If nh >= h: It blocks water. It is a wall of height 'nh'.
                
                pq.push({max(nh, h), nr, nc});
                vis[nr][nc] = true; // Mark visited ON PUSH to prevent duplicates
            }
        }

        return waterTrapped;
    }
};

/**
 * ==========================================================================================
 * DEEP DIVE: COMMON DOUBTS & ANSWERS
 * ==========================================================================================
 *
 * Q1: "Why do we use max(h, neighbor_h) when pushing?"
 * A1: Because the heap represents the boundary. If you pour water into a cell of height 2
 * from a wall of height 5, the water fills the cell. The surface of the water is now at 5.
 * Any further expansion inward sees a wall of height 5 (the water surface), not 2.
 * If the neighbor was 8 (taller than 5), the water stops. The new wall is 8.
 * Hence: new_effective_height = max(current_wall, neighbor_height).
 *
 * Q2: "What if there is no smaller neighbor?"
 * A2: That's fine. It means no water is trapped at this specific step.
 * We still push the neighbor (with its larger height) into the heap.
 * It simply becomes a higher boundary wall that will be processed later.
 *
 * Q3: "Why don't we need to check if a better path exists later?"
 * A3: This is the Dijkstra property. The Min-Heap ensures we always expand the
 * globally smallest boundary first. If we reach a cell, we are guaranteed
 * to have reached it via the lowest possible enclosing wall. 
 * No "lower" path can be found later because all other paths start 
 * from boundaries >= current_min.
 *
 * ==========================================================================================
 */
