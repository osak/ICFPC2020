#pragma once

#include <algorithm>
#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"
#include <cmath>

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
            auto act = main_move(planet, field_rad, loc, vel, 256, 512);
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

public:
    JoinParams join_params() {
        return JoinParams();
    }
    StartParams start_params(const GameResponse& response) {
        int spec_point = response.game_info.ship_info.max_points;
        int reactor = 8;
        int armament = 0;
        int core = 50;
        int engine = spec_point - reactor * 12 - core*2;
        return StartParams(engine, armament, reactor, core);
    }
    CommandParams command_params(const GameResponse& response) {
        auto states = response.game_info.is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        auto enemy_states = response.game_info.is_defender ? response.game_state.attacker_states : response.game_state.defender_states;

        auto pos = states[0].pos;
        auto vel = states[0].velocity;

        int life = states[0].ship_parameter.life;
        int unit_id = states[0].id;
        int remaining_turn = response.game_info.max_turns - response.game_state.current_turn;

        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);

        Act next_act = main_move(
                response.game_info.field_info.planet_radius,
                response.game_info.field_info.field_radius,
                my_location,
                my_velocity,
                response.game_info.max_turns - response.game_state.current_turn, states[0].ship_parameter.energy, states[0].ship_parameter.life);
        auto next_move = next_act.move;
        cout << "Next move: " << next_move << endl;
        CommandParams params;
        if (next_move.x != 0 || next_move.y != 0) {
            params.commands.push_back(new Move(unit_id, next_move));
        }

        if (life > 1 && next_act.fission) {
            params.commands.push_back(new Fission(unit_id, StartParams(0, 0, 0, 1)));
        }

        return params;
    }
};
