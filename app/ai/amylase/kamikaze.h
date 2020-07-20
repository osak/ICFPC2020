#pragma once

#include <algorithm>
#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

class KamikazeAI : public AI {
    const int kamikaze_rad = 3;

    int consecutiveZeroVelocities;
public:
    KamikazeAI() {
        consecutiveZeroVelocities = 0;
    }
private:

    bool is_enemy_stopping() {
        return consecutiveZeroVelocities >= 10;
    }

    bool dead(const Vector &loc, int center_rad, int field_rad) {
        if (abs(loc.x) <= center_rad && abs(loc.y) <= center_rad) return true;
        return abs(loc.x) >= field_rad || abs(loc.y) >= field_rad;
    }

    pair<Vector, Vector> simulate(Vector loc, Vector vel) {
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

    int test(Vector loc, Vector vel, Vector eloc, Vector evel, int center_rad, long long field_rad, int max_turn, int offset, bool show_trail = false) {
        int cnt = offset;
        const int trace_turn = 20;

        while (cnt < max_turn) {
            auto p = simulate(loc, vel);
            loc = p.first;
            vel = p.second;
            if (not is_enemy_stopping()) {
                auto q = simulate(eloc, evel);
                eloc = q.first;
                evel = q.second;
            }
            if (show_trail) {
                cerr << loc << endl;
            }

            if (dead(loc, center_rad, field_rad)) break;
            const int distance = max(abs(loc.x - eloc.x), abs(loc.y - eloc.y));
            if (cnt <= trace_turn && distance <= kamikaze_rad) {
                return max_turn + (trace_turn + 1 - cnt) + (kamikaze_rad - distance);
            }
            ++cnt;
        }

        return cnt;
    }

    bool danger(const Vector &loc, const Vector &vel, const Vector &eloc, const Vector &evel) {
        auto a = simulate(loc, vel).first;
        auto b = simulate(eloc, evel).first;


        return a.x == b.x || a.y == b.y || (abs(a.x - b.x) == abs(a.y - b.y));
    }

    pair<int, Vector> dfs(int depth, const Vector &loc, const Vector &vel, const Vector &eloc, const Vector &evel, const vector<Vector> &vec, int center_rad, long long field_rad, int max_turn, bool danger) {
        if (depth == 6) return make_pair(-1, Vector(0, 0));

        int best = -1;
        Vector best_move = Vector(0, 0);

        const auto next_enemy = simulate(eloc, evel);
        const Vector next_eloc = is_enemy_stopping() ? eloc : next_enemy.first;
        const Vector next_evel = is_enemy_stopping() ? evel : next_enemy.second;

        int use_thruster = danger && depth == 0;
        for (int i = 0 + use_thruster; i < vec.size() + use_thruster; ++i) {
            const auto &v = vec[i%vec.size()];

            auto new_vel = vel;
            new_vel.x += v.x;
            new_vel.y += v.y;

            int result = test(loc, new_vel, eloc, evel, center_rad, field_rad, max_turn, depth);

            auto next_param = simulate(loc, new_vel);
            if (dead(next_param.first, center_rad, field_rad)) continue;

            if (best < result) {
                best = result;
                best_move = v;
            }


            auto dfs_result = dfs(depth + 1, next_param.first, next_param.second, next_eloc, next_evel, vec, center_rad, field_rad, max_turn, false);
            if (best < dfs_result.first) {
                best = dfs_result.first;
                best_move = v;
            }
        }

        return make_pair(best, best_move);
    }

    Vector safe_move(long long planet_size, int field_size, const Vector &loc, const Vector &vel, const Vector &eloc, const Vector &evel, int remaining_turn, bool danger) {
        vector<vector<Vector>> vecs = {
                {Vector(0, 0), Vector(0, 1),  Vector(1, 0),  Vector(1, 1)},
                {Vector(0, 0), Vector(0, 1),  Vector(-1, 0), Vector(-1, 1)},
                {Vector(0, 0), Vector(0, -1), Vector(1, 0),  Vector(1, -1)},
                {Vector(0, 0), Vector(0, -1), Vector(-1, 0), Vector(-1, -1)}
        };

        int best = -1;
        Vector best_move = Vector(0, 0);
        for (const auto &vec: vecs) {
            auto result = dfs(0, loc, vel, eloc, evel, vec, planet_size, field_size, remaining_turn, danger);

            auto new_vel = vel;
            new_vel.x += result.second.x; new_vel.y += result.second.y;
            auto simulated = simulate(loc, new_vel);
            if (simulated.first.x == loc.x && simulated.first.y == loc.y
                && simulated.second.x == vel.x && simulated.second.y == vel.y) continue;

            if (best < result.first) {
                best = result.first;
                best_move = result.second;
            }
        }

        return Vector(-best_move.x, -best_move.y);
    }

    bool check_kamikaze(
            const Vector &loc, const Vector &vel,
            const Vector &enemy_loc, const Vector &enemy_vel
    ) {
        const int next_my_x = loc.x + vel.x;
        const int next_my_y = loc.y + vel.y;
        const int next_op_x = enemy_loc.x + enemy_vel.x;
        const int next_op_y = enemy_loc.y + enemy_vel.y;

        const int distance = max(abs(next_my_x - next_op_x), abs(next_my_y - next_op_y));
        return distance <= kamikaze_rad;
    }


public:
    void test_safe_move(int x, int y) {
        Vector loc(x, y), vel(0, 0);
        Vector eloc(-x, -y), evel(0, 0);
        consecutiveZeroVelocities = 50;

        int planet = 16;
        int field_rad = 128;

        if (abs(loc.x) <= planet && abs(loc.y) <= planet) return;
        if (abs(loc.x) >= field_rad || abs(loc.y) >= field_rad) return;

        int cost = 0;
        int cnt = 0;
        while(cnt < 256) {
            // simulate
            if (check_kamikaze(loc, vel, eloc, evel)) {
                cerr << "kamikaze!" << endl;
            }

            auto vec = safe_move(planet, field_rad, loc, vel, eloc, evel, 256, false);

            if (vec.x != 0 || vec.y != 0) {
                ++cost;
            }

            vel.x -= vec.x;
            vel.y -= vec.y;
            auto new_state = simulate(loc, vel);
            loc = new_state.first;
            vel = new_state.second;

            if (dead(loc, planet, field_rad)) break;

            ++cnt;

            cerr << "current loc:" << loc << " current vel" << vel << endl;
        }

        if (cnt > 256 && cost < 30) {
//            cerr << "cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, planet, field_rad, 256, false) << endl;
        } else {
            if (cnt < 250) {
                cout << "dead at turn cnt: " << cnt << " cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, eloc, evel, planet, field_rad, 256, 0, false) << endl;
            } else {
                cout << "cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, eloc, evel, planet, field_rad, 256, 0, false) << endl;
            }
        }
    }

public:
    JoinParams join_params() {
        return JoinParams();
    }
    StartParams start_params(const GameResponse& response) {
        int spec_point = response.game_info.ship_info.max_points;
        int core = 1;
        int armament = 0;
        int reactor = 10;
        int engine = spec_point - 2 - reactor * 12;
        return StartParams(engine, armament, reactor, core);
    }
    CommandParams command_params(const GameResponse& response) {
        int unit_id = response.game_info.is_defender ? 0 : 1;
        auto pos = response.game_info.is_defender ? response.game_state.defender_states[0].pos : response.game_state.attacker_states[0].pos;
        auto vel = response.game_info.is_defender ? response.game_state.defender_states[0].velocity : response.game_state.attacker_states[0].velocity;

        auto epos = !response.game_info.is_defender ? response.game_state.defender_states[0].pos : response.game_state.attacker_states[0].pos;
        auto evel = !response.game_info.is_defender ? response.game_state.defender_states[0].velocity : response.game_state.attacker_states[0].velocity;

        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
        Vector enemy_location(epos.first, epos.second), enemy_velocity(evel.first, evel.second);

        if (evel.first == 0 && evel.second == 0) {
            consecutiveZeroVelocities++;
        } else {
            consecutiveZeroVelocities = 0;
        }
        cout << "is_enemy_stopping(): " << is_enemy_stopping() << endl;

        if (check_kamikaze(my_location, my_velocity, enemy_location, enemy_velocity)) {
            cout << "Ten-nou Heika Banzai!!!!!" << endl;
            CommandParams params;
            params.commands.push_back(new Kamikaze(unit_id));
            return params;
        }

        Vector next_move = safe_move(response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, my_location, my_velocity, enemy_location, enemy_velocity, response.game_info.max_turns - response.game_state.current_turn,
                danger(pos, vel, epos, evel));
        cout << "Next move: " << next_move << endl;
        CommandParams params;
        if (next_move.x != 0 || next_move.y != 0) {
            params.commands.push_back(new Move(unit_id, next_move));
        }
        return params;
    }
};
