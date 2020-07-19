#pragma once

#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

class GoatAI : public AI {

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

    bool danger(const Vector &loc, const Vector &vel, const Vector &eloc, const Vector &evel) {
        auto a = simulate(loc, vel).first;
        auto b = simulate(eloc, evel).first;


        return a.x == b.x || a.y == b.y || (abs(a.x - b.x) == abs(a.y - b.y));
    }

    pair<int, Vector> dfs(int depth, const Vector &loc, const Vector &vel, const vector<Vector> &vec, int center_rad, long long field_rad, int max_turn, bool danger) {
        if (depth == 4) return make_pair(-1, Vector(0, 0));

        int best = -1;
        Vector best_move = Vector(0, 0);

        int use_thruster = danger && depth == 0;
        for (int i = 0 + use_thruster; i < vec.size() + use_thruster; ++i) {
            const auto &v = vec[i%vec.size()];

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


            auto dfs_result = dfs(depth + 1, next_param.first, next_param.second, vec, center_rad, field_rad, max_turn, false);
            if (best < dfs_result.first) {
                best = dfs_result.first;
                best_move = v;
            }
        }

        return make_pair(best, best_move);
    }

    Vector safe_move(long long planet_size, int field_size, const Vector &loc, const Vector &vel, int remaining_turn, bool danger) {
        vector<vector<Vector>> vecs = {
                {Vector(0, 0), Vector(0, 1),  Vector(1, 0),  Vector(1, 1)},
                {Vector(0, 0), Vector(0, 1),  Vector(-1, 0), Vector(-1, 1)},
                {Vector(0, 0), Vector(0, -1), Vector(1, 0),  Vector(1, -1)},
                {Vector(0, 0), Vector(0, -1), Vector(-1, 0), Vector(-1, -1)}
        };

        int best = -1;
        Vector best_move = Vector(0, 0);
        for (const auto &vec: vecs) {
            auto result = dfs(0, loc, vel, vec, planet_size, field_size, remaining_turn, danger);

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

    ShipState getMainShip(vector<ShipState>& ships) {
        ShipState main_ship = ships[0];
        int best_score = 0;
        for (auto ship: ships) {
            ShipParameter param = ship.ship_parameter;
            int score = param.life * 2 + param.energy + param.attack * 4 + param.recharge_rate * 8;
            if (best_score < score) {
                best_score = score;
                main_ship = ship;
            }
        }
        return main_ship;
    }

    int distance(pair<int, int> a, pair<int, int> b) {
        return max(abs(a.first - b.first), abs(a.second - b.second));
    }

public:
    void test_safe_move(int x, int y) {
        Vector loc(x, y), vel(0, 0);

        int planet = 16;
        int field_rad = 128;

        if (abs(loc.x) <= planet && abs(loc.y) <= planet) return;
        if (abs(loc.x) >= field_rad || abs(loc.y) >= field_rad) return;

        int cost = 0;
        int cnt = 0;
        while(cnt < 256) {
            // simulate
            auto vec = safe_move(planet, field_rad, loc, vel, 256, false);

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

    bool fissioned = false;
    int num_children = 0;

    JoinParams join_params() {
        return JoinParams();
    }
    StartParams start_params(const GameResponse& response) {
        int spec_point = response.game_info.ship_info.max_points;
        int reactor = max(spec_point - 160, 0) / 12;
        int armament = 0;
        int engine = spec_point - 20 - reactor * 12;
        int core = 10;
        return StartParams(engine, armament, reactor, core);
    }

    CommandParams command_params(const GameResponse& response) {
        bool is_defender = response.game_info.is_defender;
        int main_ship_id = is_defender ? 0 : 1;
        vector<ShipState> ships = is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        vector<ShipState> enemies = !is_defender ? response.game_state.defender_states : response.game_state.attacker_states;
        ShipState main_ship = ships[0];
        CommandParams params;
        ShipState target = getMainShip(enemies);
        Vector next_move;

        cout << "Target: " << target.pos.first + target.velocity.first << " " << target.pos.second + target.velocity.second << endl;

        for (auto ship: ships) {
            int unit_id = ship.id;
            if (unit_id == main_ship_id) {
                // main unit
                auto pos = ship.pos;
                auto vel = ship.velocity;
                auto epos = enemies[0].pos;
                auto evel = enemies[0].velocity;

                int life = ship.ship_parameter.life;
                int remaining_turn = response.game_info.max_turns - response.game_state.current_turn;
                Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
                next_move = safe_move(response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, my_location, my_velocity, response.game_info.max_turns - response.game_state.current_turn,
                        danger(pos, vel, epos, evel));
                cout << "Next move: " << next_move << endl;
                if (next_move.x != 0 || next_move.y != 0) {
                    params.commands.push_back(new Move(unit_id, next_move));
                } else if (!fissioned) {
                    if (life > 1 && ship.ship_parameter.energy > 5 && remaining_turn > 5) {
                        int score = test(pos, vel, response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, remaining_turn);

                        if ((num_children < 2 && score > 100) || score >= remaining_turn) {
                            cout << "Fission! It will live for " << score << "turns" << endl;
                            num_children++;
                            params.commands.push_back(new Fission(unit_id, StartParams(2, 0, 0, 1)));
                            fissioned = true;
                        }
                    }
                } else {
                    vector<Vector> vecs = {Vector(1, 1), Vector(1, -1), Vector(-1, 1), Vector(-1, -1)};
                    Vector best_move = Vector(0, 0);
                    int best = 20;
                    for (auto& vec: vecs) {
                        int score = test(pos, {-vec.x + vel.first, -vec.y + vel.second}, response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, remaining_turn);
                        if (score > best) {
                            best = score;
                            best_move = vec;
                        }
                    }

                    if (best_move.x != 0 || best_move.y != 0) {
                        cout << "Decided move a bit: " << best_move << endl;
                        params.commands.push_back(new Move(unit_id, best_move));
                        fissioned = false;
                        next_move = best_move;
                    }
            
                }
            } else {
                if (unit_id == main_ship.id) {
                    continue;
                }
                pair<int, int> main_ship_next_pos = {main_ship.pos.first + main_ship.velocity.first - next_move.x, main_ship.pos.second + main_ship.velocity.second - next_move.y};
                pair<int, int> target_next_pos = {target.pos.first + target.velocity.first, target.pos.second + target.velocity.second};
                
                const int initialBestDistance = 4;
                int bestDistance = initialBestDistance;
                pair<int, int> bestMove = {0, 0};
                for (int i = -2; i <= 2; i++) { for (int j = -2; j <= 2; j++) {
                    pair<int, int> next_pos = {ship.pos.first + ship.velocity.first + i, ship.pos.second + ship.velocity.second + j};
                    if (distance(next_pos, main_ship_next_pos) <= 4) {
                        // avoid bombing our main ship
                        continue;
                    }
                    if (distance(next_pos, target_next_pos) < bestDistance) {
                        bestDistance = distance(next_pos, target_next_pos);
                        bestMove = {i, j};
                    }
                }}
                if (bestDistance < initialBestDistance) {
                    cout << "DOITDOITDOIT! " << ship.pos.first + ship.velocity.first + bestMove.first << ", " << ship.pos.second + ship.velocity.second + bestMove.second << endl; 
                    params.commands.push_back(new Move(unit_id, Vector(-bestMove.first, -bestMove.second)));
                    params.commands.push_back(new Kamikaze(unit_id));
                }
            }
        }
        return params;
    }
};
