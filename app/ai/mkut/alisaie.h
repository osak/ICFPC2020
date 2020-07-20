#pragma once

#include <cstdio>
#include <set>
#include "../kawatea/calc.h"
#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

bool possible[MAX_P * 2 + 2][MAX_P * 2 + 2][MAX_D * 2 + 2][MAX_D * 2 + 2];
int direction[MAX_P * 2 + 2][MAX_P * 2 + 2][MAX_D * 2 + 2][MAX_D * 2 + 2][2];
int dist[MAX_P * 2 + 2][MAX_P * 2 + 2][MAX_D * 2 + 2][MAX_D * 2 + 2];

class AlisaieAI : public AI {
    public:
    JoinParams join_params() {
        load();
        return JoinParams();
    }
    StartParams start_params(const GameResponse& response) {
        int spec_point = response.game_info.ship_info.max_points;
        int reactor = 16;
        int armament = max(spec_point - 256, 0) / 64 * 16;
        int core = 8;
        int engine = spec_point - core * 2 - reactor * 12 - armament * 4;
        return StartParams(engine, armament, reactor, core);
    }
    
    CommandParams command_params(const GameResponse& response) {
        bool is_defender = response.game_info.is_defender;
        int main_ship_id = is_defender ? 0 : 1;
        const vector<ShipState>& ships = is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        const vector<ShipState>& enemy_ships = !is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        map<State, int> next_positions;
        CommandParams params;
        
        for (const ShipState& ship : ships) {
            auto pos = ship.pos;
            auto vel = ship.velocity;
            int x = pos.first;
            int y = pos.second;
            int dx = vel.first;
            int dy = vel.second;
            State s = State(x, y, dx, dy);
            
            // cerr << next(s).x << "," << next(s).y << "," << next(s).dx << "," << next(s).dy << "," << endl;
            next_positions[next(s)]++;
        }
        
        for (const ShipState& ship : ships) {
            cerr << "ship" << ship.id << endl;
            personal_commands(ship, enemy_ships, response.game_state, next_positions, params);
        }
        return params;
    }
    
    private:

    void personal_commands(const ShipState& ship, const vector<ShipState>& enemy_ships, const GameState game_state, map<State, int>& next_positions, CommandParams& params) {
        auto pos = ship.pos;
        auto vel = ship.velocity;
        int x = pos.first;
        int y = pos.second;
        int dx = vel.first;
        int dy = vel.second;
        int life = ship.ship_parameter.life;
        int energy = ship.ship_parameter.energy;
        int attack = ship.ship_parameter.attack;
        int recharge_rate = ship.ship_parameter.recharge_rate;
        int heat = ship.heat;
        int turn = game_state.current_turn;
        State s = State(x, y, dx, dy);
        State ns = next(s);
        
        // move
        if (!is_static(x, y, dx, dy) || next_positions[ns] > 1) {
            int ndx = 0, ndy = 0, d = 1e9, e = 0;
            bool keep = false;
            if (is_alive(x, y, dx, dy)) {
                ndx = -get_ndx(x, y, dx, dy);
                ndy = -get_ndy(x, y, dx, dy);
                d = get_dist(x, y, dx, dy);
                e = 1;
                keep = next_positions[next(s, ndx, ndy)] > 1;
            }
            
            for (int i = -2; i <= 2; i++) {
                for (int j = -2; j <= 2; j++) {
                    if (i == 0 && j == 0) continue;
                    if (turn > 30 && (abs(i) == 2 || abs(j) == 2)) continue;
                    State ss = next(s, i, j);
                    if (next_positions[ss] == 0 && is_alive(ss.x, ss.y, ss.dx, ss.dy)) {
                        int nd = get_dist(ss.x, ss.y, ss.dx, ss.dy);
                        int ne = max(abs(i), abs(j));
                        if (keep || nd < d || (nd == d && ne > e)) {
                            ndx = i;
                            ndy = j;
                            d = nd;
                            e = ne;
                            keep = false;
                        }
                    }
                }
            }
            
            if (ndx != 0 || ndy != 0) {
                next_positions[ns]--;
                next_positions[next(s, ndx, ndy)]++;
                params.commands.push_back(new Move(ship.id, Vector(-ndx, -ndy)));
            }
            cerr << "move" << endl;
            return;
        }
        
        // critical shot
        State next_s = next(s);
        for (int move = 0; move <= 2; move++) {
            for (int dx = -move; dx <= move; dx++) {
                for (int dy = -move; dy <= move; dy++) {
                    if (max(abs(dx), abs(dy)) != move) continue;
                    if (!is_alive(ship, dx, dy)) continue;

                    for (const ShipState& enemy : enemy_ships) {
                        State enemy_state(enemy.pos.first, enemy.pos.second, enemy.velocity.first, enemy.velocity.second);
                        State next_enemy_state = next(enemy_state);
                        if (critical_point(next_s, next_enemy_state)) {
                            int power = get_full_power(ship, move);
                            params.commands.push_back(new Attack(ship.id, Vector(next_enemy_state.x, next_enemy_state.y), power));
                            
                            cerr << "critical shot" << endl;
                            return;
                        }
                    }
                }
            }
        }

        // fission
        if (life > 1) {
            StartParams clone_params(energy / 2, attack / 2, recharge_rate / 2, life / 2);
            params.commands.push_back(new Fission(ship.id, clone_params));
            cerr << "fission" << endl;
        }
    }

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

    bool is_alive(const ShipState& ship, int dx, int dy) {
        State s(ship.pos.first, ship.pos.second, ship.velocity.first, ship.velocity.second);
        State next_s = next(s, dx, dy);
        int next_energy = ship.ship_parameter.energy - max(abs(dx), abs(dy));
        return next_energy >= get_dist(next_s.x, next_s.y, next_s.dx, next_s.dy);
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

    bool critical_point(const State& a, const State& b) {
        if (a.x == b.x) return true;
        if (a.y == b.y) return true;
        if (abs(a.x - b.x) == abs(a.y - b.y)) return true;
        return false;
    }

    int get_full_power(const ShipState& my_ship_state, int move) {
        int remaining_heat = max(0, my_ship_state.max_heat - my_ship_state.heat + my_ship_state.ship_parameter.recharge_rate - move * 8);
        return min(remaining_heat, my_ship_state.ship_parameter.attack);
    }
};