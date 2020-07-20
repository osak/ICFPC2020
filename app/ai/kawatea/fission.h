#pragma once

#include <cstdio>
#include <set>
#include "calc.h"
#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

bool possible[MAX_P * 2 + 2][MAX_P * 2 + 2][MAX_D * 2 + 2][MAX_D * 2 + 2];
int direction[MAX_P * 2 + 2][MAX_P * 2 + 2][MAX_D * 2 + 2][MAX_D * 2 + 2][2];
int dist[MAX_P * 2 + 2][MAX_P * 2 + 2][MAX_D * 2 + 2][MAX_D * 2 + 2];

class FissionAI : public AI {
    public:
    JoinParams join_params() {
        load();
        return JoinParams();
    }
    
    StartParams start_params(const GameResponse& response) {
        int spec_point = response.game_info.ship_info.max_points;
        int reactor = 8;
        int armament = 0;
        int core = 70;
        int engine = spec_point - reactor * 12 - armament * 4 - core * 2;
        return StartParams(engine, armament, reactor, core);
    }
    
    CommandParams command_params(const GameResponse& response) {
        bool is_defender = response.game_info.is_defender;
        int main_ship_id = is_defender ? 0 : 1;
        vector<ShipState> ships = is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        set<State> positions, next_positions;
        CommandParams params;
        
        for (const ShipState& ship : ships) {
            if (ship.id == main_ship_id) continue;
            
            auto pos = ship.pos;
            auto vel = ship.velocity;
            int x = pos.first;
            int y = pos.second;
            int dx = vel.first;
            int dy = vel.second;
            State s = State(x, y, dx, dy);
            
            positions.insert(s);
            next_positions.insert(next(s));
        }
        
        for (const ShipState& ship : ships) {
            if (ship.id != main_ship_id) continue;
            
            auto pos = ship.pos;
            auto vel = ship.velocity;
            int x = pos.first;
            int y = pos.second;
            int dx = vel.first;
            int dy = vel.second;
            int life = ship.ship_parameter.life;
            int energy = ship.ship_parameter.energy;
            int turn = response.game_state.current_turn;
            bool prev_fissioned = fissioned;
            State s = State(x, y, dx, dy);
            
            fissioned = false;
            
            if (energy == 0) break;
            
            if (!is_static(x, y, dx, dy) || prev_fissioned || positions.count(s)) {
                int ndx = 0, ndy = 0, d = 1e9, e = 0;
                bool keep = false;
                
                if (!prev_fissioned && is_alive(x, y, dx, dy)) {
                    ndx = get_ndx(x, y, dx, dy);
                    ndy = get_ndy(x, y, dx, dy);
                    d = get_dist(x, y, dx, dy);
                    e = 1;
                    keep = next_positions.count(next(s, ndx, ndy));
                }
                
                for (int i = -2; i <= 2; i++) {
                    for (int j = -2; j <= 2; j++) {
                        if (i == 0 && j == 0) continue;
                        if (turn > 30 && (abs(i) == 2 || abs(j) == 2)) continue;
                        State ss = next(s, i, j);
                        if (!next_positions.count(ss) && is_alive(ss.x, ss.y, ss.dx, ss.dy)) {
                            int nd = get_dist(ss.x, ss.y, ss.dx, ss.dy);
                            int ne = max(abs(i), abs(j));
                            if (keep || nd < d || ne > e) {
                                ndx = -i;
                                ndy = -j;
                                d = nd;
                                e = ne;
                                keep = false;
                            }
                        }
                    }
                }
                
                if (ndx != 0 || ndy != 0) {
                    params.commands.push_back(new Move(ship.id, Vector(ndx, ndy)));
                }
            } else if (life > 1) {
                params.commands.push_back(new Fission(ship.id, StartParams(0, 0, 0, 1)));
                fissioned = true;
            }
        }
        return params;
    }
    
    private:
    bool fissioned = false;
    
    void load() {
        FILE* fp = fopen("ai/kawatea/pre.txt", "r");
        if (fp == NULL) {
            fprintf(stderr, "failed to open pre.txt\n");
            fflush(stderr);
            return;
        }
        int num;
        fscanf(fp, "%d", &num);
        fprintf(stderr, "loading %d records\n", num);
        fflush(stderr);
        for (int i = 0; i < num; i++) {
            unsigned long long value;
            int x, y, dx, dy, ndx, ndy, d;
            fscanf(fp, "%llu", &value);
            decode(value, d, x, y, dx, dy, ndx, ndy);
            possible[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D] = true;
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][0] = ndx;
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][1] = ndy;
            dist[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D] = d;
        }
        fprintf(stderr, "loaded %d records\n", num);
        fflush(stderr);
        fclose(fp);
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
    
    bool is_alive(int x, int y, int dx, int dy) {
        if (abs(x) > MAX_P || abs(y) > MAX_P || abs(dx) > MAX_D || abs(dy) > MAX_D) return false;
        return possible[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D];
    }
    
    bool is_static(int x, int y, int dx, int dy) {
        return is_alive(x, y, dx, dy) && get_dist(x, y, dx, dy) == 0;
    }
    
    int get_ndx(int x, int y, int dx, int dy) {
        return -direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][0];
    }
    
    int get_ndy(int x, int y, int dx, int dy) {
        return -direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][1];
    }
    
    int get_dist(int x, int y, int dx, int dy) {
        return dist[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D];
    }
};