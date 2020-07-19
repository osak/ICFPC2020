#pragma once 
#include "../../ai.h"
#include "../../command.h"

using namespace std;

class SatelliteAI : public AI {
    const int FACTOR = 8;

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
        auto pos = response.game_info.is_defender
                       ? response.game_state.defender_state.pos
                       : response.game_state.attacker_state.pos;
        auto vel = response.game_info.is_defender
                       ? response.game_state.defender_state.velocity
                       : response.game_state.attacker_state.velocity;
        const Vector p(pos.first, pos.second);
        const Vector v(vel.first, vel.second);

        CommandParams res;
        if (p.y > 0 && abs(p.y) > abs(p.x) && abs(v.x) != FACTOR) {
            res.commands.push_back(new Move(unit_id, Vector(v.x - -FACTOR, 0)));
        } else if (p.y < 0 && abs(p.y) > abs(p.x) && abs(v.x) != FACTOR) {
            res.commands.push_back(
                new Move(unit_id, Vector(sign(v.x - FACTOR), 0)));
        } else if (p.x > 0 && abs(p.x) > abs(p.y) && abs(v.y) != FACTOR) {
            res.commands.push_back(
                new Move(unit_id, Vector(0, sign(v.y - -FACTOR))));
        } else if (p.x < 0 && abs(p.x) > abs(p.y) && abs(v.y) != FACTOR) {
            res.commands.push_back(new Move(unit_id, Vector(0, sign(v.y - FACTOR))));
        }
        return res;
    }

  private:
    int sign(int n) {
        if (n == 0)
            return 0;
        if (n > 0)
            return 1;
        return -1;
    }
};
