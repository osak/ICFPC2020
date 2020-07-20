import src.send as send
import src.gamestate as gamestate
import src.dem as dem
import json
import sys
import csv


def extract_attacks(raw_response):
    data = gamestate.decode(dem.run(raw_response))
    result = []
    for turn in data["details"]["turns"]:
        for d in turn["data"]:
            location = d["state"]["location"]
            commands = d["command"]
            if commands == None:
                continue
            for command in commands:
                if command[0] != 2:
                    continue

                x = location["x"]
                y = location["y"]
                result.append({"x": x, "y": y, "command": command})

    return result


def main():
    attacks = []
    with open("saved_responses.in") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            print(f"loading {i+1}/{len(lines)}")
            attacks += extract_attacks(lines[i])
    with open("attacks.csv", "w") as f:
        writer = csv.writer(f)
        writer.writerow(["from_x", "from_y", "to_x", "to_y",
                         "param1", "param2", "param3"])
        for attack in attacks:
            from_x, from_y = attack["x"], attack["y"]
            to_x, to_y = attack["command"][1][0], attack["command"][1][1]
            param1 = attack["command"][2]
            param2 = attack["command"][3]
            param3 = attack["command"][4]
            writer.writerow(
                [from_x, from_y, to_x, to_y, param1, param2, param3])


if __name__ == "__main__":
    main()
