import json
from pathlib import Path
import subprocess
import tempfile
import os
import src.compile as compile
import src.dem as dem
import src.mod as mod
import src.send as send


galaxy_cache = None
SRC_DIR = Path(__file__).parent


def get_tree(s):
    return dem.demodulate(mod.run(s))


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

'{"state":"(6,((0,(9,(270608505102339400,(2,(0,(2,(nil,(nil,(4,((8,((16,(128,nil)),((((1,(0,((-10,-27),((-4,6),((202,(30,(10,(1,nil)))),(20,(64,(1,nil)))))))),(((2,((-16,12),(30,(25,(4,nil))))),nil),nil)),(((0,(1,((-16,12),((0,-8),((0,(0,(0,(0,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil))),(nil,(((16,(128,nil)),(((0,((((1,(0,((16,-48),((0,0),((206,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),(((0,(1,((-16,48),((0,0),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((1,((((1,(0,((15,-48),((-1,0),((205,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(((0,((1,1),nil)),nil),nil)),(((0,(1,((-16,47),((0,-1),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((2,((((1,(0,((13,-48),((-2,0),((204,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(((0,((1,1),nil)),nil),nil)),(((0,(1,((-16,45),((0,-2),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((3,((((1,(0,((10,-47),((-3,1),((203,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(((0,((1,0),nil)),nil),nil)),(((0,(1,((-16,42),((0,-3),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((4,((((1,(0,((6,-45),((-4,2),((202,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(((0,((1,0),nil)),nil),nil)),(((0,(1,((-16,38),((0,-4),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((5,((((1,(0,((2,-42),((-4,3),((202,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),(((0,(1,((-16,33),((0,-5),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((6,((((1,(0,((-2,-38),((-4,4),((202,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),(((0,(1,((-16,27),((0,-6),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((7,((((1,(0,((-6,-33),((-4,5),((202,(30,(10,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),(((0,(1,((-16,20),((0,-7),((510,(0,(0,(1,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),((8,((((1,(0,((-10,-27),((-4,6),((202,(30,(10,(1,nil)))),(20,(64,(1,nil)))))))),(((2,((-16,12),(30,(25,(4,nil))))),nil),nil)),(((0,(1,((-16,12),((0,-8),((0,(0,(0,(0,nil)))),(0,(64,(1,nil)))))))),(nil,nil)),nil)),nil)),nil))))))))),nil)),(nil,nil))))))))))))),(1,(nil,nil))))","data":"(0,0)"}'
def apply(state, data):
    interp_path = get_interp_path()
    galaxy = get_galaxy()

    if state.find('ap') == -1:
        state_str = compile.run(state)
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

    raw = None
    state = None
    data = []
    for l in lines:
        if not l.strip():
            continue
        key, value = [s.strip() for s in l.split("=")]
        if key == "full":
            raw = value
        elif key == "state":
            state = value
        elif key == "data":
            data.append(value)
        else:
            raise ValueError("Unknown key `{}`".format(key))
    return raw, state, data


def run(state, data):
    while True:
        raw, state, data = apply(state, data)
        raw_tree = get_tree(raw)
        num, tl = raw_tree
        _, tl = tl
        send_data, nil = tl
        if num == 0:
            break
        else:
            send_request = dem.serialize(send_data)
            data = send.exchange(send_request)["response"]
    return {
        "state": state,
        "data": data
    }


def main():
    import sys
    request = json.load(sys.stdin)
    json.dump(run(**request), sys.stdout, indent=4)


if __name__ == "__main__":
    main()