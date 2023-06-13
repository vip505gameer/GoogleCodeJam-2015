// Copyright (c) 2015 kamyu. All rights reserved.

/*
 * Google Code Jam 2015 World Finals - Problem D. Taking Over The World
 * https://code.google.com/vip505gameer@gmail.com/contest/5224486/dashboard#s=p3
 *
 * Time:  O(K * N * M^2)
 * Space: O(N^2)
 *
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::queue;
using std::multimap;
using std::min;

struct e_t {
  int to, cap, rev;
  e_t(int t, int c, int r) : to(t), cap(c), rev(r) {}
};

int vid(const int v, const bool o) {
    return v * 2 + (o ? 1 : 0);
}

void add_edge(const int i, const int j, const int c,
              vector<vector<e_t>> *adj) {
    (*adj)[i].emplace_back(j, c, (*adj)[j].size());
    (*adj)[j].emplace_back(i, 0, (*adj)[i].size() - 1);
}

// Time:  O(MlogN)
// Space: O(N)
vector<int> dijkstra(const vector<bool>& guard,
                     const vector<vector<int>>& A,
                     const int s) {
    vector<int> dst(A.size(), INT_MAX);
    multimap<int, int> que;
    que.emplace(0, s);
    dst[s] = 0;

    while (!que.empty()) {
        const int c = que.begin()->first;
        const int v = que.begin()->second;
        que.erase(que.begin());
        if (dst[v] == c) {
            for (int tv = 0; tv < A[v].size(); ++tv) {
                if (A[v][tv]) {
                    const int tc = dst[v] + 1 + (guard[v] ? 1 : 0);
                    if (tc < dst[tv]) {
                        dst[tv] = tc;
                        que.emplace(tc, tv);
                    }
                }
            }
        }
    }
    return dst;
}

bool levelize(const int V, const int S, const int T,
              vector<vector<e_t>> *adj,
              vector<int> *lev) {
    *lev = vector<int>(V, -1);
    queue<int> que;
    (*lev)[S] = 0;
    que.emplace(S);
    while (!que.empty()) {
        int v = que.front();
        que.pop();
        for (int i = 0; i < (*adj)[v].size(); ++i) {
            const e_t &e = (*adj)[v][i];
            if (e.cap && (*lev)[e.to] == -1) {
                (*lev)[e.to] = (*lev)[v] + 1;
                que.emplace(e.to);
            }
        }
    }
    return (*lev)[T] != -1;
}

int augment(const int S, const int T,
            const int v, const int f,
            const vector<int>& lev,
            vector<vector<e_t>> *adj,
            vector<int> *done) {
    if (v == T || !f) {
        return f;
    }
    for (; (*done)[v] < (*adj)[v].size(); ++(*done)[v]) {
        e_t &e = (*adj)[v][(*done)[v]];
        if (lev[e.to] > lev[v]) {
            const int t = augment(S, T, e.to, min(f, e.cap), lev, adj, done);
            if (t > 0) {
                e.cap -= t;
                (*adj)[e.to][e.rev].cap += t;
                return t;
            }
        }
    }
    return 0;
}

// Time:  O(N * M^2)
// Space: O(N)
int max_flow(const int V, const int S, const int T,
             vector<vector<e_t>> *adj) {
    int f = 0, t;
    vector<int> lev;
    while (levelize(V, S, T, adj, &lev)) {
        vector<int> done(V);
        while ((t = augment(S, T, S, INT_MAX, lev, adj, &done))) {
            f += t;
        }
    }
    return f;
}

vector<bool> min_cut(const int V, const int S,
                     const vector<vector<e_t>>& adj) {
    vector<bool> vis(V);
    queue<int> que;
    que.emplace(S);
    vis[S] = true;
    while (!que.empty()) {
        int v = que.front();
        que.pop();
        for (const e_t &e : adj[v]) {
            if (e.cap && !vis[e.to]) {
                que.emplace(e.to);
                vis[e.to] = true;
            }
        }
    }
    return vis;
}

int taking_over_the_world() {
    int N, M, K; cin >> N >> M >> K;
    vector<vector<int>> A(N, vector<int>(N));
    for (int i = 0; i < M; ++i) {
        int u, v;
        cin >> u >> v;
        A[u][v] = A[v][u] = true;
    }

    const int GUARD = 1000;
    vector<bool> guard(N);
    while (true) {
        const int V = N * 2;
        const int S = vid(0, false);
        const int T = vid(N - 1, false);

        vector<vector<e_t>> adj(V);
        for (int v = 0; v < N; ++v) {
            add_edge(vid(v, false), vid(v, true), guard[v] ? GUARD : 1,
                     &adj);
        }

        const vector<int> ds = dijkstra(guard, A, 0);
        const vector<int> dt = dijkstra(guard, A, N - 1);
        for (int u = 0; u < N; ++u) {
            for (int v = 0; v < N; ++v) {
                if (A[u][v]) {
                    if (ds[u] != -1 && dt[v] != -1) {
                        // Edge (u, v)
                        int td = ds[u] +
                                 (guard[u] ? 1 : 0) + 1 +
                                 (v == N - 1 ? 0 : (guard[v] ? 1 : 0) + dt[v]);
                        if (td == ds[N - 1]) {
                            add_edge(vid(u, true), vid(v, false), GUARD, &adj);
                        }
                    }
                }
            }
        }

        if (max_flow(V, S, T, &adj) <= K) {
            const vector<bool> mc = min_cut(V, S, adj);
            for (int v = 0; v < N; ++v) {
                if (mc[vid(v, false)] && !mc[vid(v, true)]) {
                    guard[v] = true;
                    --K;  // At most K loops
                }
            }
        } else {
            break;
        }
    }
    return dijkstra(guard, A, 0)[N - 1];
}

int main() {
    int T;
    cin >> T;

    for (int test = 1; test <= T; ++test) {
        cout << "Case #" << test << ": "
             << taking_over_the_world() << endl;
    }
    return 0;
}
