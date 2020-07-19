#pragma once

#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

class AlphinaudAI : public AI {

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

    int test(Vector loc, Vector vel, int center_rad, long long field_rad, int max_turn, bool show_trail = false) {
        int cnt = 0;

        while (cnt < max_turn) {
            auto p = simulate(loc, vel);
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

    pair<int, Vector> dfs(int depth, const Vector &loc, const Vector &vel, const vector<Vector> &vec, int center_rad, long long field_rad, int max_turn) {
        if (depth == 4) return make_pair(-1, Vector(0, 0));

        int best = -1;
        Vector best_move = Vector(0, 0);

        for (const auto &v: vec) {
            auto new_vel = vel;
            new_vel.x += v.x;
            new_vel.y += v.y;

            int result = test(loc, new_vel, center_rad, field_rad, max_turn);

            auto next_param = simulate(loc, new_vel);
            if (dead(next_param.first, center_rad, field_rad)) continue;

            if (best < result) {
                best = result;
                best_move = v;
            }


            auto dfs_result = dfs(depth + 1, next_param.first, next_param.second, vec, center_rad, field_rad, max_turn);
            if (best < dfs_result.first) {
                best = dfs_result.first;
                best_move = v;
            }
        }

        return make_pair(best, best_move);
    }

    Vector safe_move(long long planet_size, int field_size, const Vector &loc, const Vector &vel, int remaining_turn) {
        vector<vector<Vector>> vecs = {
                {Vector(0, 0), Vector(0, 1),  Vector(1, 0),  Vector(1, 1)},
                {Vector(0, 0), Vector(0, 1),  Vector(-1, 0), Vector(-1, 1)},
                {Vector(0, 0), Vector(0, -1), Vector(1, 0),  Vector(1, -1)},
                {Vector(0, 0), Vector(0, -1), Vector(-1, 0), Vector(-1, -1)}
        };

        int best = -1;
        Vector best_move = Vector(0, 0);
        for (const auto &vec: vecs) {
            auto result = dfs(0, loc, vel, vec, planet_size, field_size, remaining_turn);

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

    int get_full_power(const ShipState& my_ship_state) {
        int remaining_heat = my_ship_state.max_heat - my_ship_state.heat + my_ship_state.ship_parameter.recharge_rate;
        return min(remaining_heat, my_ship_state.ship_parameter.attack);
    }

    bool critical_point(const Vector& a, const Vector& b) {
        if (a.x == b.x) return true;
        if (a.y == b.y) return true;
        if (abs(a.x - b.x) == abs(a.y - b.y)) return true;
        return false;
    }

    vector<Command*> critical_shot(int unit_id, const ShipState& my_ship_state, const vector<ShipState>& enemy_ship_states) {
        for (auto enemy_ship_state : enemy_ship_states) {
            Vector next_enemy_location = simulate(Vector(enemy_ship_state.pos), Vector(enemy_ship_state.velocity)).first;
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    auto next_my_velocity = Vector(my_ship_state.velocity) + Vector(dx, dy);
                    Vector next_my_location = simulate(Vector(my_ship_state.pos), next_my_velocity).first;
                    if (critical_point(next_enemy_location, next_my_location)) {
                        vector<Command*> ret;
                        if (dx != 0 && dy != 0) {
                            ret.push_back(new Move(unit_id, Vector(-dx, -dy)));
                        }
                        ret.push_back(new Attack(unit_id, next_enemy_location, get_full_power(my_ship_state)));
                        return ret;
                    }
                }
            }
        }
        return vector<Command*>();
    }

    void test_safe_move() {
        Vector loc(16 + rand()%112, 16 + rand()%112), vel(0, 0);

        int planet = 16;
        int field_rad = 128;

        if (abs(loc.x) <= planet && abs(loc.y) <= planet) return;
        if (abs(loc.x) >= field_rad || abs(loc.y) >= field_rad) return;

        int cost = 0;
        int cnt = 0;
        while(cnt < 256) {
            // simulate
            auto vec = safe_move(planet, field_rad, loc, vel, 256);

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

//            cerr << "current loc:" << loc << " current vel" << vel << endl;
        }

        if (cnt > 256 && cost < 30) {
//            cerr << "cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, planet, field_rad, 256, false) << endl;
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
        int reactor = max(spec_point - 80, 0) / 16;
        int armament = max(spec_point - 80, 0) / 16;
        int engine = spec_point - 2 - reactor * 12 - armament * 4;
        int core = 1;
        return StartParams(engine, armament, reactor, core);
    }
    CommandParams command_params(const GameResponse& response) {
        int unit_id = response.game_info.is_defender ? 0 : 1;
        auto pos = response.game_info.is_defender ? response.game_state.defender_states[0].pos : response.game_state.attacker_states[0].pos;
        auto vel = response.game_info.is_defender ? response.game_state.defender_states[0].velocity : response.game_state.attacker_states[0].velocity;
        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
        Vector next_move = safe_move(response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, my_location, my_velocity, response.game_info.max_turns - response.game_state.current_turn);
        cout << "Next move: " << next_move << endl;

        auto my_ship_state = response.game_info.is_defender ? response.game_state.defender_states[0] : response.game_state.attacker_states[0];
        auto enemy_ship_states = !response.game_info.is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        CommandParams params;
        bool move = false;
        for (auto command : critical_shot(unit_id, my_ship_state, enemy_ship_states)) {
            params.commands.push_back(command);
            move = true;
        };
        if (!move) {
            Vector next_move = safe_move(response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, my_location, my_velocity, response.game_info.max_turns - response.game_state.current_turn);
            if (next_move.x != 0 || next_move.y != 0) {
                params.commands.push_back(new Move(unit_id, next_move));
            }
        }
        cout << "Next commands: " << endl;
        for (auto command : params.commands) {
            command->print();
            cout << endl;
        }
        return params;
    }
};