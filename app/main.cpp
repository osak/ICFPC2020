#include "client.h"
#include "modem.h"
#include "game.h"

#include <iostream>
#include <regex>
#include <string>

#include "ai/mkut/titan.h"
#include "ai/yuusti/meteor.h"

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

int main(int argc, char **argv) {
    Client *client = init_client(argv);
    AI* ai = new MeteorAI();

    JoinParams join_params = ai->join_params();
	GameResponse join_response = GameResponse(as_galaxy(client->join(join_params)));
    StartParams start_params = ai->start_params(join_response);
	GameResponse response(as_galaxy(client->start(start_params)));
    double accum_time = 0;
	while (true) {
        clock_t start_time = clock();
        long long planet_radius = response.game_info.field_info.planet_radius;
        auto pos = response.game_info.is_defender ? response.game_state.defender_state.pos : response.game_state.attacker_state.pos;
        auto vel = response.game_info.is_defender ? response.game_state.defender_state.velocity : response.game_state.attacker_state.velocity;
        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
        cout << "My location: " << my_location << endl;
        cout << "My velocity: " << my_velocity << endl;
        CommandParams command_params = ai->command_params(response);
        clock_t end_time = clock();
        double time_used = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;
        accum_time += time_used;
        cout << "time used: " << time_used << endl;
        cout << "accumulated time used: " << accum_time << endl;

		response = GameResponse(as_galaxy(client->command(command_params)));
	}

    return 0;
}