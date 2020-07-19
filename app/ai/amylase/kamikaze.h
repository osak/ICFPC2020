#pragma once

#include <cmath>
#include <complex>
#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

class KamikazeAI : public AI {
//public:
    typedef complex<double> Point;

    Vector get_gravity(const Vector &loc) {
        int acc_x = abs(loc.x) >= abs(loc.y) ? (loc.x > 0 ? -1 : 1) : 0;
        int acc_y = abs(loc.y) >= abs(loc.x) ? (loc.y > 0 ? -1 : 1) : 0;
        return Vector(acc_x, acc_y);
    }

    Point to_p(const Vector &vec) {
        return Point(vec.x, vec.y);
    }

    double dot(const Point &p, const Point &q) {
        return p.real() * q.real() + p.imag() * q.imag();
    }

    double calc_chase_time(const double x, const double v, const double max_a) {
        if (x == 0) {
            return 0;
        } else if (x < 0) {
            return calc_chase_time(-x, -v, max_a);
        }

        // x > 0
        return (v + sqrt(v * v + max_a * x)) / max_a;
    }

    double calc_arg_chase_time(const double my_arg, const double op_arg, const double v, const double max_a) {
        double d = fmod(my_arg - op_arg, 2 * M_PI);

        double dd;
        if (d >= 0) {
            dd = d - 2 * M_PI;
        } else {
            dd = d + 2 * M_PI;
        }
        return min(calc_chase_time(d, v, max_a), calc_chase_time(dd, v, max_a));

    }

    double calc_arg_distance(const double my_arg, const double op_arg) {
        double d = abs(fmod(my_arg - op_arg, 2 * M_PI));
        return min(d, 2 * M_PI - d);

    }

    Vector chase_move(
            long long planet_size, long long space_size,
            const Vector &loc, const Vector &vel,
            const Vector &enemy_loc, const Vector &enemy_vel
    ) {
        Point my_x = to_p(loc);
        Point my_v = to_p(vel);
        Point op_x = to_p(enemy_loc);
        Point op_v = to_p(enemy_vel);

        Point my_grav = to_p(get_gravity(loc));

        Point my_norm = my_x / abs(my_x);
        Point my_rot = my_norm * Point(0, 1);
        double my_height = abs(my_x);
        double my_arg = arg(my_x);

        Point op_norm = op_x / abs(op_x);
        Point op_rot = op_norm * Point(0, 1);
        double op_height = abs(op_x);
        double op_arg = arg(op_x);
        double op_norm_sp = dot(op_v, op_norm);
        double op_rot_sp = dot(op_v, op_rot) / op_height;  // arg speed
        double new_op_height = op_height + op_norm_sp;
        double new_op_arg = op_arg + op_rot_sp;

        double min_cost = 1e300;
        int min_dx = 0;
        int min_dy = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                Point thrust(dx, dy);

                Point new_my_v = my_v + my_grav + thrust;
                Point new_my_x = my_x + new_my_v;
                double my_norm_sp = dot(new_my_v, my_norm);
                double my_rot_sp = dot(new_my_v, my_rot) / my_height;

                double new_my_height = my_height + my_norm_sp;
                double new_my_arg = my_arg + my_rot_sp;

                double norm_distance = abs(new_op_height - new_my_height);
                double arg_distance = calc_arg_distance(new_my_arg, new_op_arg) * new_my_height;
                double int_height = max(abs(new_my_x.real()), abs(new_my_x.imag()));
                double planet_penalty = 4000 / max(1., int_height - planet_size);
                double outer_penalty = 4000 / max(1., space_size - int_height);
                double cost = pow(norm_distance, 3) + arg_distance + planet_penalty;

//                double norm_chase_time = calc_chase_time(new_op_height - new_my_height, my_norm_sp - op_norm_sp, 1);
//                double max_rot_acc = 1 / new_my_height;
//                double arg_chase_time = calc_arg_chase_time(new_my_arg, new_op_arg, my_rot_sp - op_rot_sp, max_rot_acc);
//                double cost = max(norm_chase_time, arg_chase_time);

                if (cost < min_cost) {
                    min_cost = cost;
                    min_dx = dx;
                    min_dy = dy;
                }

            }
        }
        return Vector(-min_dx, -min_dy);
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
        return distance <= 2;
    }

public:
    JoinParams join_params() override {
        return JoinParams();
    }

    StartParams start_params(const GameResponse &response) override {
        int spec_point = response.game_info.ship_info.max_points;
        int reactor = max(spec_point - 160, 0) / 12;
        int armament = 0;
        int engine = spec_point - 2 - reactor * 12;
        int core = 1;
        return StartParams(engine, armament, reactor, core);
    }

    CommandParams command_params(const GameResponse &response) override {
        int unit_id = response.game_info.is_defender ? 0 : 1;
        auto pos = response.game_info.is_defender
                   ? response.game_state.defender_state.pos
                   : response.game_state.attacker_state.pos;
        auto vel = response.game_info.is_defender
                   ? response.game_state.defender_state.velocity
                   : response.game_state.attacker_state.velocity;

        auto enemy_pos = response.game_info.is_defender
                   ? response.game_state.attacker_state.pos
                   : response.game_state.defender_state.pos;
        auto enemy_vel = response.game_info.is_defender
                   ? response.game_state.attacker_state.velocity
                   : response.game_state.defender_state.velocity;

        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
        Vector enemy_location(enemy_pos.first, enemy_pos.second), enemy_velocity(enemy_vel.first, enemy_vel.second);

        if (check_kamikaze(my_location, my_velocity, enemy_location, enemy_velocity)) {
            cout << "Ten-nou Heika Banzai!!!!!" << endl;
            CommandParams params;
            params.commands.push_back(new Kamikaze(unit_id));
            return params;
        }

        Vector next_move = chase_move(
                response.game_info.field_info.planet_radius,
                response.game_info.field_info.field_radius,
                my_location, my_velocity,
                enemy_location, enemy_velocity);
        cout << "Next move: " << next_move << endl;
        CommandParams params;
        if (next_move.x != 0 || next_move.y != 0) {
            params.commands.push_back(new Move(unit_id, next_move));
        }
        return params;
    }
};
