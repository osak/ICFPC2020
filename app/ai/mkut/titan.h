#pragma once

#include <cmath>
#include "../../command.h"
#include "../../game.h"

class TitanAI {
   pair<Vector, Vector> next_location_and_velocity(const Vector& loc, const Vector& vel) {
      int acc_x = abs(loc.x) >= abs(loc.y) ? (loc.x > 0 ? -1 : 1) : 0;
      int acc_y = abs(loc.y) >= abs(loc.x) ? (loc.y > 0 ? -1 : 1) : 0;
      Vector next_velocity(vel.x + acc_x, vel.y + acc_y);
      Vector next_location(loc.x + next_velocity.x, loc.y + next_velocity.y);
      return make_pair(next_location, next_velocity);
   }

   double living_time(long long planet_size, long long space_size, const Vector& loc, const Vector& vel) {
      Vector tmp_loc = loc, tmp_vel = vel;
      double ret = 0;
      int cnt = 0;
      while (true) {
         if ((abs(tmp_loc.x) <= planet_size && abs(tmp_loc.y) <= planet_size) || (abs(tmp_loc.x) > space_size && abs(tmp_loc.y) > space_size)) {
               break;
         }
         auto pair = next_location_and_velocity(tmp_loc, tmp_vel);
         tmp_loc = pair.first;
         tmp_vel = pair.second;
         ret += acos(loc * tmp_loc / loc.norm() / tmp_loc.norm());
         cnt++;
         if (cnt > 100) {
               return ret;
         }
      }
      return ret;
   }

   Vector safe_move(long long planet_size, long long space_size, const Vector& loc, const Vector& vel) {
      int max_t = living_time(planet_size, space_size, loc, vel);
      int max_dx = 0;
      int max_dy = 0;
      for (int dx = -1; dx <= 1; dx++) {
         for (int dy = -1; dy <= 1; dy++) {
               if (dx == 0 && dy == 0) continue;
               Vector tmp_vel = vel;
               tmp_vel.x += dx;
               tmp_vel.y += dy;
               double t = living_time(planet_size, space_size, loc, tmp_vel);
               // cerr << dx << "," << dy << ":" << t << endl;
               if (t > max_t) {
                  max_t = t;
                  max_dx = dx;
                  max_dy = dy;
               }
         }
      }
      return Vector(-max_dx, -max_dy);
   }

public:
   JoinParams join_params() {
      return JoinParams();
   }
   StartParams start_params(const GameResponse& response) {
      int spec_point = response.game_info.ship_info.max_points;
      int reactor = max(spec_point - 160, 0) / 12;
      int armament = 0;
      int engine = spec_point - 2 - reactor * 12;
      int core = 1;
      return StartParams(engine, armament, reactor, core);
   }
   CommandParams command_params(const GameResponse& response) {
      int unit_id = response.game_info.is_defender ? 0 : 1;
      auto pos = response.game_info.is_defender ? response.game_state.defender_state.pos : response.game_state.attacker_state.pos;
      auto vel = response.game_info.is_defender ? response.game_state.defender_state.velocity : response.game_state.attacker_state.velocity;
      Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
      Vector next_move = safe_move(response.game_info.field_info.planet_radius, response.game_info.field_info.field_radius, my_location, my_velocity);
      cout << "Next move: " << next_move << endl;
      CommandParams params;
      if (next_move.x != 0 || next_move.y != 0) {
         params.commands.push_back(new Move(unit_id, next_move));
      }
      return params;
   }
};

/*
int main() {
   Vector loc(-10, -48), vel(0, 0);
   cout << safe_move(16, 128, loc, vel) << endl;
}
*/