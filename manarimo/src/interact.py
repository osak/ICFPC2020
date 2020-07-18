import json
from pathlib import Path


galaxy_cache = None


def get_galaxy():
    global galaxy_cache
    if galaxy_cache is not None:
        return galaxy_cache
    with (Path(__file__).parent.parent.parent / "blob" / "galaxy.txt").open() as f:
        galaxy_cache = f.read()
    return galaxy_cache


def run(state, data):
    # mock. todo: implement
    return {
        "state": state,
        "data": ["[]", "[(1,2)]"]
    }


def main():
    import sys
    request = json.load(sys.stdin)
    json.dump(run(**request), sys.stdout, indent=4)


if __name__ == "__main__":
    main()