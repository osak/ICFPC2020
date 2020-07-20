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

class scanner {
    public:
    scanner(const string& name);
    ~scanner();
    inline bool load();
    inline char read_char();
    
    private:
    constexpr static int SIZE = 1 << 18;
    FILE* fp;
    char buf[SIZE + 1];
    int now = 0;
    int end = 0;
};

scanner::scanner(const string& name) {
    fp = fopen(name.c_str(), "rb");
}

scanner::~scanner() {
    fclose(fp);
}

inline bool scanner::load() {
    if (now != end) return true;
    now = 0;
    end = fread(buf, sizeof(char), SIZE, fp);
    if (end == 0) return false;
    return true;
}

inline char scanner::read_char() {
    if (!load()) {
        fprintf(stderr, "failed to read\n");
        exit(0);
    }
    return buf[now++];
}

//--- kamikaze info
const int kamikaze_power_array_size = 5;
int kamikaze_powers[] = {0, 128, 161, 181, 195};

int calc_ship_sum(const ShipState& ship) {
    return ship.ship_parameter.attack + ship.ship_parameter.energy + ship.ship_parameter.life + ship.ship_parameter.recharge_rate;
}

int get_kamikaze_power(const ShipState& ship) {
    const int sum = calc_ship_sum(ship);
    if (sum < kamikaze_power_array_size) {
        return kamikaze_powers[sum];
    } else {
        return kamikaze_powers[kamikaze_power_array_size - 1];
    }
}

class AlisaieAI : public AI {

    int next_distance(const ShipState &p, const ShipState &q) {
        State ps(p.pos.first, p.pos.second, p.velocity.first, p.velocity.second);
        State nps = next(ps);
        State qs(q.pos.first, q.pos.second, q.velocity.first, q.velocity.second);
        State nqs = next(qs);

        return max(abs(nps.x - nqs.x), abs(nps.y - nqs.y));
    }

    int next_distance(const ShipState &p, int x, int y) {
        State ps(p.pos.first, p.pos.second, p.velocity.first, p.velocity.second);
        State nps = next(ps);

        return abs(nps.x - x) + abs(nps.y - y);
    }

    bool check_kamikaze(const ShipState& my_ship, const vector<ShipState>& my_ships, const vector<ShipState>& enemy_ships, const int main_ship_id) {
        const int kamikaze_power = get_kamikaze_power(my_ship);

        int our_loss = calc_ship_sum(my_ship);
        int our_total_sum = calc_ship_sum(my_ship);
        for (const ShipState& our_ship : my_ships) {
            if (my_ship.id == our_ship.id) {
                continue;
            }
            const int distance = next_distance(my_ship, our_ship);
            const int damage = max(0, kamikaze_power - 32 * distance);
            const int sum = calc_ship_sum(our_ship);
            our_total_sum += sum;
            if (damage > 0 && our_ship.id == main_ship_id) {
                return false;
            }
            const int value = min(damage, sum);
            our_loss += value;
        }

        int enemy_loss = 0;
        int enemy_total_sum = 0;
        for (const ShipState& enemy_ship : enemy_ships) {
            const int distance = next_distance(my_ship, enemy_ship);
            const int damage = max(0, kamikaze_power - 32 * distance);
            const int sum = calc_ship_sum(enemy_ship);
            const int value = min(damage, sum);
            enemy_loss += value;
            enemy_total_sum += sum;
        }
        return our_loss * enemy_total_sum <= enemy_loss * our_total_sum;
    }

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
        map<Vector, int> kamikaze_area;
        CommandParams params;
        
        for (const ShipState& ship : ships) {
            auto pos = ship.pos;
            auto vel = ship.velocity;
            int x = pos.first;
            int y = pos.second;
            int dx = vel.first;
            int dy = vel.second;
            State s = State(x, y, dx, dy);

            // kamikaze
            if (check_kamikaze(ship, ships, enemy_ships, -1)) {
                params.commands.push_back(new Kamikaze(ship.id));
                const int kamikaze_power = get_kamikaze_power(ship);
                const int kamikaze_radius = kamikaze_power/32;
                for (int x = -kamikaze_radius; x <= kamikaze_radius; x++) {
                    for (int y = -kamikaze_radius; y <= kamikaze_radius; x++) {
                        const int distance = next_distance(ship, x, y);
                        const int damage = max(0, kamikaze_power - 32 * distance);
                        kamikaze_area[Vector(x, y)] += damage;
                    }
                }
            } else {
                next_positions[next(s)]++;
            }
        }
        
        for (const ShipState& ship : ships) {
            cerr << "ship" << ship.id << endl;
            personal_commands(ship, enemy_ships, response.game_info, response.game_state, next_positions, kamikaze_area, params);
        }
        return params;
    }
    
    private:

    void personal_commands(const ShipState& ship, const vector<ShipState>& enemy_ships, const GameInfo game_info, const GameState game_state, map<State, int>& next_positions, map<Vector, int>& kamikaze_area, CommandParams& params) {
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
        int max_heat = ship.max_heat;
        int heat = ship.heat;
        int turn = game_state.current_turn;
        int rem_turn = game_info.max_turns - turn;
        State s = State(x, y, dx, dy);
        State ns = next(s);

        cerr << "A" << endl;
        
        // move
        if (!is_static(x, y, dx, dy) || next_positions[ns] > 1 || kamikaze_area[Vector(ns.x, ns.y)] > 0) {
            int ndx = 0, ndy = 0, d = 1e9, e = 0, dmg = 0;
            bool keep = false;
            if (is_alive(x, y, dx, dy)) {
                ndx = -get_ndx(x, y, dx, dy);
                ndy = -get_ndy(x, y, dx, dy);
                d = get_dist(x, y, dx, dy);
                e = 1;
                dmg = kamikaze_area[Vector(ns.x, ns.y)];
                keep = next_positions[next(s, ndx, ndy)] > 1;
            }
            cerr << 1 << endl;
            for (int i = -2; i <= 2; i++) {
                for (int j = -2; j <= 2; j++) {
                    if (i == 0 && j == 0) continue;
                    if (turn > 30 && (abs(i) == 2 || abs(j) == 2)) continue;
                    State ss = next(s, i, j);
                    if (next_positions[ss] == 0 && is_alive(ss.x, ss.y, ss.dx, ss.dy)) {
                        int nd = get_dist(ss.x, ss.y, ss.dx, ss.dy);
                        int ne = max(abs(i), abs(j));
                        int ndmg = kamikaze_area[Vector(ss.x, ss.y)];
                        if (keep || nd < d || (nd == d && ndmg < dmg) || (nd == d && ndmg == dmg && ne > e)) {
                            ndx = i;
                            ndy = j;
                            d = nd;
                            e = ne;
                            dmg = ndmg;
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

        cerr << "B" << endl;
        
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
                            if (power > 0) {
                                params.commands.push_back(new Attack(ship.id, Vector(next_enemy_state.x, next_enemy_state.y), power));
                            
                                cerr << "critical shot" << endl;
                                return;
                            }
                        }
                    }
                }
            }
        }

        cerr << "C" << endl;

        // fission
        if (life > 1) {
            StartParams clone_params(energy / 2, attack / 2, recharge_rate / 2, life / 2);
            params.commands.push_back(new Fission(ship.id, clone_params));
            cerr << "fission" << endl;
            return;
        }

        cerr << "D" << endl;

        // random move
        if (energy > rem_turn / 3 && max_heat - heat + recharge_rate >= 8) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (max(abs(dx), abs(dy)) == 0) continue;
                    if (!is_alive(ship, dx, dy)) continue;
                    
                    params.commands.push_back(new Move(ship.id, Vector(dx, dy)));
                    return;
                }
            }
        }

        cerr << "E" << endl;
        
        // free shot
        {
            int power = get_free_power(ship, 0);
            if (power > 0) {
                int d = 1000;
                auto target = enemy_ships[0];
                for (auto enemy : enemy_ships) {
                    int cd = next_distance(ship, enemy);
                    if (d > cd) {
                        d = cd;
                        target = enemy;
                    }
                }
                State t = next(State(target.pos.first, target.pos.second, target.velocity.first, target.velocity.second));
                params.commands.push_back(new Attack(ship.id, Vector(t.x, t.y), power));
                return;
            }
        }
    }

    void load() {
        scanner sc("ai/kawatea/pre2.txt");
        int num = 0;
        fprintf(stderr, "loading records\n");
        fflush(stderr);
        while (sc.load()) {
            int x, y, dx, dy, ndx, ndy, d;
            x = sc.read_char();
            y = sc.read_char();
            dx = sc.read_char();
            dy = sc.read_char();
            ndx = sc.read_char();
            ndy = sc.read_char();
            d = sc.read_char();
            possible[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D] = true;
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][0] = ndx;
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][1] = ndy;
            dist[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D] = d;
            num++;
        }
        fprintf(stderr, "loaded %d records\n", num);
        fflush(stderr);
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

    int get_free_power(const ShipState& my_ship_state, int move) {
        return max(0, my_ship_state.ship_parameter.recharge_rate - my_ship_state.heat - move * 8);
    }
};