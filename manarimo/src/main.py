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
    send.raw(request.data)

@app.route("/")
def hello():
    return app.send_static_file('index.html')


@app.route("/bundle.js")
def bundle():
    return app.send_static_file('bundle.js')


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)