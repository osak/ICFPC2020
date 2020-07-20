import requests
import sys

# PARAMS
TOURNAMENT_ID = 6
WATCH_LIST = [
    "Unagi",
    "Pigimarl",
    "RGBTeam",
    "Spacecat",
    "182020"
]

MY_NAME = "Atelier Manarimo"

# Constants
API_BASE = "https://icfpc2020-api.testkontur.ru"
API_KEY = "decffdda9f2d431792a37fbfb770f825"
AI_LIST = f"{API_BASE}/submissions/other-teams?apiKey={API_KEY}&tournamentId={TOURNAMENT_ID}"


def battle_url(attacker, defender):
    return f"{API_BASE}/games/non-rating/run?apiKey={API_KEY}&attackerSubmissionId={attacker}&defenderSubmissionId={defender}"


def main(my_submission_id, loops):
    watching_submission_ids = []

    team_list = requests.get(AI_LIST).json()
    for team in team_list:
        submission_id = team["submissionId"]
        team_name = team["team"]["teamName"]
        for watching in WATCH_LIST:
            if team_name in watching:
                print(f"{team_name}={submission_id}")
                watching_submission_ids.append([team_name, submission_id])

    for _ in range(loops):
        for (enemy, enemy_id) in watching_submission_ids:
            url = battle_url(my_submission_id, enemy_id)
            requests.post(url)
            print(f"Triggered {MY_NAME} vs {enemy}")
            url = battle_url(enemy_id, my_submission_id)
            requests.post(url)
            print(f"Triggered {enemy} vs {MY_NAME}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Please specify your submission id")
    if len(sys.argv) > 2:
        loops = int(sys.argv[2])
    else:
        loops = 1
    main(sys.argv[1], loops)
