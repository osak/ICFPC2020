def add(p, q):
    px, py = p
    qx, qy = q
    return px + qx, py + qy


def dot(p, q):
    px, py = p
    qx, qy = q
    return px * qx + py * qy


def main():
    x = 48, 6
    v = 0, 0

    target_velocity = 8
    target_height = 48

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
        g = gx, gy
        nv = add(v, g)

        xnorm = dot(x, x) ** 0.5
        nx, ny = px / xnorm, py / xnorm
        rx, ry = ny, -nx
        r = rx, ry

        r_v = dot(nv, r) / (dot(r, r) ** 0.5)

        cur_height = xnorm

        cur_diff = abs(r_v - target_velocity)
        action = -9999, (-5, -5)
        for ctx in range(-1, 2):
            for cty in range(-1, 2):
                ct = ctx, cty
                cv = add(nv, ct)
                cr_v = dot(cv, r) / (dot(r, r) ** 0.5)
                cx = add(x, cv)
                cand_height = dot(cx, cx) ** 0.5
                improve = (cur_diff - abs(cr_v - target_velocity))
                improve += (abs(cur_height - target_height) - abs(cand_height - target_height))
                cand_action = improve, ct
                if cand_action > action:
                    action = cand_action
        tx, ty = action[1]

        t = tx, ty
        if tx != 0 or ty != 0:
            cost += 1
        v = add(add(v, g), t)
        x = add(x, v)
    print("survived 256 turns with cost {}".format(cost))


if __name__ == '__main__':
    main()