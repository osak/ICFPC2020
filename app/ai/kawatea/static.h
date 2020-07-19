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
        int unit_id = response.game_info.is_defender ? 0 : 1;
        auto pos = response.game_info.is_defender ? response.game_state.defender_states[0].pos : response.game_state.attacker_states[0].pos;
        auto vel = response.game_info.is_defender ? response.game_state.defender_states[0].velocity : response.game_state.attacker_states[0].velocity;
        int x = pos.first;
        int y = pos.second;
        int dx = vel.first;
        int dy = vel.second;
        fprintf(stderr, "x: %d, y: %d\n", x, y);
        fprintf(stderr, "dx: %d, dy: %d\n", dx, dy);
        fflush(stderr);
        int ndx = -direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][0];
        int ndy = -direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][1];
        fprintf(stderr, "ndx: %d, ndy: %d\n", ndx, ndy);
        fflush(stderr);
        CommandParams params;
        if (ndx != 0 || ndy != 0) {
            params.commands.push_back(new Move(unit_id, Vector(ndx, ndy)));
        }
        return params;
    }
    
    private:
    void load() {
        FILE* fp = fopen("ai/kawatea/pre.txt", "r");
        if (fp == NULL) {
            fprintf(stderr, "failed to open pre.txt\n");
            fflush(stderr);
            return;
        }
        int num;
        fscanf(fp, "%d", &num);
        fprintf(stderr, "loading %d records\n", num);
        fflush(stderr);
        for (int i = 0; i < num; i++) {
            unsigned value;
            int x, y, dx, dy, ndx, ndy;
            fscanf(fp, "%u", &value);
            decode(value, x, y, dx, dy, ndx, ndy);
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][0] = ndx;
            direction[x + MAX_P][y + MAX_P][dx + MAX_D][dy + MAX_D][1] = ndy;
        }
        fprintf(stderr, "loaded %d records\n", num);
        fflush(stderr);
        fclose(fp);
    }
};