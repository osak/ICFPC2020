import requests
import sys

# PARAMS
TOURNAMENT_ID = 5
WATCH_LIST = [
    # "Unagi",
    "Pigimarl",
    "RGBTeam"
]

MY_ID = ["Atelier Manarimo", "7469"]

# Constants
API_BASE = "https://icfpc2020-api.testkontur.ru"
API_KEY = "decffdda9f2d431792a37fbfb770f825"
AI_LIST = f"{API_BASE}/submissions/other-teams?apiKey={API_KEY}&tournamentId={TOURNAMENT_ID}"


def battle_url(attacker: int, defender: int) -> str:
    return f"{API_BASE}/games/non-rating/run?apiKey={API_KEY}&attackerSubmissionId={attacker}&defenderSubmissionId={defender}"


def main():
    watching_submission_ids = []

    team_list = requests.get(AI_LIST).json()
    for team in team_list:
        submission_id = team["submissionId"]
        team_name = team["team"]["teamName"]
        if team_name in WATCH_LIST:
            print(f"{team_name}={submission_id}")
            watching_submission_ids.append([team_name, submission_id])

    watching_submission_ids.append(MY_ID)
    for (a_user, a_id) in watching_submission_ids:
        for (d_user, d_id) in watching_submission_ids:
            url = battle_url(a_id, d_id)
            requests.post(url)
            print(f"Triggered {a_user} vs {d_user}")


if __name__ == "__main__":
    main()
