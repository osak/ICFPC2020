def modulate_num(s):
    p = 0
    d = 0
    neg = False
    if s[0] == '-':
        neg = True
        p += 1
    while p < len(s) and s[p] in "0123456789":
        d = d * 10 + int(s[p])
        p += 1
    if d == 0:
        return "010", s[3:]

    bin_d = bin(d)[2:]
    l = (len(bin_d) + 3) // 4
    pad_bin_d = "0" * (l * 4 - len(bin_d)) + bin_d
    header = "1" * l + "0"
    typ = "10" if neg else "01"
    return typ + header + pad_bin_d, s[p:]


def modulate_top(s):
    if not s:
        return "", ""
    if s[0] == "n":
        # nil
        return "00", s[3:]
    elif s[0] == '[':
        rem = s
        result = ""
        while rem[0] != ']':
            item, rem = modulate_top(rem[1:])
            result += "11" + item
        return result + "00", rem[1:]
    elif s[0] == '(':
        # tup
        first, rem = modulate_top(s[1:])
        second, rem = modulate_top(rem[1:])
        return "11" + first + second, rem[1:]
    else:
        return modulate_num(s)


def modulate(s):
    s = s.replace(" ", "")
    return modulate_top(s)


def run(s):
    return modulate(s)[0]


def main():
    print(run(input()))


if __name__ == '__main__':
    main()