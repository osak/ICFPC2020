def decode_e_element(expr):
    is_defender, b, c = expr
    return {
        "is_defender": bool(is_defender),
        "b": b,
        "c": c
    }


def decode_e(expr):
    return [decode_e_element(e) for e in expr]


def decode_vec(expr):
    x, y = expr
    return {
        "x": x,
        "y": y
    }


def decode_parameters(expr):
    fuel, attack, cool_speed, health = expr
    return {
        "fuel": fuel,
        "attack": attack,
        "cool_speed": cool_speed,
        "health": health
    }


def decode_state(expr):
    is_defender, is_attacker, location, velocity, parameters, heat, max_heat, max_velocity = expr
    return {
        "is_defender": bool(is_defender),
        "is_attacker": bool(is_attacker),
        "location": decode_vec(location),
        "velocity": decode_vec(velocity),
        "parameters": decode_parameters(parameters),
        "heat": heat,
        "max_heat": max_heat,
        "max_velocity": max_velocity
    }


def decode_command(expr):
    "returns a list of list."
    return expr


def decode_state_and_command(expr):
    state, command = expr
    return {
        "state": decode_state(state),
        "command": decode_command(command)
    }


def decode_state_and_commands(expr):
    return [decode_state_and_command(e) for e in expr]


def decode_turn(expr):
    turn_id, state_and_commands = expr
    return {
        "turn_id": turn_id,
        "data": decode_state_and_commands(state_and_commands)
    }


def decode_turns(expr):
    return [decode_turn(e) for e in expr]


def decode_f(expr):
    [planet_size, space_size], turns = expr
    return {
        "planet_size": planet_size,
        "space_size": space_size,
        "turns": decode_turns(turns)
    }


def decode(human_readable_expr):
    a, b, c, turns, e, f = eval(human_readable_expr.replace("nil", "None"))
    return {
        "a": a,
        "b": b,
        "c": c,
        "turns": turns,
        "result": decode_e(e),
        "details": decode_f(f)
    }


if __name__ == '__main__':
    import json
    import sys
    print(json.dumps(decode(sys.stdin.read()), indent=4))