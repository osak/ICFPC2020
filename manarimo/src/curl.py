import json

import requests


LOCAL_BASE = "http://localhost:8080"
PROD_BASE = "https://manarimo.herokuapp.com"


def post_send(base):
    headers = {'Content-type': 'application/json'}

    post_result = requests.post(base + "/api/send", data='{"payload": "[1,2,3]"}', headers=headers)
    print(post_result.status_code)
    print(post_result.text)


def get_history(base):
    result = requests.get(base + "/api/send_history")
    print(result.status_code)
    print(result.text)


def post_interact(base):
    headers = {'Content-type': 'application/json'}

    data_obj = {
        "state": "nil",
        "data": "(0,0)"
    }

    post_result = requests.post(base + "/api/interact", data=json.dumps(data_obj), headers=headers)
    print(post_result.status_code)
    print(post_result.text)


def post_replay(base):
    headers = {'Content-type': 'application/json'}

    data_obj = {
        "playerKey": "1392211421083123212"
    }

    post_result = requests.post(base + "/api/replay", data=json.dumps(data_obj), headers=headers)
    print(post_result.status_code)
    print(post_result.text)


def main():
    base = LOCAL_BASE
    post_replay(base)


if __name__ == '__main__':
    main()