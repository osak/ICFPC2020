#pragma once

#include <algorithm>
#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"
#include <cmath>


//--- kamikaze info
const int kamikaze_power_array_size = 5;
int kamikaze_powers[] = {0, 128, 161, 181, 195};

const int kamikaze_size = 3;

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

pair<Vector, Vector> simulate(Vector loc, Vector vel, Vector thruster) {
    vel = vel + thruster;
    if (abs(loc.x) >= abs(loc.y)) {
        if (loc.x < 0) ++vel.x;
        if (loc.x > 0) --vel.x;
    }

    if (abs(loc.x) <= abs(loc.y)) {
        if (loc.y < 0) ++vel.y;
        if (loc.y > 0) --vel.y;
    }

    loc.x += vel.x;
    loc.y += vel.y;

    return make_pair(loc, vel);
};

int next_distance(const ShipState &p, const ShipState &q) {
    auto nps = simulate(Vector(p.pos.first, p.pos.second), Vector(p.velocity.first, p.velocity.second), Vector(0, 0)).first;
    auto nqs = simulate(Vector(q.pos.first, q.pos.second), Vector(q.velocity.first, q.velocity.second), Vector(0, 0)).first;

    return max(abs(nps.x - nqs.x), abs(nps.y - nqs.y));
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

class MeteorAI : public AI {

    bool dead(const Vector &loc, int center_rad, int field_rad) {
        if (abs(loc.x) <= center_rad && abs(loc.y) <= center_rad) return true;
        return abs(loc.x) >= field_rad || abs(loc.y) >= field_rad;
    }

    pair<Vector, Vector> simulate(Vector loc, Vector vel, Vector thruster) {
        vel = vel + thruster;
        if (abs(loc.x) >= abs(loc.y)) {
            if (loc.x < 0) ++vel.x;
            if (loc.x > 0) --vel.x;
        }

        if (abs(loc.x) <= abs(loc.y)) {
            if (loc.y < 0) ++vel.y;
            if (loc.y > 0) --vel.y;
        }

        loc.x += vel.x;
        loc.y += vel.y;

        return make_pair(loc, vel);
    };

    int test(Vector loc, Vector vel, int center_rad, long long field_rad, int max_turn, bool show_trail = false) {
        int cnt = 0;

        while (cnt < max_turn) {
            auto p = simulate(loc, vel, Vector(0, 0));
            loc = p.first;
            vel = p.second;

            if (show_trail) {
                cerr << loc << endl;
            }

            if (dead(loc, center_rad, field_rad)) break;
            ++cnt;
        }

        return cnt;
    }

    bool danger(const Vector &loc, const Vector &vel, const Vector &eloc, const Vector &evel, int dist = 0) {
        auto a = simulate(loc, vel, Vector(0, 0)).first;
        auto b = simulate(eloc, evel, Vector(0, 0)).first;

        return abs(a.x - b.x) < dist || abs(a.y - b.y) < dist || abs(abs(a.x - b.x) == abs(a.y - b.y)) < dist;
    }

    pair<int, Vector> dfs(int remaining_depth, const Vector &loc, const Vector &vel, const vector<Vector> &vec, int center_rad, long long field_rad, int max_turn, int fuel) {
        if (remaining_depth-- <= 0) return make_pair(-1, Vector(0, 0));

        int best = -1;
        Vector best_move = Vector(0, 0);

        for (int i = 0; i < vec.size(); ++i) {
            const auto &v = vec[i];

            auto new_vel = vel + v;
            auto next_param = simulate(loc, vel, v);
            if (dead(next_param.first, center_rad, field_rad)) continue;

            int result = test(loc, next_param.second, center_rad, field_rad, max_turn);
            if (best < result) {
                best = result;
                best_move = v;
            }

            auto dfs_result = dfs(remaining_depth, next_param.first, next_param.second, vec, center_rad, field_rad, max_turn - 1,
                                  static_cast<int>(fuel - max(abs(v.x), abs(v.y))));
            if (best < dfs_result.first) {
                best = dfs_result.first;
                best_move = v;
            }
        }

        return make_pair(best, best_move);
    }

    static vector<vector<Vector>> generate_vector() {
        vector<vector<Vector>> vecs;
        for (int dx: {-1, 1}) {
            for (int dy: {-1, 1}) {
                vector<Vector> vec;
                for (int i = 0; i <= 2; ++i) {
                    for (int j = 0; j <= 2; ++j) {
                        vec.emplace_back(i * dx, j * dy);
                    }
                }
                vecs.push_back(vec);
            }
        }

        return vecs;
    }

    Vector safe_move(int planet_size, int field_size, const Vector &loc, const Vector &vel, int remaining_turn, bool danger, int fuel) {
        static auto vecs = generate_vector();

        int best = -1;
        Vector best_move = Vector(0, 0);
        for (const auto &vec: vecs) {
            auto result = dfs(3, loc, vel, vec, planet_size, field_size, remaining_turn, fuel);

            auto simulated = simulate(loc, vel, result.second);
            if (simulated.first.x == loc.x && simulated.first.y == loc.y
                && simulated.second.x == vel.x && simulated.second.y == vel.y) continue;

            if (best < result.first) {
                best = result.first;
                best_move = result.second;
            }
        }

        return Vector(-best_move.x, -best_move.y);
    }

    Vector bomb_move(int planet_size, int field_size, const Vector &loc, const Vector &vel, int remaining_turn, int fuel) {
        return Vector(0, 0);
    }

    struct Act {
        Vector move;
        bool fission;
    };

    Act main_move(int planet_size, int field_size, const Vector &loc, const Vector &vel, int remaining_turn, int fuel, int core) {
        if (core > 1) {
            for (int dx : {-2, 0, 2}) {
                for (int dy : {-2, 0, 2}) {
                    if (dx == 0 && dy == 0) continue;
                    if (fuel < max(abs(dx), abs(dy))) continue;
                    auto next_state = simulate(loc, vel, Vector(dx, dy));
                    cerr << test(next_state.first, next_state.second, planet_size, field_size, remaining_turn) << endl;
                    if (remaining_turn <= test(next_state.first, next_state.second, planet_size, field_size, remaining_turn)) {
                        return {Vector(-dx, -dy), true};
                    }
                }
            }
        }

        return {safe_move(planet_size, field_size, loc, vel, remaining_turn, false, fuel), false};
    }

public:
    void test_main_move(int x, int y) {
        Vector loc(x, y), vel(0, 0);

        int planet = 16;
        int field_rad = 128;

        if (abs(loc.x) <= planet && abs(loc.y) <= planet) return;
        if (abs(loc.x) >= field_rad || abs(loc.y) >= field_rad) return;

        int cost = 0;
        int cnt = 0;
        while(cnt < 256) {
            // simulate
            auto act = main_move(planet, field_rad, loc, vel, 256, 512, 10000);
            auto vec = act.move;

            cost += max(abs(vec.x), vec.y);
            auto new_state = simulate(loc, vel, Vector(-vec.x, -vec.y));
            loc = new_state.first;
            vel = new_state.second;

            if (dead(loc, planet, field_rad)) break;

            ++cnt;
            if (act.fission) {
                cout << "fision at " << loc << endl;
            }

            cerr << "current loc:" << loc << " current vel" << vel << endl;
        }

        if (cnt > 256 && cost < 30) {
            cerr << "cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, planet, field_rad, 256, false) << endl;
        } else {
            if (cnt < 250) {
                cout << "dead at turn cnt: " << cnt << " cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, planet, field_rad, 256, false) << endl;
            } else {
                cout << "cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, planet, field_rad, 256, false) << endl;
            }
        }
    }

    Vector initial_move(Vector loc) {
        Vector vel = Vector(0, 0);
        if (abs(loc.x) >= abs(loc.y)) {
            if (loc.x < 0) vel.x = 2;
            if (loc.x > 0) vel.x = -2;
        } else {
            vel.x = 2;
        }

        if (abs(loc.x) <= abs(loc.y)) {
            if (loc.y < 0) vel.y = 2;
            if (loc.y > 0) vel.y = -2;
        } else {
            vel.y = -2;
        }

        return Vector(-vel.x, -vel.y);
    }

    Act main_move(const ShipState &state, const GameInfo &game_info, const GameState &game_state) {
        auto pos = state.pos;
        auto vel = state.velocity;

        int life = state.ship_parameter.life;
        int unit_id = state.id;
        int remaining_turn = game_info.max_turns - game_state.current_turn;

        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);

        return main_move(
                game_info.field_info.planet_radius,
                game_info.field_info.field_radius,
                my_location,
                my_velocity,
                remaining_turn, state.ship_parameter.energy, state.ship_parameter.life);
    }

    int initial_core = 50;
    void add_fission(const ShipState &state, CommandParams &params) {
        if (state.ship_parameter.life > 1) {
            if (state.ship_parameter.life >= initial_core) {
                params.commands.push_back(new Fission(state.id, StartParams(state.ship_parameter.energy / 2, 0, 0, state.ship_parameter.life / 2)));
            }

            if (state.ship_parameter.energy >= kamikaze_size) {
                params.commands.push_back(new Fission(state.id, StartParams(kamikaze_size, 0, 0, 1)));
            }
        }
    }
public:
    JoinParams join_params() {
        return JoinParams();
    }
    StartParams start_params(const GameResponse& response) {
        int spec_point = response.game_info.ship_info.max_points;
        int reactor = 8;
        int armament = 0;
        int initial_core = 50;
        int engine = spec_point - reactor * 12 - initial_core*2;
        return StartParams(engine, armament, reactor, initial_core);
    }
    CommandParams command_params(const GameResponse& response) {
        auto states = response.game_info.is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        auto enemy_states = !response.game_info.is_defender ? response.game_state.defender_states : response.game_state.attacker_states;

        Act next_act = main_move(states[0], response.game_info, response.game_state);
        auto next_move = next_act.move;

        cout << "Next move: " << next_move << endl;
        if (states[1].ship_parameter.life != initial_core && response.game_state.current_turn < 5
            && abs(states[1].pos.first) - 30 < response.game_info.field_info.field_radius
            && abs(states[1].pos.second) - 30 < response.game_info.field_info.field_radius) {
            next_move = initial_move(Vector(states[1].pos.first, states[1].pos.second));
        }

        CommandParams params;
        if (next_move.x != 0 || next_move.y != 0) {
            params.commands.push_back(new Move(states[0].id, next_move));
        }

        if (next_act.fission) {
            add_fission(states[0], params);
        }

        // ship 2
        if (states.size() > 1) {
            auto state = states[1];
            Act next_act2 = main_move(state, response.game_info, response.game_state);
            auto next_move2 = next_act.move;

            if (next_move2.x != 0 || next_move2.y != 0) {
                params.commands.push_back(new Move(state.id, next_move2));
            }

            if (next_act2.fission) {
                add_fission(states[1], params);
            }
        }


        for (int i = 2; i < states.size(); ++i) {
            const auto state = states[i];
            if (check_kamikaze(state, states, enemy_states, -1)) {
                params.commands.push_back(new Kamikaze(state.id));
            }

        }


        return params;
    }
};
