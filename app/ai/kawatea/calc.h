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

unsigned long long encode(const State& s, int dx, int dy, long long dist) {
    return (unsigned long long)(dist << 32) | ((unsigned long long)(s.x + MAX_P) << 24) | ((unsigned long long)(s.y + MAX_P) << 16) | ((unsigned long long)(s.dx + MAX_D) << 10) | ((unsigned long long)(s.dy + MAX_D) << 4) | ((unsigned long long)(dx + 1) << 2) | (unsigned long long)(dy + 1);
}

void decode(unsigned long long value, int& dist, int& x, int &y, int& dx, int& dy, int& ndx, int& ndy) {
    dist = (value >> 32);
    value = (value << 32) >> 32;
    x = (value >> 24) - MAX_P;
    value = (value << 40) >> 40;
    y = (value >> 16) - MAX_P;
    value = (value << 48) >> 48;
    dx = (value >> 10) - MAX_D;
    value = (value << 54) >> 54;
    dy = (value >> 4) - MAX_D;
    value = (value << 60) >> 60;
    ndx = (value >> 2) - 1;
    value = (value << 62) >> 62;
    ndy = value - 1;
}
