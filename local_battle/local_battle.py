import requests
import json
import subprocess
import time
import sys

BATTLE_DOME_URL = "https://manarimo.herokuapp.com/api/battledome"
AI_URL = "http://manarimo.herokuapp.com:80"


def print_help():
    print(
        "Usage: python3 ./local_battle.py [attacker_path] [attacker_name] [defender_path] [defender_name]"
    )
    return


def main(args):
    if args[1] is "-h" or args[1] is "--help" or len(args) < 5:
        print_help()
        return
    attacker_path = args[1]
    attacker_name = args[2]
    defender_path = args[3]
    defender_name = args[4]
    print("attacker_path: ", attacker_path)
    print("attacker_name: ", attacker_name)
    print("defender_path: ", defender_path)
    print("defender_name: ", defender_name)
    headers = {'Content-type': 'application/json'}
    request = {
        "attacker_name": attacker_name,
        "defender_name": defender_name
    }
    response = requests.post(
        BATTLE_DOME_URL,
        data=json.dumps(request),
        headers=headers).json()

    attacker_key = response["attacker_key"]
    defender_key = response["defender_key"]

    attacker_process = subprocess.Popen(
        [attacker_path, AI_URL, attacker_key])
    defender_process = subprocess.Popen(
        [defender_path, AI_URL, defender_key])
    while attacker_process.poll() is None and defender_process.poll() is None:
        print("waiting AIs ...")
        time.sleep(1)


if __name__ == "__main__":
    main(sys.argv)
