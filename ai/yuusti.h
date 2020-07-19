
pair<Vector, Vector> simulate(Vector loc, Vector vel) {
    if (abs(loc.x) >= abs(loc.y)) {
        if (loc.x < 0) ++vel.x;
        if (loc.x > 0) --vel.x;
    }

    if (abs(loc.x) <= abs(loc.y)) {
        if (loc.y < 0) ++vel.y;
        if (loc.y > 0) --vel.y;
    }

    loc.x += vel.x;
    loc.y += vel.y;

    return make_pair(loc, vel);
};

int test(Vector loc, Vector vel, int center_rad, long long field_rad, int max_turn, bool show_trail = false) {
    int cnt = 0;

    while (cnt < max_turn) {
        auto p = simulate(loc, vel);
        loc = p.first;
        vel = p.second;

        if (show_trail) {
            cerr << loc << endl;
        }

        if (abs(loc.x) <= center_rad && abs(loc.y) <= center_rad) break;
        if (abs(loc.x) >= field_rad || abs(loc.y) >= field_rad) break;
        ++cnt;
    }

    return cnt;
}

pair<int, Vector> dfs(int depth, const Vector &loc, const Vector &vel, const vector<Vector> &vec, int center_rad, long long field_rad, int max_turn) {
    if (depth == 5) return make_pair(-1, Vector(0, 0));

    int best = -1;
    Vector best_move = Vector(0, 0);

    for (const auto &v: vec) {
        auto new_vel = vel;
        new_vel.x += v.x;
        new_vel.y += v.y;

        int result = test(loc, new_vel, center_rad, field_rad, max_turn);
        if (best < result) {
            best = result;
            best_move = v;
        }

        auto next_param = simulate(loc, new_vel);

        auto dfs_result = dfs(depth + 1, next_param.first, next_param.second, vec, center_rad, field_rad, max_turn);
        if (best < dfs_result.first) {
            best = dfs_result.first;
            best_move = dfs_result.second;
        }
    }

    return make_pair(best, best_move);
}

Vector safe_move(long long planet_size, const Vector &loc, const Vector &vel, int field_size, int remaining_turn) {
    vector<vector<Vector>> vecs = {
            {Vector(0, 0), Vector(0, 1),  Vector(1, 0),  Vector(1, 1)},
            {Vector(0, 0), Vector(0, 1),  Vector(-1, 0), Vector(-1, 1)},
            {Vector(0, 0), Vector(0, -1), Vector(1, 0),  Vector(1, -1)},
            {Vector(0, 0), Vector(0, -1), Vector(-1, 0), Vector(-1, -1)}
    };

    int best = -1;
    Vector best_move = Vector(0, 0);

    for (const auto &vec: vecs) {
        auto result = dfs(0, loc, vel, vec, planet_size, field_size, remaining_turn);
        if (best < result.first) {
            best = result.first;
            best_move = result.second;
        }
    }

    return best_move;
}

void test_safe_move() {
    Vector loc(-4, 48), vel(0, 0);

    int planet = 30;
    int field_rad = 80;
    for (int i = 0; i < 256; ++i) {
        // simulate
        auto vec = safe_move(planet, loc, vel, field_rad, 256);

        vel.x += vec.x;
        vel.y += vec.y;
        auto new_state = simulate(loc, vel);
        loc = new_state.first;
        vel = new_state.second;

        cerr << "loc:" << loc << " vel:" << vel << " " << test(loc, vel, planet, field_rad, 256, false) << endl;
    }
}

