#pragma once

#include <cstdio>
#include "calc.h"
#include "../../command.h"
#include "../../game.h"
#include "../../ai.h"

int direction[MAX_P * 2 + 2][MAX_P * 2 + 2][MAX_D * 2 + 2][MAX_D * 2 + 2][2];

class StaticAI : public AI {
    public:
    JoinParams join_params() {
        load();
        return JoinParams();
    }
    
    StartParams start_params(const GameResponse& response) {
        int spec_point = response.game_info.ship_info.max_points;
        int reactor = (spec_point - 80) / 12;
        int armament = 0;
        int engine = spec_point - 2 - reactor * 12 - armament * 4;
        int core = 1;
        return StartParams(engine, armament, reactor, core);
    }
    
    CommandParams command_params(const GameResponse& response) {
        auto pos = response.game_info.is_defender ? response.game_state.defender_states[0].pos : response.game_state.attacker_states[0].pos;
        auto vel = response.game_info.is_defender ? response.game_state.defender_states[0].velocity : response.game_state.attacker_states[0].velocity;
        int unit_id = response.game_info.is_defender ? response.game_state.defender_states[0].id : response.game_state.attacker_states[0].id;
        int dx = direction[pos.first + MAX_P][pos.second + MAX_P][vel.first + MAX_D][vel.second + MAX_D][0];
        int dy = direction[pos.first + MAX_P][pos.second + MAX_P][vel.first + MAX_D][vel.second + MAX_D][1];
        CommandParams params;
        if (dx != 0 || dy != 0) {
            params.commands.push_back(new Move(unit_id, Vector(dx, dy)));
        }
        return params;
    }
    
    private:
    void load() {
        FILE* fp = fopen("ai/kawatea/pre.txt", "r");
        if (fp == NULL) {
            fprintf(stderr, "failed to open pre.txt\n");
            return;
        }
        int num;
        fscanf(fp, "%d", &num);
        for (int i = 0; i < num; i++) {
            unsigned value;
            int x, y, dx, dy, ndx, ndy;
            fscanf(fp, "%u", &value);
            decode(value, x, y, dx, dy, ndx, ndy);
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][0] = ndx;
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][1] = ndy;
        }
        fclose(fp);
    }
};