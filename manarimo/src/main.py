from flask import Flask, request, jsonify
import psycopg2
import os
import src.send as send
from datetime import datetime, timezone
import psycopg2.extras
from pathlib import Path
import src.interact as interact
import sys
import src.gamestate as gamestate


DATABASE_URL = os.environ["DATABASE_URL"]

root_dir = Path(__file__).parent.parent
app = Flask(__name__, static_folder=str(root_dir/"web-dist"), static_url_path="/web-dist")
app.config['MAX_CONTENT_LENGTH'] = 50 * 1024 * 1024
sys.setrecursionlimit(100000)


def get_connection():
    return psycopg2.connect(DATABASE_URL)


@app.route("/api/send", methods=["POST"])
def post_send():
    request_value = request.json["payload"]
    response = send.exchange(request_value)
    timestamp = datetime.now(tz=timezone.utc)
    response["timestamp"] = timestamp.isoformat()
    with get_connection() as conn:
        with conn.cursor() as cursor:
            cursor.execute("INSERT INTO aliens (raw_request, request, raw_response, response, send_at) VALUES (%s, %s, %s, %s, %s)",
                           (response["raw_request"], response["request"], response["raw_response"], response["response"], response["timestamp"])
                           )
        conn.commit()
    return jsonify(response)


@app.route("/api/send_history", methods=["GET"])
def get_send_history():
    from_id = request.args.get("from", 0)
    with get_connection() as conn:
        with conn.cursor(cursor_factory=psycopg2.extras.DictCursor) as cursor:
            cursor.execute(
                "SELECT * FROM aliens WHERE id >= %s",
                (from_id, )
                )
            results = []
            for row in cursor.fetchall():
                dict_row = dict(row)
                dict_row["timestamp"] = dict_row["send_at"].isoformat()
                results.append(dict_row)

    return jsonify({"items": results})


@app.route("/api/interact", methods=["POST"])
def post_interact():
    state = request.json["state"]
    data = request.json["data"]
    response = interact.run(state, data)

    return jsonify({
        "state": response["state"],
        "data": response["data"],
        "sends": response["sends"]
    })


@app.route("/api/replay", methods=["POST"])
def post_replay():
    player_key = request.json["playerKey"]
    response = send.exchange("[5, {}]".format(player_key))

    return jsonify({
        "data": gamestate.decode(response["response"])
    })

@app.route("/aliens/send", methods=["POST"])
def aliens_send():
    return send.raw(request.get_data())

@app.route("/api/battledome", methods=["POST"])
def battledome():
    response = send.exchange("[1, 0]")
    data = eval(response['response'])
    attacker_key = data[1][0][1]
    defender_key = data[1][1][1]
    attacker_name = request.json['attacker_name']
    defender_name = request.json['defender_name']
    timestamp = datetime.now(tz=timezone.utc)
    with get_connection() as conn:
        with conn.cursor() as cursor:
            cursor.execute("INSERT INTO local_runs (attacker_name, attacker_key, defender_name, defender_key, send_at) VALUES (%s, %s, %s, %s, %s)",
                           (attacker_name, attacker_key, defender_name, defender_key, timestamp.isoformat()))
        conn.commit()

    return jsonify({
        "attacker_key": str(attacker_key),
        "defender_key": str(defender_key)
    })

@app.route("/api/local_runs", methods=["GET"])
def get_local_runs():
    from_id = request.args.get("from", 0)
    with get_connection() as conn:
        with conn.cursor(cursor_factory=psycopg2.extras.DictCursor) as cursor:
            cursor.execute(
                "SELECT * FROM local_runs WHERE id >= %s",
                (from_id, )
                )
            results = []
            for row in cursor.fetchall():
                dict_row = dict(row)
                dict_row["timestamp"] = dict_row["send_at"].isoformat()
                results.append(dict_row)

    return jsonify({"items": results})

@app.route("/api/games", methods=["GET"])
def get_games():
    with get_connection() as conn:
        with conn.cursor(cursor_factory=psycopg2.extras.DictCursor) as cursor:
            cursor.execute("SELECT * FROM games ORDER BY played_at DESC")
            results = []
            for row in cursor.fetchall():
                dict_row = dict(row)
                dict_row["played_at"] = dict_row["played_at"].isoformat()
                results.append(dict_row)
    return jsonify({"items": results})

@app.route("/api/submissions", methods=["GET"])
def get_manarimo_submissions():
    with get_connection() as conn:
        with conn.cursor(cursor_factory=psycopg2.extras.DictCursor) as cursor:
            cursor.execute("SELECT * FROM submissions")
            results = []
            for row in cursor.fetchall():
                dict_row = dict(row)
                if dict_row['created_at'] != None:
                    dict_row["created_at"] = dict_row["created_at"].isoformat()
                results.append(dict_row)
    return jsonify({"items": results})

@app.route("/api/submissions/<submission_id>", methods=["POST"])
def update_submission(submission_id):
    new_alias = request.json['alias']
    with get_connection() as conn:
        with conn.cursor(cursor_factory=psycopg2.extras.DictCursor) as cursor:
            cursor.execute("UPDATE submissions SET alias=%s WHERE submission_id=%s RETURNING *", (new_alias, submission_id))
            result = dict(cursor.fetchone())
            if result['created_at'] != None:
                result["created_at"] = result["created_at"].isoformat()
        conn.commit()
    return jsonify({"updated": result})

@app.route("/")
def hello():
    return app.send_static_file('index.html')


@app.route("/bundle.js")
def bundle():
    return app.send_static_file('bundle.js')


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)