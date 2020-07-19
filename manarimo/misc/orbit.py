def add(p, q):
    px, py = p
    qx, qy = q
    return px + qx, py + qy


def main():
    x = 64, 0
    v = 0, 0

    cost = 0
    for turn in range(256):
        px, py = x
        print("turn {}: {}, {}".format(turn, px, py))
        if max(abs(px), abs(py)) > 128:
            print("went outer space on turn {}".format(turn))
            return
        if max(abs(px), abs(py)) < 16:
            print("crashed on planet surface {}".format(turn))
            return
        gx, gy = 0, 0
        if px >= 0 and px >= abs(py):
            gx -= 1
        if px <= 0 and px <= -abs(py):
            gx += 1
        if py >= 0 and py >= abs(px):
            gy -= 1
        if py <= 0 and py <= -abs(px):
            gy += 1



        nx = add(x, v)
        nv = add(v, (gx, gy))
        x, v = nx, nv
    print("survived 256 turns with cost {}".format(cost))


if __name__ == '__main__':
    main()