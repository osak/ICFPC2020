import subprocess
import sys
import urllib.request

def post(url: str, body: str):
    print(f"Request: {body}", file=sys.stderr)
    req = urllib.request.Request(url, body.encode(), {'Content-Type': 'text/plain'})
    with urllib.request.urlopen(req) as res:
        response_body = res.read().decode("utf-8")
        print(f"Response: {response_body}", file=sys.stderr)
        return response_body


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



if __name__ == '__main__':
    main() 
