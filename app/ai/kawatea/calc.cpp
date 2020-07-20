#include <cstdio>
#include <vector>
#include <queue>
#include <map>
#include <algorithm>
#include "calc.h"

using namespace std;

class printer {
    public:
    printer(const string& name);
    ~printer();
    inline void write_char(char c);
    
    private:
    constexpr static int SIZE = 1 << 18;
    FILE* fp;
    char buf[SIZE];
    char* now = buf;
    const char* end = buf + SIZE;
    inline void flush(int need);
};

printer::printer(const string& name) {
    fp = fopen(name.c_str(), "wb");
}

printer::~printer() {
    flush(SIZE);
    fclose(fp);
}

inline void printer::write_char(char c) {
    flush(1);
    *now++ = c;
}

inline void printer::flush(int need) {
    if (now + need <= end) return;
    fwrite(buf, sizeof(char), now - buf, fp);
    now = buf;
}

map<State, bool> checked;
map<State, int> dist;
map<State, vector<pair<State, pair<int, int>>>> candidate;

bool valid(const State& s) {
    return abs(s.x) <= MAX_P && abs(s.y) <= MAX_P && abs(s.dx) <= MAX_D && abs(s.dy) <= MAX_D && (abs(s.x) > MAX_F || abs(s.y) > MAX_F);
}

bool check(const State& s) {
    if (!valid(s)) return false;
    if (checked.count(s)) return checked[s];
    
    checked[s] = true;
    State ss = next(s);
    
    return checked[s] = check(ss);
}

int main() {
    int num = 0;
    printer pr("pre2.txt");
    queue<State> q;
    
    for (int x = -MAX_P; x <= MAX_P; x++) {
        for (int y = -MAX_P; y <= MAX_P; y++) {
            for (int dx = -MAX_D; dx <= MAX_D; dx++) {
                for (int dy = -MAX_D; dy <= MAX_D; dy++) {
                    State s = State(x, y, dx, dy);
                    if (check(s)) {
                        dist[s] = 0;
                        q.push(s);
                        num++;
                        pr.write_char(s.x);
                        pr.write_char(s.y);
                        pr.write_char(s.dx);
                        pr.write_char(s.dy);
                        pr.write_char(0);
                        pr.write_char(0);
                        pr.write_char(0);
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
                num++;
                pr.write_char(ss.x);
                pr.write_char(ss.y);
                pr.write_char(ss.dx);
                pr.write_char(ss.dy);
                pr.write_char(dx);
                pr.write_char(dy);
                pr.write_char(dist[ss]);
            }
        }
    }
    
    printf("%d\n", num);
    
    return 0;
}
