import subprocess
import sys
import urllib.request

def post(url: str, body: str):
    req = urllib.request.Request(url, body.encode(), {'Content-Type': 'text/plain'})
    with urllib.request.urlopen(req) as res:
        response_body = res.read()
        print(f"Request: {body}")
        print(f"Response: {response_body}")
        return res.read().decode("utf-8")


def main():
    server_url = sys.argv[1]
    player_key = sys.argv[2]
    post(server_url, player_key)

    with subprocess.Popen(["./main"], encoding='UTF-8', stdin=subprocess.PIPE, stdout=subprocess.PIPE) as proc:
        output = proc.stdout.readline().strip()
        response = response = post(server_url, output)
        for _ in range(3):
            print(f'{response}', file=proc.stdin, flush=True)
            output = proc.stdout.readline().strip()
            response = post(server_url, output)
            print(response)



if __name__ == '__main__':
    main() 
