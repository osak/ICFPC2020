#pragma once

#include <set>

#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

class AlisaieAI : public AI {

    int center_rad;
    int field_rad;

    bool dead(const Vector &loc) {
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

    int test(Vector loc, Vector vel, int max_turn, bool show_trail = false) {
        int cnt = 0;

        while (cnt < max_turn) {
            auto p = simulate(loc, vel);
            loc = p.first;
            vel = p.second;

            if (show_trail) {
                cerr << loc << endl;
            }

            if (dead(loc)) break;
            ++cnt;
        }

        return cnt;
    }

    pair<int, Vector> dfs(int depth, const Vector &loc, const Vector &vel, const vector<Vector> &vec, int max_turn) {
        if (depth == 4) return make_pair(-1, Vector(0, 0));

        int best = -1;
        Vector best_move = Vector(0, 0);

        for (const auto &v: vec) {
            auto new_vel = vel;
            new_vel.x += v.x;
            new_vel.y += v.y;

            int result = test(loc, new_vel, max_turn);

            auto next_param = simulate(loc, new_vel);
            if (dead(next_param.first)) continue;

            if (best < result) {
                best = result;
                best_move = v;
            }


            auto dfs_result = dfs(depth + 1, next_param.first, next_param.second, vec, max_turn);
            if (best < dfs_result.first) {
                best = dfs_result.first;
                best_move = v;
            }
        }

        return make_pair(best, best_move);
    }

    pair<Vector, int> safe_move(const Vector &loc, const Vector &vel, int remaining_turn) {
        vector<vector<Vector>> vecs = {
                {Vector(0, 0), Vector(0, 1),  Vector(1, 0),  Vector(1, 1)},
                {Vector(0, 0), Vector(0, 1),  Vector(-1, 0), Vector(-1, 1)},
                {Vector(0, 0), Vector(0, -1), Vector(1, 0),  Vector(1, -1)},
                {Vector(0, 0), Vector(0, -1), Vector(-1, 0), Vector(-1, -1)}
        };

        int best = -1;
        Vector best_move = Vector(0, 0);
        for (const auto &vec: vecs) {
            auto result = dfs(0, loc, vel, vec, remaining_turn);

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
        int remaining_heat = max(0, my_ship_state.max_heat - my_ship_state.heat + my_ship_state.ship_parameter.recharge_rate - move * 8);
        return min(remaining_heat, my_ship_state.ship_parameter.attack);
    }

    int get_free_power(const ShipState& my_ship_state, int move) {
        return max(0, my_ship_state.ship_parameter.recharge_rate - my_ship_state.heat - move * 8);
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
            for (int dx = -2; dx <= 2; dx++) {
                for (int dy = -2; dy <= 2; dy++) {
                    auto next_my_loc_vel = simulate(Vector(my_ship_state.pos), Vector(my_ship_state.velocity) + Vector(dx, dy));
                    auto next_my_location = next_my_loc_vel.first;
                    auto next_my_velocity = next_my_loc_vel.second;
                    if (dead(next_my_location)) continue;
                    if (safe_move(next_my_location, next_my_velocity, 100).second < 50) continue;
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
            auto vec = safe_move(loc, vel, 256).first;

            if (vec.x != 0 || vec.y != 0) {
                ++cost;
            }

            vel.x -= vec.x;
            vel.y -= vec.y;
            auto new_state = simulate(loc, vel);
            loc = new_state.first;
            vel = new_state.second;

            if (dead(loc)) break;

            ++cnt;

//            cerr << "current loc:" << loc << " current vel" << vel << endl;
        }

        if (cnt > 256 && cost < 30) {
//            cerr << "cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, planet, field_rad, 256, false) << endl;
        } else {
            if (cnt < 250) {
                cout << "dead at turn cnt: " << cnt << " cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, 256, false) << endl;
            } else {
                cout << "cost" << cost << " loc:" << loc << " vel:" << vel << " " << test(loc, vel, 256, false) << endl;
            }
        }
    }

    bool stable(const Vector& loc, const Vector& vel, int max_turn) {
        return test(loc, vel, max_turn) == max_turn;
    }

    set<int> ids_to_move;
    vector<Command*> personal_commands(const ShipState& my_ship_state, const vector<ShipState>& enemy_ship_states, int max_turn) {
        vector<Command*> ret;
        // move
        if (!stable(my_ship_state.pos, my_ship_state.velocity, max_turn)) {
            Vector next_move = safe_move(my_ship_state.pos, my_ship_state.velocity, max_turn).first;
            if (next_move.x != 0 || next_move.y != 0) {
                ret.push_back(new Move(my_ship_state.id, next_move));
            }
            auto next_loc_vel = simulate(my_ship_state.pos, my_ship_state.velocity);
            if (stable(next_loc_vel.first, next_loc_vel.second, max_turn)) {
                if (my_ship_state.ship_parameter.life > 1) {
                    // fission
                    StartParams clone_params(my_ship_state.ship_parameter.energy / 2, my_ship_state.ship_parameter.attack / 2, my_ship_state.ship_parameter.recharge_rate / 2, my_ship_state.ship_parameter.life / 2);
                    ret.push_back(new Fission(my_ship_state.id, clone_params));
                    ids_to_move.insert(my_ship_state.id);
                }
            }
            return ret;
        }

        // move after fission
        if (ids_to_move.count(my_ship_state.id)) {
            int best = -1;
            Vector best_diff;
            for (int x = -2; x <= 2; x++) {
                for (int y = -2; y <= 2; y++) {
                    Vector diff(x, y);
                    auto next_loc_vel = simulate(my_ship_state.pos, Vector(my_ship_state.velocity) + diff);
                    if (safe_move(next_loc_vel.first, next_loc_vel.second, max_turn).second >= 50) {
                        if (best < abs(x) + abs(y)) {
                            best = abs(x) + abs(y);
                            best_diff = diff;
                        }
                    }
                }
            }
            if (best > 0) {
                ret.push_back(new Move(my_ship_state.id, best_diff));
                ids_to_move.erase(my_ship_state.id);
            }
        }

        Vector my_next_location = simulate(my_ship_state.pos, my_ship_state.velocity).first;

        // kamikaze

        // critical
        for (const ShipState& enemy_ship_state : enemy_ship_states) {
            Vector next_enemy_location = simulate(enemy_ship_state.pos, enemy_ship_state.velocity).first;
            if (critical_point(next_enemy_location, my_next_location)) {
                int power = get_full_power(my_ship_state, 0);
                ret.push_back(new Attack(my_ship_state.id, next_enemy_location, power));
                return ret;
            }
        }
    }

public:
    JoinParams join_params() {
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
        int unit_id = response.game_info.is_defender ? 0 : 1;
        auto my_ship_states = response.game_info.is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        auto enemy_ship_states = !response.game_info.is_defender ? response.game_state.defender_states : response.game_state.attacker_states;

        CommandParams params;
        for (auto my_ship_state : my_ship_states) {
            for (auto command : personal_commands(my_ship_state, enemy_ship_states, response.game_info.max_turns - response.game_state.current_turn)) {
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