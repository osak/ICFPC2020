#include "client.h"
#include "modem.h"
#include "game.h"

#include <iostream>
#include <regex>
#include <string>

using namespace std;

Client *init_client(char **argv) {
    const std::string serverUrl(argv[1]);
    const std::string playerKey(argv[2]);

    std::cout << "ServerUrl: " << serverUrl << "; PlayerKey: " << playerKey
              << std::endl;

    const std::regex urlRegexp("http://(.+):(\\d+)");
    std::smatch urlMatches;
    if (!std::regex_search(serverUrl, urlMatches, urlRegexp) ||
        urlMatches.size() != 3) {
        std::cout << "Bad server URL" << std::endl;
        exit(1);
    }
    const std::string serverName = urlMatches[1];
    const int serverPort = std::stoi(urlMatches[2]);
    return new Client(serverName, serverPort, atoll(playerKey.c_str()));
}

pair<Vector, Vector> next_location_and_velocity(const Vector& loc, const Vector& vel) {
    int acc_x = abs(loc.x) >= abs(loc.y) ? (loc.x > 0 ? -1 : 1) : 0;
    int acc_y = abs(loc.y) >= abs(loc.x) ? (loc.y > 0 ? -1 : 1) : 0;
    Vector next_velocity(vel.x + acc_x, vel.y + acc_y);
    Vector next_location(loc.x + next_velocity.x, loc.y + next_velocity.y);
    return make_pair(next_location, next_velocity);
}

int area(const Vector& loc) {
    // 3
    //2 0 +
    // 1
    // +
    if (abs(loc.x) > abs(loc.y)) {
        return loc.x > 0 ? 0 : 2;
    } else {
        return loc.y > 0 ? 1 : 3;
    }
}

Vector safe_move(long long planet_size, const Vector& loc, const Vector& vel) {
    int current_area = area(loc);

    Vector tmp_loc = loc, tmp_vel = vel;
    while (area(tmp_loc) == current_area) {
        auto pair = next_location_and_velocity(tmp_loc, tmp_vel);
        tmp_loc = pair.first;
        tmp_vel = pair.second;
        if (abs(tmp_loc.x) <= planet_size || abs(tmp_loc.y) <= planet_size) {
            switch (current_area) {
                case 0: return Vector(-1, -1);
                case 1: return Vector(1, -1);
                case 2: return Vector(1, 1);
                case 3: return Vector(-1, 1);
            }
        }
    }
    return Vector();
}

void test_safe_move() {
    Vector loc(-4, 48), vel(0, 0);
    cout << safe_move(16, loc, vel) << endl;
}

int main(int argc, char **argv) {
	// test();

    Client *client = init_client(argv);

	GalaxyValue* join_resnponse = as_galaxy(client->join(JoinParams()));
    int spec_point = join_resnponse->list[2]->list[2]->list[0]->num;
    int reactor = max(spec_point - 160, 0) / 12;
    int armament = 0;
    int engine = spec_point - 2 - reactor * 12;
    int core = 1;
	GameResponse response(as_galaxy(client->start(StartParams(engine, armament, reactor, core))));
    int unit_id = response.game_info.is_defender ? 0 : 1;
    cout << "Unit ID: " << unit_id << endl;
	while (true) {
        long long planet_radius = response.game_info.field_info.planet_radius;
        auto pos = response.game_info.is_defender ? response.game_state.defender_state.pos : response.game_state.attacker_state.pos;
        auto vel = response.game_info.is_defender ? response.game_state.defender_state.velocity : response.game_state.attacker_state.velocity;
        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
        cout << "My location: " << my_location << endl;
        cout << "My velocity: " << my_velocity << endl;
        Vector next_move = safe_move(response.game_info.field_info.planet_radius, my_location, my_velocity);
        cout << "Next move: " << next_move << endl;
        CommandParams params;
        if (next_move.x != 0 && next_move.y != 0) {
            params.commands.push_back(new Move(unit_id, next_move));
        }
		client->command(params);
	}

    return 0;
}