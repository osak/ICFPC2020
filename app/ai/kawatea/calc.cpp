#include <cstdio>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include "calc.h"

using namespace std;

map<State, bool> checked;
map<State, int> dist;
map<State, vector<pair<State, pair<int, int>>>> candidate;
vector<unsigned long long> best;

bool valid(const State& s) {
    return abs(s.x) <= MAX_P && abs(s.y) <= MAX_P && abs(s.dx) <= MAX_D && abs(s.dy) <= MAX_D && (abs(s.x) > MAX_F || abs(s.y) > MAX_F);
}

State next(const State& s, int dx = 0, int dy = 0) {
    int nx = s.x, ny = s.y, ndx = s.dx + dx, ndy = s.dy + dy;
    if (abs(s.x) >= abs(s.y)) {
        if (s.x > 0) {
            ndx--;
        } else {
            ndx++;
        }
    }
    if (abs(s.x) <= abs(s.y)) {
        if (s.y > 0) {
            ndy--;
        } else {
            ndy++;
        }
    }
    nx += ndx;
    ny += ndy;
    return State(nx, ny, ndx, ndy);
}

bool check(const State& s) {
    if (!valid(s)) return false;
    if (checked.count(s)) return checked[s];
    
    checked[s] = true;
    State ss = next(s);
    
    return checked[s] = check(ss);
}

int main() {
    queue<State> q;
    
    for (int x = -MAX_P; x <= MAX_P; x++) {
        for (int y = -MAX_P; y <= MAX_P; y++) {
            for (int dx = -MAX_D; dx <= MAX_D; dx++) {
                for (int dy = -MAX_D; dy <= MAX_D; dy++) {
                    State s = State(x, y, dx, dy);
                    if (check(s)) {
                        dist[s] = 0;
                        q.push(s);
                        best.push_back(encode(s, 0, 0, 0));
                    } else {
                        for (int i = -1; i <= 1; i++) {
                            for (int j = -1; j <= 1; j++) {
                                State ss = next(s, i, j);
                                if (valid(ss)) candidate[ss].push_back(make_pair(s, make_pair(i, j)));
                            }
                        }
                    }
                }
            }
        }
    }
    
    while (!q.empty()) {
        State s = q.front();
        q.pop();
        vector<pair<State, pair<int, int>>>& v = candidate[s];
        
        for (int i = 0; i < v.size(); i++) {
            State ss = v[i].first;
            int dx = v[i].second.first;
            int dy = v[i].second.second;
            
            if (!dist.count(ss)) {
                dist[ss] = dist[s] + 1;
                q.push(ss);
                best.push_back(encode(ss, dx, dy, dist[ss]));
            }
        }
    }
    
    printf("%d\n", best.size());
    for (int i = 0; i < best.size(); i++) printf("%llu\n", best[i]);
    
    return 0;
}
