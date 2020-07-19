#pragma once

const int MAX_P = 127;
const int MAX_F = 16;
const int MAX_D = 5;

struct State {
    int x;
    int y;
    int dx;
    int dy;
    
    State() {}
    State(int x, int y, int dx, int dy) : x(x), y(y), dx(dx), dy(dy) {}
    
    bool operator<(const State& s) const {
        if (x != s.x) return x < s.x;
        if (y != s.y) return y < s.y;
        if (dx != s.dx) return dx < s.dx;
        return dy < s.dy;
    }
};

unsigned encode(const State& s, int dx, int dy) {
    return ((s.x + MAX_P) << 24) | ((s.y + MAX_P) << 16) | ((s.dx + MAX_D) << 10) | ((s.dy + MAX_D) << 4) | ((dx + 1) << 2) | (dy + 1);
}

void decode(unsigned value, int& x, int &y, int& dx, int& dy, int& ndx, int& ndy) {
    x = (value >> 24) - MAX_P;
    value = (value << 8) >> 8;
    y = (value >> 16) - MAX_P;
    value = (value << 16) >> 16;
    dx = (value >> 10) - MAX_D;
    value = (value << 22) >> 22;
    dy = (value >> 4) - MAX_D;
    value = (value << 28) >> 28;
    ndx = (value >> 2) - 1;
    value = (value << 30) >> 30;
    ndy = value - 1;
}
