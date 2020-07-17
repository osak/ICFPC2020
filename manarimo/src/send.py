import requests
import manarimo.src.mod
import manarimo.src.dem


API_BASE = "https://icfpc2020-api.testkontur.ru"
API_KEY = "decffdda9f2d431792a37fbfb770f825"


def exchange(request):
    raw_request = mod.run(request)
    params = {
        "apiKey": API_KEY
    }
    post_result = requests.post(API_BASE + "/aliens/send", data=raw_request, params=params)
    if post_result.status_code != 200:
        raise IOError("Failed to communicate with the server")
    raw_response = post_result.text
    response = dem.run(raw_response)
    return {
        "request": request,
        "raw_request": raw_request,
        "raw_response": raw_response,
        "response": response
    }


def main():
    import json
    result = exchange(input())
    print(json.dumps(result, indent=4))


if __name__ == '__main__':
    main()