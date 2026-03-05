/**
 * ==========================================================================================
 * Problem: 327. Count of Range Sum
 * Difficulty: Hard
 * Tags: Prefix Sum, Fenwick Tree, Coordinate Compression, Divide & Conquer (Merge Sort)
 * Source: LeetCode
 * Link: https://leetcode.com/problems/count-of-range-sum/
 * ==========================================================================================
 *
 * PROBLEM SUMMARY:
 * Given an integer array nums and two integers lower and upper,
 * return the number of range sums that lie in [lower, upper].
 *
 * Range sum S(i, j) = nums[i] + nums[i+1] + ... + nums[j]
 *
 * ==========================================================================================
     EVOLUTION OF LOGIC
 * ==========================================================================================
 *
 * CORE IDEA: Prefix Sum Transformation
 *
 * Let prefix[i] = sum of first i elements.
 *
 * Then:
 *
 *      sum(i, j) = prefix[j+1] - prefix[i]
 *
 * We want:
 *
 *      lower ≤ prefix[j] - prefix[i] ≤ upper
 *
 * Rearranging:
 *
 *      prefix[j] - upper ≤ prefix[i] ≤ prefix[j] - lower
 *
 * So for each prefix[j], we must count how many earlier prefix[i]
 * values lie in this range.
 *
 * ==========================================================================================
 *
 * APPROACH 1: Brute Force
 *
 * Compute every subarray sum.
 *
 * Complexity:
 *      Time  : O(N^2)
 *      Space : O(1)
 *
 * Too slow for N = 1e5.
 *
 * ==========================================================================================
 *
 * APPROACH 2: Fenwick Tree + Coordinate Compression
 *
 * Steps:
 *
 * 1. Compute prefix sums.
 * 2. Compress prefix values to indices.
 * 3. Use Fenwick Tree to store frequency of previous prefix sums.
 * 4. For each prefix[j], count prefixes in:
 *
 *      [prefix[j] - upper , prefix[j] - lower]
 *
 * Complexity:
 *
 *      Time  : O(N log N)
 *      Space : O(N)
 *
 * ==========================================================================================
 *
 * APPROACH 3: Merge Sort Counting (Divide & Conquer)
 *
 * Observation:
 *
 * During merge sort, left half prefixes are already processed.
 *
 * For each prefix[i] in left half, find how many prefix[j] in right half satisfy:
 *
 *      lower ≤ prefix[j] - prefix[i] ≤ upper
 *
 * Using two pointers while merging.
 *
 * This is similar to the classic:
 *
 *      "count of smaller numbers after self"
 *
 * Complexity:
 *
 *      Time  : O(N log N)
 *      Space : O(N)
 *
 * ==========================================================================================
 */

#include<bits/stdc++.h>
using namespace std;

using ll=long long;
using vi=vector<int>;
using vll=vector<ll>;
using pii=pair<int,int>;
using pll=pair<ll,ll>;
using vpi=vector<pii>;
using vpll=vector<pll>;
using vvi=vector<vi>;
using vvll=vector<vll>;
using bin=__int128;
using mxhp=priority_queue<int>;
using mnhp=priority_queue<int,vector<int>,greater<int>>;

#define wl(l,h) while((l)<=(h))
#define fl(i,a,n) for(int i=(a);i<(n);i++)
#define rl(i,n,a) for(int i=(n);i>=(a);i--)
#define all(v) (v).begin(),(v).end()
#define rall(v) (v).rbegin(),(v).rend()
#define pb push_back
#define ppb pop_back
#define pf push_front
#define ppf pop_front
#define hmap unordered_map
#define fwu(i,a,n) for(int i=(a);i<=(n);i+=(i&-i))
#define fwd(idx) for(int i=(idx);i>0;i-=(i&-i))
#define fastio() ios::sync_with_stdio(false);cin.tie(nullptr)

class Solution {
public:

/**
 * ==========================================================================================
 * APPROACH 1: Brute Force
 * ==========================================================================================
 */

int countRangeSum_Brute(vector<int>& nums, int lower, int upper) {

    int n=nums.size();
    int ans=0;

    fl(i,0,n){
        ll sum=0;
        fl(j,i,n){
            sum+=nums[j];
            if(sum>=lower && sum<=upper) ans++;
        }
    }

    return ans;
}

/**
 * ==========================================================================================
 * APPROACH 2: Fenwick Tree + Coordinate Compression
 * ==========================================================================================
 */

struct Fenwick{

    vll bit;
    int n;

    Fenwick(int nn){
        n=nn;
        bit.resize(n+1,0);
    }

    void upd(int idx){
        fwu(i,idx,n)
            bit[i]++;
    }

    ll qry(int idx){
        ll sum=0;
        fwd(idx)
            sum+=bit[i];
        return sum;
    }

    ll rqry(int l,int r){
        return qry(r)-qry(l-1);
    }
};

int countRangeSum_Fenwick(vector<int>& nums, int lower, int upper){

    int n=nums.size();

    vll pfx(n+1,0);

    fl(i,1,n+1)
        pfx[i]=pfx[i-1]+nums[i-1];

    auto tmp=pfx;

    sort(all(tmp));
    tmp.erase(unique(all(tmp)),tmp.end());

    Fenwick fenw(tmp.size());

    int st=lower_bound(all(tmp),pfx[0])-tmp.begin()+1;

    fenw.upd(st);

    int ans=0;

    fl(i,1,n+1){

        int lb=lower_bound(all(tmp),pfx[i]-upper)-tmp.begin()+1;
        int ub=upper_bound(all(tmp),pfx[i]-lower)-tmp.begin();

        int rnk=lower_bound(all(tmp),pfx[i])-tmp.begin()+1;

        ans+=fenw.rqry(lb,ub);

        fenw.upd(rnk);
    }

    return ans;
}

/**
 * ==========================================================================================
 * APPROACH 3: Merge Sort Based Counting
 * ==========================================================================================
 */

ll mergeCount(vll &pfx,int l,int r,int lower,int upper){

    if(r-l<=1) return 0;

    int mid=(l+r)/2;

    ll cnt=0;

    cnt+=mergeCount(pfx,l,mid,lower,upper);
    cnt+=mergeCount(pfx,mid,r,lower,upper);

    int j=mid,k=mid,t=mid;
    vector<ll> cache;

    for(int i=l;i<mid;i++){

        while(k<r && pfx[k]-pfx[i]<lower) k++;
        while(j<r && pfx[j]-pfx[i]<=upper) j++;

        cnt+=j-k;
    }

    merge(pfx.begin()+l,pfx.begin()+mid,
          pfx.begin()+mid,pfx.begin()+r,
          back_inserter(cache));

    copy(cache.begin(),cache.end(),pfx.begin()+l);

    return cnt;
}

int countRangeSum_MergeSort(vector<int>& nums,int lower,int upper){

    int n=nums.size();

    vll pfx(n+1,0);

    fl(i,1,n+1)
        pfx[i]=pfx[i-1]+nums[i-1];

    return mergeCount(pfx,0,n+1,lower,upper);
}


/**
 * ==========================================================================================
 * DEFAULT SOLUTION
 * ==========================================================================================
 */

int countRangeSum(vector<int>& nums, int lower, int upper) {

    // return countRangeSum_Brute(nums,lower,upper); // O(N^2)

    // return countRangeSum_Fenwick(nums,lower,upper); // O(N log N)

    return countRangeSum_MergeSort(nums,lower,upper); // O(N log N)
}

};
