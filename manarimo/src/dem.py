def dem_tup(s, offset):
    first, offset = dem_top(s, offset)
    second, offset = dem_top(s, offset)
    return (first, second), offset


def dem_num(s, s_offset):
    i = s_offset
    while s[i] == "1":
        i += 1
    offset = i + 1
    if i == s_offset:
        return 0, s_offset + 1
    rem_start = offset + 4 * (i - s_offset)
    bin_number = s[offset: rem_start]
    return int(bin_number, 2), rem_start


def dem_top(s, offset):
    length = len(s) - offset
    if length == 0:
        return ""
    if length < 2:
        raise ValueError()
    typ = s[offset:offset + 2]
    if typ == "00":
        return None, offset + 2
    elif typ == "01" or typ == "10":
        num, offset = dem_num(s, offset + 2)
        if typ == "10":
            num *= -1
        return num, offset
    else:
        return dem_tup(s, offset + 2)


def demodulate(s):
    return dem_top(s, 0)[0]


def to_list(x):
    if type(x) == tuple:
        fst, snd = x
        fst = to_list(fst)
        snd = to_list(snd)
        if snd is None:
            return [fst]
        elif type(snd) == list:
            return [fst] + snd
        else:
            return x
    else:
        return x


def _serialize(x, buf):
    if type(x) == tuple:
        fst, snd = x
        buf.append("(")
        _serialize(fst, buf)
        buf.append(", ")
        _serialize(snd, buf)
        buf.append(")")
        return
    elif type(x) == list:
        buf.append("[")
        for item in x:
            _serialize(item, buf)
            buf.append(", ")
        if len(x) > 0:
            buf.pop()
        buf.append("]")
        return
    elif x is None:
        buf.append("nil")
        return
    else:
        buf.append(str(x))
        return


def serialize(x):
    buf = []
    _serialize(x, buf)
    return ''.join(buf)


def run(s):
    return serialize(to_list(demodulate(s)))


def main():
    print(run(input()))


if __name__ == '__main__':
    main()