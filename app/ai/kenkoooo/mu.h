#pragma once

#include "../../ai.h"
#include "../../command.h"
#include "../../game.h"

class ChokudAI : public AI {

  public:
    JoinParams join_params() override { return JoinParams(); }
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
        auto my_ship_state = response.game_info.is_defender
                                 ? response.game_state.defender_states[0]
                                 : response.game_state.attacker_states[0];
        auto enemy_ship_states = !response.game_info.is_defender
                                     ? response.game_state.defender_states
                                     : response.game_state.attacker_states;

        CommandParams params;
        bool move = false;
        if (enemy_ship_states.size() == 1) {
            auto commands =
                try_kamikaze(unit_id, my_ship_state, enemy_ship_states[0]);
            if (commands.size() > 0) {
                for (auto command : commands) {
                    params.commands.push_back(command);
                    return params;
                }
            }
        }

        for (auto command :
             critical_shot(unit_id, my_ship_state, enemy_ship_states)) {
            params.commands.push_back(command);
            move = true;
        };

        if (!move) {
            Vector next_move = safe_move(
                response.game_info.field_info.planet_radius,
                response.game_info.field_info.field_radius,
                Vector(my_ship_state.pos), Vector(my_ship_state.velocity),
                response.game_info.max_turns -
                    response.game_state.current_turn);
            if (next_move.x != 0 || next_move.y != 0) {
                params.commands.push_back(new Move(unit_id, next_move));
                move = true;
            }
        }

        // randomize
        if (!move &&
            my_ship_state.pos.first == -enemy_ship_states[0].pos.first &&
            my_ship_state.pos.second == -enemy_ship_states[0].pos.second &&
            my_ship_state.velocity.first ==
                -enemy_ship_states[0].velocity.first &&
            my_ship_state.velocity.second ==
                -enemy_ship_states[0].velocity.second &&
            !(my_ship_state.velocity.first == 0 &&
              my_ship_state.velocity.second == 0)) {
            params.commands.push_back(new Move(
                unit_id, Vector(my_ship_state.pos.first > 0 ? 1 : -1, 0)));
            move = true;
        }

        {
            Command *command =
                free_shot(unit_id, my_ship_state, enemy_ship_states, move);
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