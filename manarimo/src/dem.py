def dem_tup(s):
    first, rem = dem_top(s)
    second, rem = dem_top(rem)
    return (first, second), rem


def dem_num(s):
    i = 0
    while s[i] == "1":
        i += 1
    offset = i + 1
    if i == 0:
        return 0, s[1:]
    rem_start = offset + 4 * i
    bin_number = s[offset: rem_start]
    return int(bin_number, 2), s[rem_start:]


def dem_top(s):
    if len(s) == 0:
        return ""
    if len(s) < 2:
        raise ValueError()
    typ = s[:2]
    if typ == "00":
        return None, s[2:]
    elif typ == "01" or typ == "10":
        num, rem = dem_num(s[2:])
        if typ == "10":
            num *= -1
        return num, rem
    else:
        return dem_tup(s[2:])


def demodulate(s):
    return dem_top(s)[0]


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


def serialize(x):
    if type(x) == tuple:
        fst, snd = x
        return "({}, {})".format(serialize(fst), serialize(snd))
    elif type(x) == list:
        ser_items = [serialize(item) for item in x]
        return "[" + ", ".join(ser_items) + "]"
    elif x is None:
        return "nil"
    else:
        return str(x)


def run(s):
    return serialize(to_list(demodulate(s)))


def main():
    print(run(input()))


if __name__ == '__main__':
    main()