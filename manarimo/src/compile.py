import src.mod as mod
import src.disasm as disasm


def run(s):
    return disasm.run(mod.run(s))


def main():
    print(run(input()))


if __name__ == '__main__':
    main()