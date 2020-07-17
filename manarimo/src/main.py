from flask import Flask, render_template, request, jsonify
import psycopg2
import os
import manarimo.src.send
from datetime import datetime, timezone
import psycopg2.extras


DATABASE_URL = os.environ["DATABASE_URL"]


app = Flask(__name__, static_url_path="/web-dist")
app.config['MAX_CONTENT_LENGTH'] = 50 * 1024 * 1024


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

    return jsonify(results)


@app.route("/")
def hello():
    # todo: distribute webpack file via static
    return render_template('index.html')


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080)