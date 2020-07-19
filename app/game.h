#pragma once

#include "value.h"
#include <cassert>

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
    ShipState defender_state; // deprecated
    ShipState attacker_state; // deprecated
    vector<ShipState> defender_states;
    vector<ShipState> attacker_states;

    GameState() {}

    GameState(const GalaxyValue *value) {
        assert(value->list.size() == 3);

        current_turn = static_cast<int>(value->list[0]->num);
        field_info = FieldInfo(value->list[1]);

        auto* ship_states = value->list[2];

        // to be removed
        defender_state = ShipState(ship_states->list[0]);
        attacker_state = ShipState(ship_states->list[1]);

        for (auto& state: ship_states->list) {
            ShipState ship_state = ShipState(state);
            if (ship_state.is_defender) {
                defender_states.push_back(ship_state);
            } else {
                attacker_states.push_back(ship_state);
            }
        }
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
