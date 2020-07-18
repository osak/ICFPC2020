import json
from pathlib import Path
import subprocess
import tempfile
import os
import src.compile as compile


galaxy_cache = None
SRC_DIR = Path(__file__).parent


def get_galaxy():
    global galaxy_cache
    if galaxy_cache is not None:
        return galaxy_cache
    with (SRC_DIR.parent / "galaxy.txt").open() as f:
        galaxy_cache = f.read()
    return galaxy_cache


def get_tmpdir():
    if os.name == "nt":
        # windows
        return "./"
    else:
        return "/tmp"


def get_interp_path():
    if os.name == "nt":
        # windows
        return SRC_DIR.parent.parent / "a.exe"
    else:
        return SRC_DIR.parent / "a.out"

'{"state":"nil","data":"(0,0)"}'
def run(state, data):
    interp_path = get_interp_path()
    galaxy = get_galaxy()

    if state.find('ap') == -1:
        stats_str = compile.run(state)
    else:
        state_str = state.strip()

    galaxy_def = ":galaxy = ap ap :1338 {} {}".format(state_str, compile.run(data))

    fd, tf_path = tempfile.mkstemp(dir=get_tmpdir())

    with open(tf_path, "w") as tf:
        print(galaxy.strip(), file=tf)
        print(galaxy_def, file=tf)

    response = subprocess.check_output([str(interp_path), "--interactive", tf_path]).decode()  # type: str
    os.close(fd)
    os.remove(tf_path)
    lines = response.splitlines()
    # raw = lines[0]

    result = {
        "data": []
    }
    for l in lines[1:]:
        key, value = [s.strip() for s in l.split("=")]
        if key == "state":
            result["state"] = value
        elif key == "data":
            result["data"].append(value)
        else:
            raise ValueError("Unknown key `{}`".format(key))
    return result


def main():
    import sys
    request = json.load(sys.stdin)
    json.dump(run(**request), sys.stdout, indent=4)


if __name__ == "__main__":
    main()