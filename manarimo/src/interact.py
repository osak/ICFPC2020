import json
from pathlib import Path
import subprocess
import tempfile
import os
import src.compile as compile


galaxy_cache = None
REPO_ROOT = Path(__file__).parent.parent.parent / "blob"


def get_galaxy():
    global galaxy_cache
    if galaxy_cache is not None:
        return galaxy_cache
    with (REPO_ROOT / "manarimo" / "galaxy.txt").open() as f:
        galaxy_cache = f.read()
    return galaxy_cache


def get_tmpdir():
    if os.name == "nt":
        # windows
        return "./"
    else:
        return "/tmp"


def run(state, data):
    interp_path = REPO_ROOT / "a.out"
    galaxy = get_galaxy()

    galaxy_def = ":galaxy = ap ap :1338 ap ap {} {}".format(compile.run(state), compile.run(data))

    with tempfile.NamedTemporaryFile(dir=get_tmpdir()) as tf:
        tf.write(galaxy)
        if not galaxy.endswith("\n"):
            tf.write("\n")
        tf.write(galaxy_def)

        response = subprocess.check_output([interp_path, "--interact", tf.name]).decode()  # type: str
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