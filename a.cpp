#include <bits/stdc++.h>
using namespace std;

#pragma GCC optimize("Ofast")
#pragma GCC target("avx,avx2,fma")
#pragma GCC optimize("unroll-loops")

#define int long long
#define pb push_back
#define eb emplace_back
#define all(v) v.begin(), v.end()
#define rall(v) v.rbegin(), v.rend()
#define fi first
#define se second
#define pii pair<int,int>
#define vi vector<int>
#define vvi vector<vi>
#define YES cout << "YES\n"
#define NO cout << "NO\n"
#define fastio ios_base::sync_with_stdio(false); cin.tie(nullptr);
#define FOR(i,a,b) for(int i = (a); i < (b); ++i)
#define ROF(i,a,b) for(int i = (a); i >= (b); --i)
#define REP(i,n) FOR(i,0,n)
#define EACH(x,a) for (auto& x : a)
#define dbg(x) cerr << #x << " = " << (x) << "\n"
#define dbg2(x,y) cerr << #x << " = " << (x) << ", " << #y << " = " << (y) << "\n"
#define dbg3(x,y,z) cerr << #x << " = " << (x) << ", " << #y << " = " << (y) << ", " << #z << " = " << (z) << "\n"
#define dbgp(p) cerr << #p << " = {" << (p).fi << ", " << (p).se << "}\n"
#define dbgv(v) cerr << #v << " = ["; for (auto i : v) cerr << i << " "; cerr << "]\n"
#define dbgmat(mat) cerr << #mat << " = \n"; for (auto& row : mat) { for (auto x : row) cerr << x << " "; cerr << "\n"; }
#define endl '\n'
const int INF = 1e18;
const int MOD = 1e9 + 7;
const int MOD2 = 998244353;
const double EPS = 1e-9;
const double PI = acos(-1);
