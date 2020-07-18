from pathlib import Path
import sys


sys.setrecursionlimit(10000)
lib_file = Path(__file__).parent / "curry_lib.scm"
galaxy_file = Path(__file__).parent.parent / "blob" / "sun.txt"

output_file = Path(__file__).parent / "output.scm"


def serialize(t):
    if type(t) == tuple:
        x, y = t
        return "(delay ((force {}) {}))".format(serialize(x), serialize(y))
    else:
        return "{}".format(t)


def insert_paren(eq):
    tokens = eq.split()
    stack = []
    overrides = {"div", "cons", "car", "cdr"}
    for token in reversed(tokens):
        if token == "ap":
            applied = (stack[-1], stack[-2])
            stack.pop()
            stack.pop()
            stack.append(applied)
        elif token in overrides:
            stack.append("my" + token)
        else:
            stack.append(token)
    assert len(stack) == 3
    return "({} {})".format(stack[2], serialize(stack[0]))


def main():
    with output_file.open("w") as of:

        with lib_file.open() as f:
            print(f.read(), file=of)

        with galaxy_file.open() as f:
            let_defs = []
            for l in f:
                parsed = insert_paren(l)
                let_def = parsed.replace(":", "v")
                let_defs.append(let_def)

            # print("(letrec ({}) (print (calc galaxy)))".format(" ".join(let_defs)), file=of)
            print("(letrec ({}) (print (force galaxy)))".format(" ".join(let_defs)), file=of)


if __name__ == "__main__":
    main()