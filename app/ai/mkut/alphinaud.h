#pragma once

#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

class AlphinaudAI : public AI {
    /*
public:
    void test() {
        auto x = safe_move(16, 128, Vector(6, 27), Vector(0, -6), 100);
        cerr << x.first << "," << x.second << endl;
    }
*/
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

    pair<Vector, int> safe_move(long long planet_size, int field_size, const Vector &loc, const Vector &vel, int remaining_turn) {
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

        return make_pair(Vector(-best_move.x, -best_move.y), best);
    }

    int get_full_power(const ShipState& my_ship_state, int move) {
        int remaining_heat = max(0, my_ship_state.max_heat - my_ship_state.heat + my_ship_state.ship_parameter.recharge_rate - move * 2);
        return min(remaining_heat, my_ship_state.ship_parameter.attack);
    }

    int get_free_power(const ShipState& my_ship_state, int move) {
        return max(0, my_ship_state.ship_parameter.recharge_rate - my_ship_state.heat - move * 2);
    }

    bool critical_point(const Vector& a, const Vector& b) {
        if (a.x == b.x) return true;
        if (a.y == b.y) return true;
        if (abs(a.x - b.x) == abs(a.y - b.y)) return true;
        return false;
    }

    vector<Command*> critical_shot(int unit_id, const ShipState& my_ship_state, const vector<ShipState>& enemy_ship_states, int center_rad, int field_rad) {
        for (auto enemy_ship_state : enemy_ship_states) {
            Vector next_enemy_location = simulate(Vector(enemy_ship_state.pos), Vector(enemy_ship_state.velocity)).first;
            for (int dx = -2; dx <= 2; dx++) {
                for (int dy = -2; dy <= 2; dy++) {
                    auto next_my_loc_vel = simulate(Vector(my_ship_state.pos), Vector(my_ship_state.velocity) + Vector(dx, dy));
                    auto next_my_location = next_my_loc_vel.first;
                    auto next_my_velocity = next_my_loc_vel.second;
                    if (dead(next_my_location, center_rad, field_rad)) continue;
                    if (safe_move(center_rad, field_rad, next_my_location, next_my_velocity, 100).second < 50) continue;
                    if (critical_point(next_enemy_location, next_my_location)) {
                        vector<Command*> ret;
                        if (dx != 0 || dy != 0) {
                            ret.push_back(new Move(unit_id, Vector(-dx, -dy)));
                        }
                        int power = get_full_power(my_ship_state, max(abs(dx), abs(dy)));
                        ret.push_back(new Attack(unit_id, next_enemy_location, power));
                        if (power >= my_ship_state.ship_parameter.recharge_rate * 3) {
                            return ret;
                        }
                    }
                }
            }
        }
        return vector<Command*>();
    }

    Command* free_shot(int unit_id, const ShipState& my_ship_state, const vector<ShipState>& enemy_ship_states, int move) {
        auto enemy_ship_state = enemy_ship_states[enemy_ship_states.size() - 1];
        Vector next_enemy_location = simulate(Vector(enemy_ship_state.pos), Vector(enemy_ship_state.velocity)).first;
        int power = get_free_power(my_ship_state, move);
        if (power > 0) {
            return new Attack(unit_id, next_enemy_location, power);
        }
        return NULL;
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
            auto vec = safe_move(planet, field_rad, loc, vel, 256).first;

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
        int reactor = 10;
        int armament = max(spec_point - 208, 0) / 4;
        int engine = spec_point - 2 - reactor * 12 - armament * 4;
        int core = 1;
        return StartParams(engine, armament, reactor, core);
    }
    CommandParams command_params(const GameResponse& response) {
        int unit_id = response.game_info.is_defender ? 0 : 1;
        auto my_ship_state = response.game_info.is_defender ? response.game_state.defender_states[0] : response.game_state.attacker_states[0];
        auto enemy_ship_states = !response.game_info.is_defender ? response.game_state.defender_states : response.game_state.attacker_states;

        CommandParams params;
        int move = 0;

        for (auto command : critical_shot(unit_id, my_ship_state, enemy_ship_states, response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius)) {
            params.commands.push_back(command);
            move = 2;
        };

        if (!move) {
            Vector next_move = safe_move(response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, Vector(my_ship_state.pos), Vector(my_ship_state.velocity), response.game_info.max_turns - response.game_state.current_turn).first;
            if (next_move.x != 0 || next_move.y != 0) {
                params.commands.push_back(new Move(unit_id, next_move));
                move = max(abs(next_move.x), abs(next_move.y));
            }
        }

        // randomize
        if (!move && my_ship_state.pos.first == -enemy_ship_states[0].pos.first && my_ship_state.pos.second == -enemy_ship_states[0].pos.second &&
            my_ship_state.velocity.first == -enemy_ship_states[0].velocity.first && my_ship_state.velocity.second == -enemy_ship_states[0].velocity.second &&
            !(my_ship_state.velocity.first == 0 && my_ship_state.velocity.second == 0)) {
            params.commands.push_back(new Move(unit_id, Vector(my_ship_state.pos.first > 0 ? 1 : -1, 0)));
            move = 1;
        }

        {
            Command* command = free_shot(unit_id, my_ship_state, enemy_ship_states, move);
            if (command != NULL) {
                params.commands.push_back(command);
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