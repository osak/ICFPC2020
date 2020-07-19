#include "client.h"
#include "modem.h"

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

struct ShipInfo {
    int max_points;
    int max_velocity;
    int max_heat;

    ShipInfo() {}

    ShipInfo(GalaxyValue *value) {
        assert(value->list.size() == 3);

        max_points = static_cast<int>(value->list[0]->num);
        max_velocity = static_cast<int>(value->list[1]->num);
        max_heat = static_cast<int>(value->list[2]->num);
    }
};

struct FieldInfo {
    int planet_radius;
    int field_radius;

    FieldInfo() {}

    FieldInfo(GalaxyValue *value) {
        assert(value->list.size() == 2);

        planet_radius = static_cast<int>(value->list[0]->num);
        field_radius = static_cast<int>(value->list[1]->num);
    }
};

struct ShipParameter {
    int energy;
    int attack;
    int recharge_rate;
    int life;

    ShipParameter() : energy(-1), attack(-1), recharge_rate(-1), life(-1) {}

    ShipParameter(GalaxyValue *value) {
        // Empty list when the enemy is the attacker
        if (value->list.size() != 4) {
            energy = attack = recharge_rate = life = -1;
            return;
        }

        energy = static_cast<int>(value->list[0]->num);
        attack = static_cast<int>(value->list[1]->num);
        recharge_rate = static_cast<int>(value->list[2]->num);
        life = static_cast<int>(value->list[3]->num);
    }
};

struct GameInfo {
    int max_turns;
    int is_defender;
    ShipInfo ship_info;
    FieldInfo field_info;
    ShipParameter defender_parameter;

    GameInfo() {}

    GameInfo(const GalaxyValue *value) {
        assert(value->type == GalaxyValue::Type::LIST && value->list.size() == 5);
        max_turns = static_cast<int>(value->list[0]->num);
        is_defender = static_cast<int>(value->list[1]->num);
        ship_info = ShipInfo(value->list[2]);
        field_info = FieldInfo(value->list[3]);
        defender_parameter = ShipParameter(value->list[4]);
    }

};

pair<int, int> to_pair(const GalaxyValue* value) {
    assert(value->list.size() == 2);
    return make_pair(value->list[0]->num, value->list[1]->num);
}

struct ShipState {
    bool is_defender;
    bool is_attacker;
    pair<int, int> pos;
    pair<int, int> velocity;
    ShipParameter ship_parameter;
    int heat;
    int max_heat;
    int thruster_power;
    GalaxyValue* last_move;

    ShipState() {}

    // [[1,0,[28,-48],[0,0],[10,10,10,10],0,64,1],[]]
    ShipState(const GalaxyValue *value) {
        assert(value->list.size() == 2);

        auto* ship = value->list[0];
        assert(ship->list.size() == 8);

        is_defender = static_cast<bool>(ship->list[0]->num);
        is_attacker = static_cast<bool>(ship->list[1]->num);
        pos = to_pair(ship->list[2]);
        velocity = to_pair(ship->list[3]);
        ship_parameter = ShipParameter(ship->list[4]);
        heat = static_cast<int>(ship->list[5]->num);
        max_heat = static_cast<int>(ship->list[6]->num);
        thruster_power = static_cast<int>(ship->list[7]->num);

        last_move = value->list[1];
    }
};

struct GameState {
    int current_turn;
    FieldInfo field_info;
    ShipState defender_state;
    ShipState attacker_state;

    GameState() {}

    GameState(const GalaxyValue *value) {
        assert(value->list.size() == 3);

        current_turn = static_cast<int>(value->list[0]->num);
        field_info = FieldInfo(value->list[1]);

        auto* ship_states = value->list[2];
        assert(ship_states->list.size() == 2);
        defender_state = ShipState(ship_states->list[0]);
        attacker_state = ShipState(ship_states->list[1]);
    }
};

struct GameResponse {
    int success;
    enum GameStage {
        NOT_STARTED,
        IN_PROGRESS,
        FINISHED,
    } stage;

    GameInfo game_info;

    GameState game_state;

    GameResponse(const GalaxyValue *value) {
        const auto &list = value->list;
        assert(list.size() == 4);

        success = static_cast<int>(list[0]->num);
        stage = static_cast<GameStage>(list[1]->num);
        game_info = GameInfo(list[2]);
        // JOIN and START can return empty game state
        game_state = list[3]->list.empty() ? GameState() : GameState(list[3]);
    }
};

ostream &operator<<(ostream &os, const ShipInfo &si) {
    os << "Ship info{";
    os << "max_points: " << si.max_points;
    os << ", max_velocity: " << si.max_velocity;
    os << ", max_heat: " << si.max_heat << "}";

    return os;
}

ostream &operator<<(ostream &os, const FieldInfo &fi) {
    os << "Field Info{";
    os << "planet_radius: " << fi.planet_radius;
    os << ", field_radius: " << fi.field_radius << "}";

    return os;
}

ostream &operator<<(ostream &os, const ShipParameter &ds) {
    os << "Ship Parameter{";
    os << "energy: " << ds.energy;
    os << ", attack: " << ds.attack;
    os << ", recharge_rate: " << ds.recharge_rate;
    os << ", life: " << ds.life << "}";

    return os;
}

ostream &operator<<(ostream &os, const GameInfo &gi) {
    os << "Game Info{";
    os << "max_turns: " << gi.max_turns;
    os << ", is_defender: " << gi.is_defender;
    os << ", " << gi.ship_info;
    os << ", " << gi.field_info;
    os << ", [defender] " << gi.defender_parameter << "}";

    return os;
}

ostream &operator<<(ostream &os, const pair<int, int> &pair) {
    os << '(' << pair.first << ',' << pair.second << ')';

    return os;
}

ostream &operator<<(ostream &os, const ShipState &ss) {
    os << "Ship State{";
    os << "is_defender: " << ss.is_defender;
    os << ", is_attacker: " << ss.is_attacker;
    os << ", pos: " << ss.pos;
    os << ", velocity: " << ss.velocity;
    os << ", ship_parameter: " << ss.ship_parameter;
    os << ", heat: " << ss.heat;
    os << ", max_heat: " << ss.max_heat;
    os << ", thruster_power: " << ss.thruster_power;
    os << ", last_move: " << *ss.last_move << "}";

    return os;
}

ostream &operator<<(ostream &os, const GameState &gs) {
    os << "Game State{";
    os << "current_turn: " << gs.current_turn;
    os << ", field_info: " << gs.field_info;
    os << ", defender_state: " << gs.defender_state;
    os << ", attacker_state: " << gs.attacker_state << "}";

    return os;
}

ostream &operator<<(ostream &os, const GameResponse &gr) {
    os << "Game Response{";
    os << "success: " << gr.success;
    os << ", stage: " << gr.stage;
    os << ", " << gr.game_info;
    os << ", " << gr.game_state;

    return os;
}

void testGame() {
    const auto *galaxy = as_galaxy(Demodulator().demodulate(
            "110110000111011000011111011110000100000000110101111011110001000000000110110000111011100100000000111101110000100001101110100000000011110110000111011000011101100001110110000100001111011000011111011100001000011011101000000000111111110110000111010111101110001011111011000101011111110100001010111101100001110110000111011000011101100001001101011011100100000011011000010011000011111101011011000011111101100010111101110001010111111011000010101111011010101101101010110110101011011010100011010110111001000000110110000100110000000000"));
    cout << *galaxy << endl;
    const auto decoded_response = GameResponse(galaxy);
    cout << decoded_response << endl;
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
        auto pair = next_location_and_velocity(loc, vel);
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
	while (true) {
        long long planet_radius = response.game_info.field_info.planet_radius;
        auto pos = response.game_info.is_defender ? response.game_state.defender_state.pos : response.game_state.attacker_state.pos;
        auto vel = response.game_info.is_defender ? response.game_state.defender_state.velocity : response.game_state.attacker_state.velocity;
        Vector my_location(pos.first, pos.second), my_velocity(vel.first, vel.second);
        Vector next_move = safe_move(response.game_info.field_info.planet_radius, my_location, my_velocity);
        CommandParams params;
        if (next_move.x != 0 && next_move.y != 0) {
            params.commands.push_back(new Move(next_move));
        }
		client->command(params);
	}

    return 0;
}