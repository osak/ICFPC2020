import requests
import psycopg2
from psycopg2.extras import execute_values
import os
import time
from urllib.parse import quote
from threading import Thread

API_BASE = "https://icfpc2020-api.testkontur.ru"
API_KEY = "decffdda9f2d431792a37fbfb770f825"
DATABASE_URL = os.environ["DATABASE_URL"]

def get_connection():
    return psycopg2.connect(DATABASE_URL)

def winner_team_name(game):
    role = game.get(game['winner'].lower())
    if role:
        return role['team']['teamName']
    else:
        return '(Draw)'

def save(games):
    rows = [
        (
            game['gameId'],
            game.get('tournamentId', 0),
            game['createdAt'],
            game['ticks'],
            game['winner'],
            winner_team_name(game),

            game['attacker']['submissionId'],
            game['attacker']['team']['teamId'],
            game['attacker']['team']['teamName'],
            game['attacker']['playerKey'],
            game['attacker'].get('debugLog'),

            game['defender']['submissionId'],
            game['defender']['team']['teamId'],
            game['defender']['team']['teamName'],
            game['defender']['playerKey'],
            game['defender'].get('debugLog')
        )
        for game in games if 'winner' in game
    ]
    with get_connection() as conn:
        with conn.cursor() as cursor:
            execute_values(cursor, """
            INSERT INTO games (
                game_id,
                tournament_id,
                played_at,
                ticks,
                winner,
                winner_team_name,

                attacker_submission_id,
                attacker_team_id,
                attacker_team_name,
                attacker_player_key,
                attacker_debug_log,

                defender_submission_id,
                defender_team_id,
                defender_team_name,
                defender_player_key,
                defender_debug_log
            ) VALUES %s ON CONFLICT DO NOTHING""", rows)
        conn.commit()

def paginated_games(api_path: str):
    sep = '&' if '?' in api_path else '?'
    url_base = '{}{}{}take=100&apiKey={}'.format(API_BASE, api_path, sep, API_KEY)

    before = None
    while True:
        if before == None:
            url = url_base
        else:
            url = '{}&before={}'.format(url_base, quote(before))

        print(url)
        resp = requests.get(url)
        json = resp.json()
        yield json['games']
        before = json['next']
        if not json['hasMore']:
            break

def paginated_rated_games(tournament_id: int):
    for page in paginated_games('/games?tournamentId={}'.format(tournament_id)):
        yield page

def paginated_unrated_games():
    for page in paginated_games('/games/non-rating'):
        yield page

def save_tournament(tournament_id: int):
    for games in paginated_rated_games(tournament_id):
        print("saving {} to {}".format(games[0]['tournamentRoundId'], games[len(games) - 1]['tournamentRoundId']))
        save(games)
        time.sleep(0.5)

def save_unrated():
    for games in paginated_unrated_games():
        print("saving {} to {}".format(games[0]['createdAt'], games[len(games) - 1]['createdAt']))
        save(games)
        time.sleep(0.5)

def backfill():
    for tournament in range(1,5):
        save_tournament(tournament)

class TournamentSaver(Thread):
    def __init__(self):
        Thread.__init__(self, name = "TournamentSaver")

    def run(self):
        while True:
            save_tournament(5)
            time.sleep(60)

class UnratedSaver(Thread):
    def __init__(self):
        Thread.__init__(self, name = "UnratedSaver")

    def run(self):
        while True:
            save_unrated()
            time.sleep(60)

def run():
    if 'DO_BACKFILL' in os.environ:
        backfill()

    ts = TournamentSaver()
    us = UnratedSaver()
    ts.start()
    us.start()
    ts.join()
    us.join()
        
if __name__ == '__main__':
    run()