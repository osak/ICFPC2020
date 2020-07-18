def disasm_tup(s):
    first, rem = disasm_top(s)
    second, rem = disasm_top(rem)
    return "ap ap cons {} {}".format(first, second), rem


def disasm_num(s):
    i = 0
    while s[i] == "1":
        i += 1
    offset = i + 1
    if i == 0:
        return 0, s[1:]
    rem_start = offset + 4 * i
    bin_number = s[offset: rem_start]
    return int(bin_number, 2), s[rem_start:]


def disasm_top(s):
    if len(s) == 0:
        return ""
    if len(s) < 2:
        raise ValueError()
    typ = s[:2]
    if typ == "00":
        return "nil", s[2:]
    elif typ == "01" or typ == "10":
        num, rem = disasm_num(s[2:])
        if typ == "10":
            num *= -1
        return str(num), rem
    else:
        return disasm_tup(s[2:])


def disassemble(s):
    return disasm_top(s)[0]


def run(s):
    return disassemble(s)


def main():
    print(run(input()))


if __name__ == '__main__':
    main()