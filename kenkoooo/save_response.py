import src.send as send
import src.gamestate as gamestate
import requests
import json

API_URL = "https://icfpc2020-api.testkontur.ru/games?apiKey=decffdda9f2d431792a37fbfb770f825&tournamentId=2&take=10000"


def fetch_player_ids():
    keys = []
    response = requests.get(API_URL).json()
    games = response["games"]
    for game in games:
        if "playerKey" not in game["attacker"]:
            continue
        playerKey = game["attacker"]["playerKey"]
        keys.append(playerKey)
    return keys


def fetch_results(player_key):
    response = send.exchange(f"[5, {player_key}]")
    return response["raw_response"]


def main():
    keys = fetch_player_ids()
    for key in keys:
        print(fetch_results(key))


if __name__ == "__main__":
    main()
