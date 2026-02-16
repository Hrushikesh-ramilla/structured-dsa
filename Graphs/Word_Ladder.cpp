/**
 * ==========================================================================================
 * Problem: 127. Word Ladder
 * Difficulty: Hard
 * Tags: Graph, Breadth-First Search (BFS), Bidirectional BFS, String
 * Source: LeetCode
 * Link: https://leetcode.com/problems/word-ladder/
 * Constraints: 1 <= wordList.length <= 5000, 1 <= word.length <= 10
 * ==========================================================================================
 *
 * PROBLEM SUMMARY:
 * Find the length of the shortest transformation sequence from `beginWord` to `endWord`.
 * - In each step, you can change exactly one letter.
 * - Every intermediate word must exist in `wordList`.
 *
 * ------------------------------------------------------------------------------------------
 * CORE MODELING:
 * - This is an Unweighted Shortest Path problem.
 * - Nodes: The words (beginWord + words in wordList).
 * - Edges: Undirected edges exist between words that differ by exactly 1 character.
 * - Algorithm: Breadth-First Search (BFS) guarantees the shortest path in unweighted graphs.
 *
 * EVOLUTION OF THE SOLUTION:
 * 1. Explicit Graph (The Naive Way): 
 * Compare every pair of words to build an adjacency list, then run BFS. 
 * Bottleneck: Building the graph takes O(N^2 * L) time. (Where N is word count, L is length).
 * * 2. On-the-Fly BFS (The Standard Way): 
 * Don't build the graph! For a given word, generate all possible 1-letter mutations 
 * (L * 26 possibilities) and check if they exist in an unordered_set. 
 * Bottleneck: Solves the O(N^2) issue, but the search tree still grows exponentially wide.
 * * 3. Bidirectional BFS (The Optimal Way):
 * Start searching from BOTH `beginWord` and `endWord` simultaneously. 
 * Always expand the smaller frontier. This drastically reduces the branching factor 
 * and cuts the search space significantly.
 * ==========================================================================================
 */

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <unordered_set>

using namespace std;

class Solution {
public:
    /**
     * ==================================================================================
     * APPROACH 1: Explicit Graph Construction + BFS (Conceptual Baseline)
     * Time Complexity: O(N^2 * L) for graph building, O(N + E) for BFS
     * Space Complexity: O(N^2) for the adjacency list
     * ==================================================================================
     * WHY IT'S SLOW:
     * - If N = 5000, N^2 = 25,000,000 comparisons.
     * - Each comparison takes O(L) time. 
     * - This will likely result in TLE (Time Limit Exceeded) or perform very poorly (800ms+).
     */
    bool differByOne(const string& a, const string& b) {
        int diff = 0;
        for (int i = 0; i < a.size(); i++) {
            if (a[i] != b[i]) diff++;
            if (diff > 1) return false;
        }
        return diff == 1;
    }

    int ladderLength_ExplicitGraph(string beginWord, string endWord, vector<string>& wordList) {
        int n = wordList.size();
        int endIndex = -1;
        
        for (int i = 0; i < n; i++) {
            if (wordList[i] == endWord) endIndex = i;
        }
        if (endIndex == -1) return 0; // endWord not in dictionary

        // 1. Build the Adjacency List
        vector<vector<int>> graph(n);
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                if (differByOne(wordList[i], wordList[j])) {
                    graph[i].push_back(j);
                    graph[j].push_back(i);
                }
            }
        }

        // 2. Run BFS
        queue<int> q;
        vector<int> dist(n, -1);

        // Find initial neighbors of beginWord
        for (int i = 0; i < n; i++) {
            if (differByOne(beginWord, wordList[i])) {
                q.push(i);
                dist[i] = 2; // beginWord is step 1, first neighbor is step 2
            }
        }

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            if (node == endIndex) return dist[node];

            for (int next : graph[node]) {
                if (dist[next] == -1) { // Unvisited
                    dist[next] = dist[node] + 1;
                    q.push(next);
                }
            }
        }
        return 0;
    }

    /**
     * ==================================================================================
     * APPROACH 2: Standard BFS with On-The-Fly Generation
     * Time Complexity: O(N * L^2 * 26) -> Roughly O(N * L) since L <= 10
     * Space Complexity: O(N) for queue and set
     * ==================================================================================
     * THE TRICK:
     * - Instead of checking against all other N words, generate the 26*L possible neighbors.
     * - Since L is small (<=10), 260 string lookups in a hash set is MUCH faster 
     * than comparing against 5000 words.
     */
    int ladderLength_OnTheFly(string beginWord, string endWord, vector<string>& wordList) {
        unordered_set<string> dict(wordList.begin(), wordList.end());
        if (!dict.count(endWord)) return 0;

        queue<pair<string, int>> q;
        q.push({beginWord, 1});

        while (!q.empty()) {
            auto [word, steps] = q.front();
            q.pop();

            if (word == endWord) return steps;

            // Generate all possible 1-character mutations
            for (int i = 0; i < word.size(); i++) {
                char original_char = word[i];
                
                for (char c = 'a'; c <= 'z'; c++) {
                    if (c == original_char) continue;
                    
                    word[i] = c; // Mutate
                    
                    if (dict.count(word)) {
                        q.push({word, steps + 1});
                        dict.erase(word); // Erase acts as visited array to prevent cycles
                    }
                }
                word[i] = original_char; // Backtrack mutation for next iteration
            }
        }
        return 0;
    }

    /**
     * ==================================================================================
     * APPROACH 3: Bidirectional BFS (Optimal for Production/Interviews)
     * Time Complexity: O(N * L^2 * 26 / 2) - Drastically cuts search space
     * Space Complexity: O(N)
     * ==================================================================================
     * WHY IT'S THE BEST:
     * - Standard BFS creates a massive "tree" of possibilities.
     * - By expanding from BOTH the start and the end, the two trees meet in the middle.
     * - We always expand the smaller set (frontier) to keep the branching factor as small 
     * as possible at every step.
     */
    int ladderLength(string beginWord, string endWord, vector<string>& wordList) {
        unordered_set<string> dict(wordList.begin(), wordList.end());
        if (!dict.count(endWord)) return 0;

        // Frontiers for bidirectional search
        unordered_set<string> beginSet{beginWord};
        unordered_set<string> endSet{endWord};
        
        // Remove endWord from dict so we don't visit it from beginSet without noticing
        dict.erase(endWord); 

        int steps = 1;

        while (!beginSet.empty() && !endSet.empty()) {
            // ALWAYS expand the smaller frontier to minimize branching factor
            if (beginSet.size() > endSet.size()) {
                swap(beginSet, endSet);
            }

            unordered_set<string> nextLevelSet;

            // Explore all nodes in the current smaller frontier
            for (string word : beginSet) {
                
                for (int i = 0; i < word.size(); i++) {
                    char original_char = word[i];
                    
                    for (char c = 'a'; c <= 'z'; c++) {
                        if (c == original_char) continue;
                        
                        word[i] = c; // Mutate

                        // If the mutated word is in the OTHER frontier, they have met!
                        if (endSet.count(word)) {
                            return steps + 1;
                        }

                        // If it's a valid intermediate word, add to next level
                        if (dict.count(word)) {
                            nextLevelSet.insert(word);
                            dict.erase(word); // Mark visited
                        }
                    }
                    word[i] = original_char; // Backtrack
                }
            }

            // Move to the next level
            beginSet = nextLevelSet;
            steps++;
        }

        return 0; // Never met
    }
};

/**
 * ==================================================================================
 * DRIVER CODE
 * ==================================================================================
 */
int main() {
    Solution sol;
    
    // Example 1
    string beginWord1 = "hit", endWord1 = "cog";
    vector<string> wordList1 = {"hot", "dot", "dog", "lot", "log", "cog"};
    // "hit" -> "hot" -> "dot" -> "dog" -> "cog" (Length 5)
    cout << "Test 1 (Expected 5): " << sol.ladderLength(beginWord1, endWord1, wordList1) << endl;

    // Example 2
    string beginWord2 = "hit", endWord2 = "cog";
    vector<string> wordList2 = {"hot", "dot", "dog", "lot", "log"};
    // "cog" is not in the list
    cout << "Test 2 (Expected 0): " << sol.ladderLength(beginWord2, endWord2, wordList2) << endl;

    return 0;
}
