/**
 * ==========================================================================================
 * Problem: 32. Longest Valid Parentheses
 * Difficulty: Hard
 * Tags: String, Dynamic Programming, Stack
 * Source: LeetCode
 * Link: https://leetcode.com/problems/longest-valid-parentheses/
 * Constraints: 0 <= s.length <= 3 * 10^4
 * ==========================================================================================
 */

/*
 * ======================================================================================
 * HOW TO READ THIS FILE
 * --------------------------------------------------------------------------------------
 * This file intentionally presents multiple approaches to the same problem.
 *
 * The goal is NOT to show alternatives for completeness,
 * but to show how the SAME invariant can be modeled differently.
 *
 * Read in order:
 *
 *   1) Optimized Brute Force
 *      - Makes validity rules explicit
 *      - Exposes why naive scanning is slow
 *
 *   2) Stack + DP (PRIMARY SOLUTION)
 *      - Encodes invalid boundaries explicitly
 *      - Most explainable and robust formulation
 *
 *   3) Two-Pass Scan
 *      - Removes the stack by symmetry
 *      - Space-optimal but conceptually subtle
 * ======================================================================================
 */

#include <bits/stdc++.h>
using namespace std;

/*
 * ======================================================================================
 * APPROACH 1: OPTIMIZED BRUTE FORCE
 * ======================================================================================
 * Idea:
 *   Fix a start index and scan forward.
 *   Maintain a balance counter:
 *     '(' -> +1
 *     ')' -> -1
 *
 * Rules:
 *   - If balance < 0 : invalid immediately (too many ')')
 *   - If balance == 0: valid substring found
 *
 * Time Complexity: O(N^2)
 * Space Complexity: O(1)
 *
 * Purpose:
 *   This approach is NOT meant to pass.
 *   It exists to make the validity conditions explicit.
 * ======================================================================================
 */
int longestValidParentheses_Brute(const string& s) {
    int n = s.size();
    int maxLength = 0;

    for (int start = 0; start < n; start++) {
        int balance = 0;

        for (int end = start; end < n; end++) {
            // Update balance based on current character
            if (s[end] == '(') balance++;
            else balance--;

            // Too many closing brackets → invalid
            if (balance < 0) break;

            // Perfectly balanced substring
            if (balance == 0) {
                maxLength = max(maxLength, end - start + 1);
            }
        }
    }
    return maxLength;
}

/*
 * ======================================================================================
 * TRANSITION: BRUTE FORCE -> STACK
 * --------------------------------------------------------------------------------------
 * Observation:
 *   Validity depends on:
 *     1) Balance
 *     2) WHERE balance breaks
 *
 * Brute force repeatedly rediscovers invalid boundaries.
 *
 * Key realization:
 *   We only need to remember:
 *     - indices of unmatched '('
 *     - indices of unmatched ')'
 *
 * A stack is the minimal structure that stores exactly this.
 * ======================================================================================
 */

/*
 * ======================================================================================
 * CORE STACK INSIGHT
 * --------------------------------------------------------------------------------------
 * The stack does NOT store characters.
 * It stores BOUNDARY INDICES.
 *
 * Invariant:
 *   stack.top() always represents the last index BEFORE
 *   which a valid substring cannot extend.
 *
 * Therefore:
 *   If a valid substring ends at index i,
 *   its length is:
 *       i - stack.top()
 * ======================================================================================
 */

/*
 * ======================================================================================
 * APPROACH 2: STACK + DP (PRIMARY SOLUTION)
 * ======================================================================================
 * Definitions:
 *   Use 1-based indexing for clarity.
 *
 *   dp[i] = length of the longest valid parentheses substring
 *           that ENDS exactly at position i.
 *
 * Stack Meaning:
 *   Stores indices of unmatched parentheses (boundaries).
 *
 * Transition:
 *   If '(' at index j matches ')' at index i:
 *       dp[i] = dp[j - 1] + (i - j + 1)
 *
 * This supports:
 *   - "()()"  → chaining
 *   - "(())"  → nesting
 *
 * Time Complexity: O(N)
 * Space Complexity: O(N)
 * ======================================================================================
 */
class Solution {
public:
    int longestValidParentheses(string s) {
        int n = s.size();

        // dp[i] = longest valid substring ending exactly at i (1-based)
        vector<int> dp(n + 1, 0);

        // Stack stores boundary indices (1-based)
        stack<int> boundaryStack;

        int maxLength = 0;

        // Process string using 1-based indexing
        for (int currentIndex = 1; currentIndex <= n; currentIndex++) {

            // Case 1: current char is ')', try to match
            if (!boundaryStack.empty() && s[currentIndex - 1] == ')') {

                int possibleMatchIndex = boundaryStack.top();

                // Check if this boundary is an opening '('
                if (s[possibleMatchIndex - 1] == '(') {
                    boundaryStack.pop();

                    // Extend with any valid substring ending before the match
                    dp[currentIndex] =
                        dp[possibleMatchIndex - 1] + (currentIndex - possibleMatchIndex + 1);

                    maxLength = max(maxLength, dp[currentIndex]);
                }
                // else: unmatched ')', treated as boundary
            }
            else {
                // Case 2:
                // Either '(' OR unmatched ')'
                // This index becomes a boundary
                boundaryStack.push(currentIndex);
            }
        }

        return maxLength;
    }
};

/*
 * ======================================================================================
 * TRANSITION: STACK -> TWO PASS SCAN
 * --------------------------------------------------------------------------------------
 * Observation:
 *   The stack is only used to detect invalid boundaries.
 *
 * Those boundaries can sometimes be detected implicitly:
 *   - Too many ')' when scanning left → right
 *   - Too many '(' when scanning right → left
 *
 * By scanning in both directions,
 * all maximal valid substrings are detected.
 * ======================================================================================
 */

/*
 * ======================================================================================
 * APPROACH 3: TWO PASS COUNTER SCAN
 * ======================================================================================
 * Pass 1 (Left → Right):
 *   Reset when ')' exceed '('
 *
 * Pass 2 (Right → Left):
 *   Reset when '(' exceed ')'
 *
 * Time Complexity: O(N)
 * Space Complexity: O(1)
 *
 * Tradeoff:
 *   Less explicit than stack,
 *   easier to get wrong,
 *   but space-optimal.
 * ======================================================================================
 */
int longestValidParentheses_TwoPass(const string& s) {
    int openCount = 0;
    int closeCount = 0;
    int maxLength = 0;

    // Left to Right scan
    for (char c : s) {
        if (c == '(') openCount++;
        else closeCount++;

        if (openCount == closeCount) {
            maxLength = max(maxLength, 2 * closeCount);
        }
        else if (closeCount > openCount) {
            openCount = closeCount = 0;
        }
    }

    // Right to Left scan
    openCount = closeCount = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        if (s[i] == '(') openCount++;
        else closeCount++;

        if (openCount == closeCount) {
            maxLength = max(maxLength, 2 * openCount);
        }
        else if (openCount > closeCount) {
            openCount = closeCount = 0;
        }
    }

    return maxLength;
}

/*
 * ======================================================================================
 * PRACTICAL TAKEAWAYS
 * --------------------------------------------------------------------------------------
 * - Brute force clarifies validity rules.
 * - Stack + DP is the clearest and safest formulation.
 * - Two-pass scan is elegant but subtle.
 *
 * SAME IDEA everywhere:
 *   invalid positions reset the start of a valid substring.
 *
 * Different solutions differ ONLY in how that reset is tracked.
 * ======================================================================================
 */
