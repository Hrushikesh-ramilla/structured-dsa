/*
 * ======================================================================================
 * Problem: 1425. Constrained Subsequence Sum
 * Difficulty: Hard
 * Source: LeetCode
 * Link: https://leetcode.com/problems/constrained-subsequence-sum/
 * Focus: Transition from O(NK) -> O(N log N) -> O(N)
 * ======================================================================================
 * PROBLEM SUMMARY:
 * We need to select a non-empty subsequence from `nums` to maximize the sum.
 * Constraint: If we pick `nums[i]` and the previous pick was `nums[j]` (j < i),
 * then `i - j <= k`.
 *
 * CORE DP STATE:
 * Let `dp[i]` be the maximum subsequence sum ending exactly at index `i`.
 * To compute `dp[i]`, we may extend any subsequence ending at `j` where
 * `i-k <= j < i`.
 *
 * RECURRENCE:
 *   dp[i] = nums[i] + max(0, max(dp[j])) for j in [i-k, i-1]
 *
 * Note:
 * We take max(0, ...) because extending a negative-sum subsequence is worse
 * than starting fresh at the current index.
 *
 * The only difference between the solutions below is how the inner maximum
 * over the sliding window is maintained.
 * ======================================================================================
 */

/*
 * ======================================================================================
 * HOW TO READ THIS FILE
 * --------------------------------------------------------------------------------------
 * This file intentionally contains three approaches to the same problem.
 *
 * The purpose is not to list multiple answers, but to document the natural
 * optimization path that emerges during problem solving:
 *
 *   1) Brute DP        — derive the recurrence and expose the bottleneck
 *   2) DP + Max Heap  — remove the bottleneck without changing the DP logic
 *   3) DP + Deque     — enforce a stronger invariant to eliminate log factors
 *
 * Each step answers a concrete question:
 *   - What exactly is slow?
 *   - What information is actually needed?
 *   - Can that information be maintained incrementally?
 *
 * This mirrors how the solution is discovered, not just how it is implemented.
 * ======================================================================================
 */


#include <vector>
#include <deque>
#include <queue>
#include <algorithm>
#include <iostream>

using namespace std;

class Solution {
public:
    /* * ==================================================================================
     * APPROACH 1: BRUTE FORCE DP
     * Time Complexity: O(N * K)
     * Space Complexity: O(N)
     * ==================================================================================
     * WHY: 
     * This is the mathematical definition of the recurrence.
     * * BOTTLENECK:
     * For every `i`, we iterate `k` times to find the max of the previous window.
     * If N=10^5 and K=10^5, this is 10^10 operations -> TLE.
     */
    int constrainedSubsetSum_Brute(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> dp(n);
        int ans = nums[0];

        for (int i = 0; i < n; i++) {
            dp[i] = nums[i];
            
            // Scan the window [i-k, i-1] explicitly
            int max_prev = 0;
            for (int j = max(0, i - k); j < i; j++) {
                max_prev = max(max_prev, dp[j]);
            }
            
            dp[i] += max_prev;
            ans = max(ans, dp[i]);
        }
        return ans;
    }

/*
 * ======================================================================================
 * TRANSITION: BRUTE -> HEAP
 * --------------------------------------------------------------------------------------
 * Observation from brute force:
 *   dp[i] only depends on the MAX dp[j] in the window [i-k, i-1].
 *
 * We are recomputing the same window maximum over and over.
 *
 * Key realization:
 *   The DP recurrence itself is already optimal.
 *   The ONLY inefficiency is how we compute the window maximum.
 *
 * Next step:
 *   Replace the inner loop with a data structure that supports:
 *     - insert
 *     - get maximum
 *
 * A max heap gives us this immediately.
 * ======================================================================================
 */


    /* * ==================================================================================
     * APPROACH 2: DP + MAX HEAP (Priority Queue)
     * Time Complexity: O(N log N)
     * Space Complexity: O(N)
     * ==================================================================================
     * OPTIMIZATION:
     * We need the max of a sliding window. A Max Heap naturally keeps the max at the top.
     * * CHALLENGE:
     * Heaps don't support random removal. If the max element is outside the window 
     * (index < i-k), we can't easily find and delete it.
     * * SOLUTION (LAZY REMOVAL):
     * We don't remove elements immediately when they leave the window.
     * We only remove the TOP element if it is "expired" (index < i-k) when we try to use it.
     */
    int constrainedSubsetSum_Heap(vector<int>& nums, int k) {
        int n = nums.size();
        // Store {dp_val, index}. Max heap based on dp_val.
        priority_queue<pair<int, int>> pq;
        int ans = nums[0];

        for (int i = 0; i < n; i++) {
            // 1. Clean up "expired" tops
            while (!pq.empty() && pq.top().second < i - k) {
                pq.pop();
            }

            // 2. Get max from valid window
            int max_prev = 0;
            if (!pq.empty()) {
                max_prev = max(0, pq.top().first);
            }

            int current_val = nums[i] + max_prev;
            ans = max(ans, current_val);

            // 3. Push current state to heap
            pq.push({current_val, i});
        }
        return ans;
    }

/*
 * ======================================================================================
 * TRANSITION: HEAP -> MONOTONIC DEQUE
 * --------------------------------------------------------------------------------------
 * Observation from heap solution:
 *   - We still maintain a sliding window maximum
 *   - But every operation costs log(N)
 *
 * What extra structure does the heap maintain?
 *   - A full ordering that we never actually need
 *
 * What we truly need:
 *   - Access to the current maximum
 *   - Removal of elements that become irrelevant
 *
 * Stronger observation:
 *   If a newer dp value is >= an older dp value,
 *   the older one can NEVER be useful again.
 *
 * This allows us to enforce a monotonic invariant and
 * achieve O(1) amortized operations using a deque.
 * ======================================================================================
 */


    /* * ==================================================================================
     * APPROACH 3: DP + MONOTONIC DEQUE (OPTIMAL)
     * Time Complexity: O(N)
     * Space Complexity: O(N)
     * ==================================================================================
     * WHY BEST?
     * We don't need a full sort (Heap). We only care about "potential" maximums.
     * * KEY INSIGHT:
     * If we have two indices `j1` and `j2` in the window such that `j1 < j2` (j1 is older)
     * and `dp[j1] <= dp[j2]` (j1 is smaller/weaker):
     * Then `j1` is USELESS. It will expire sooner than `j2` and is smaller than `j2`.
     * * INVARIANT:
     * The Deque will store indices such that their corresponding `dp` values are strictly DECREASING.
     * - dq.front(): The largest value in the current window.
     * - dq.back(): The newest value added.
     */
    int constrainedSubsetSum(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> dp(n);
        deque<int> dq; // Stores indices
        int ans = nums[0];

        for (int i = 0; i < n; i++) {
            // 1. Clean the front (remove expired indices)
            // If the best candidate is too old (index < i-k), toss it.
            if (!dq.empty() && dq.front() < i - k) {
                dq.pop_front();
            }

            // 2. Calculate DP[i]
            // The best previous sum is at dq.front() (due to decreasing order invariant)
            int max_prev = (!dq.empty()) ? max(0, dp[dq.front()]) : 0;
            dp[i] = nums[i] + max_prev;
            ans = max(ans, dp[i]);

            // 3. Maintain Monotonicity (Clean the back)
            // Before adding i, remove any j that is "worse" than i (dp[j] <= dp[i]).
            // Because i is newer and bigger, j will never be the max again.
            while (!dq.empty() && dp[dq.back()] <= dp[i]) {
                dq.pop_back();
            }

            dq.push_back(i);
        }
        return ans;
    }
};

/*
 * ==================================================================================
 * DRIVER CODE (For Local Testing)
 * ==================================================================================
 */
int main() {
    Solution sol;
    
    // Example 1
    vector<int> nums1 = {10, 2, -10, 5, 20};
    int k1 = 2;
    cout << "Test 1 (Expected 37): " << sol.constrainedSubsetSum(nums1, k1) << endl;

    // Example 2
    vector<int> nums2 = {-1, -2, -3};
    int k2 = 1;
    cout << "Test 2 (Expected -1): " << sol.constrainedSubsetSum(nums2, k2) << endl;

    // Example 3
    vector<int> nums3 = {10, -2, -10, -5, 20};
    int k3 = 2;
    cout << "Test 3 (Expected 23): " << sol.constrainedSubsetSum(nums3, k3) << endl;

    return 0;
}

/*
 * ======================================================================================
 * PRACTICAL TAKEAWAYS
 * --------------------------------------------------------------------------------------
 * - Use the BRUTE DP to derive and verify the recurrence.
 * - Use the HEAP version when:
 *     * constraints are moderate
 *     * implementation speed matters more than constant factors
 * - Use the MONOTONIC DEQUE when:
 *     * constraints are tight
 *     * this sliding-window DP pattern appears repeatedly
 *
 * The DP never changes.
 * Only the way we maintain the window maximum does.
 * ======================================================================================
 */
