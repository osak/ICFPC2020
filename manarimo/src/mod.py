def modulate_num(s, offset, buf):
    p = offset
    d = 0
    neg = False
    if s[offset] == '-':
        neg = True
        p += 1
    while p < len(s) and s[p] in "0123456789":
        d = d * 10 + int(s[p])
        p += 1
    if d == 0:
        buf.append("010")
        return p

    bin_d = bin(d)[2:]
    l = (len(bin_d) + 3) // 4
    pad_bin_d = "0" * (l * 4 - len(bin_d)) + bin_d
    header = "1" * l + "0"
    typ = "10" if neg else "01"
    buf.append(typ + header + pad_bin_d)
    return p


def modulate_top(s, offset, buf):
    length = len(s)
    if offset == length:
        return
    if s[offset] == "n":
        # nil
        buf.append("00")
        return offset + 3
    elif s[offset] == '[':
        while s[offset] != ']':
            buf.append("11")
            offset = modulate_top(s, offset + 1, buf)
        buf.append("00")
        return offset + 1
    elif s[offset] == '(':
        # tup
        buf.append("11")
        offset = modulate_top(s, offset + 1, buf)
        offset = modulate_top(s, offset + 1, buf)
        return offset + 1
    else:
        return modulate_num(s, offset, buf)


def modulate(s):
    s = s.replace(" ", "")
    buf = []
    modulate_top(s, 0, buf)
    return ''.join(buf)


def run(s):
    return modulate(s)


def main():
    print(run(input()))


if __name__ == '__main__':
    main()