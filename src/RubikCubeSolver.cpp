#include "RubikCubeSolver.h"

bool RubikCubeSolver::tables_built = false;
State RubikCubeSolver::base_moves[18];

std::vector<std::vector<int>> RubikCubeSolver::move_co;
std::vector<std::vector<int>> RubikCubeSolver::move_eo;
std::vector<std::vector<int>> RubikCubeSolver::move_slice;
std::vector<std::vector<int>> RubikCubeSolver::move_cp;
std::vector<std::vector<int>> RubikCubeSolver::move_ep_ud;
std::vector<std::vector<int>> RubikCubeSolver::move_ep_slice;

std::vector<uint8_t> RubikCubeSolver::pdb_co_slice;
std::vector<uint8_t> RubikCubeSolver::pdb_eo_slice;
std::vector<uint8_t> RubikCubeSolver::pdb_cp_slice;
std::vector<uint8_t> RubikCubeSolver::pdb_ep_ud_slice;

int RubikCubeSolver::C_arr[13][13];
int RubikCubeSolver::fact[9] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320};

int RubikCubeSolver::sticker_to_corner[CUBE_SIZE];
int RubikCubeSolver::pos_to_ori[CUBE_SIZE];
int RubikCubeSolver::sticker_to_edge[CUBE_SIZE];
int RubikCubeSolver::pos_to_eori[CUBE_SIZE];
int RubikCubeSolver::pos_to_edge[CUBE_SIZE];

static const int corner_stickers[8][3] = {
    {0, 47, 36}, {2, 11, 45}, {8, 9, 20}, {6, 18, 38},
    {27, 44, 24}, {29, 26, 15}, {35, 51, 17}, {33, 42, 53}
};
static const int edge_stickers[12][2] = {
    {1, 46}, {5, 10}, {7, 19}, {3, 37},
    {23, 12}, {21, 41}, {50, 39}, {48, 14},
    {28, 25}, {32, 16}, {34, 52}, {30, 43}
};
static const std::string move_names[18] = {
    "U", "U2", "U'", "D", "D2", "D'",
    "L", "L2", "L'", "R", "R2", "R'",
    "F", "F2", "F'", "B", "B2", "B'"
};
static const int p2_moves[10] = {0, 1, 2, 3, 4, 5, 7, 10, 13, 16};
static const int p2_inv_idx[10] = {2, 1, 0, 5, 4, 3, 6, 7, 8, 9};

RubikCubeSolver::RubikCubeSolver(const RubikCube& initialCube) : currentCube(initialCube) {
    if (!tables_built) {
        build_tables();
        tables_built = true;
    }
}

int RubikCubeSolver::get_perm_rank(const std::vector<int>& perm) {
    int rank = 0, n = perm.size();
    for(int i = 0; i < n - 1; ++i) {
        int smaller = 0;
        for(int j = i + 1; j < n; ++j) if(perm[j] < perm[i]) smaller++;
        rank += smaller * fact[n - 1 - i];
    }
    return rank;
}

int RubikCubeSolver::get_co(const State& s) {
    int co = 0;
    for(int c = 0; c < 7; ++c) {
        int ori = 0;
        for(int i = 0; i < 3; ++i) {
            if (pos_to_ori[s[corner_stickers[c][i]]] == 0) {
                ori = i;
                break;
            }
        }
        co = co * 3 + ori;
    }
    return co;
}

int RubikCubeSolver::get_eo(const State& s) {
    int eo = 0;
    for(int e = 0; e < 11; ++e) {
        int ori = 0;
        for(int i = 0; i < 2; ++i) {
            if (pos_to_eori[s[edge_stickers[e][i]]] == 0) {
                ori = i;
                break;
            }
        }
        eo = eo * 2 + ori;
    }
    return eo;
}

int RubikCubeSolver::get_slice(const State& s) {
    std::vector<int> pos(4);
    for(int i = 0; i < 4; ++i) {
        int orig = edge_stickers[i + 4][0], p = -1;
        for(int j = 0; j < CUBE_SIZE; ++j) if(s[j] == orig) { p = j; break; }
        pos[i] = pos_to_edge[p];
    }
    std::sort(pos.begin(), pos.end());
    return C_arr[pos[0]][1] + C_arr[pos[1]][2] + C_arr[pos[2]][3] + C_arr[pos[3]][4];
}

int RubikCubeSolver::get_cp(const State& s) {
    std::vector<int> perm(8);
    for(int c = 0; c < 8; ++c) perm[c] = sticker_to_corner[s[corner_stickers[c][0]]];
    return get_perm_rank(perm);
}

int RubikCubeSolver::get_ep_ud(const State& s) {
    std::vector<int> perm(8);
    int ud_edges[8] = {0, 1, 2, 3, 8, 9, 10, 11};
    for(int i = 0; i < 8; ++i) {
        int orig_e = sticker_to_edge[s[edge_stickers[ud_edges[i]][0]]], mapped = 0;
        for(int j = 0; j < 8; ++j) if(ud_edges[j] == orig_e) { mapped = j; break; }
        perm[i] = mapped;
    }
    return get_perm_rank(perm);
}

int RubikCubeSolver::get_ep_slice(const State& s) {
    std::vector<int> perm(4);
    for(int i = 0; i < 4; ++i) perm[i] = sticker_to_edge[s[edge_stickers[i + 4][0]]] - 4;
    return get_perm_rank(perm);
}

State RubikCubeSolver::apply_move(const State& s, int m) const {
    State next;
    for(int i = 0; i < CUBE_SIZE; ++i) next[i] = s[base_moves[m][i]];
    return next;
}

void RubikCubeSolver::build_tables() {
    RubikCube dummy;
    State solved = dummy.getStateArray();

    for (int i = 0; i < 18; ++i) {
        RubikCube temp;
        temp.applyMove(move_names[i]);
        base_moves[i] = temp.getStateArray();
    }

    std::fill(sticker_to_corner, sticker_to_corner + CUBE_SIZE, -1);
    std::fill(pos_to_ori, pos_to_ori + CUBE_SIZE, -1);
    for(int c = 0; c < 8; ++c) {
        for(int i = 0; i < 3; ++i) {
            sticker_to_corner[corner_stickers[c][i]] = c;
            pos_to_ori[corner_stickers[c][i]] = i;
        }
    }

    std::fill(sticker_to_edge, sticker_to_edge + CUBE_SIZE, -1);
    std::fill(pos_to_eori, pos_to_eori + CUBE_SIZE, -1);
    std::fill(pos_to_edge, pos_to_edge + CUBE_SIZE, -1);
    for(int e = 0; e < 12; ++e) {
        for(int i = 0; i < 2; ++i) {
            sticker_to_edge[edge_stickers[e][i]] = e;
            pos_to_eori[edge_stickers[e][i]] = i;
            pos_to_edge[edge_stickers[e][i]] = e;
        }
    }

    for(int i = 0; i <= 12; ++i) {
        C_arr[i][0] = 1;
        for(int j = 1; j <= i; ++j) C_arr[i][j] = C_arr[i-1][j-1] + C_arr[i-1][j];
    }

    move_co.assign(2187, std::vector<int>(18, -1)); move_eo.assign(2048, std::vector<int>(18, -1)); move_slice.assign(495, std::vector<int>(18, -1));
    move_cp.assign(40320, std::vector<int>(10, -1)); move_ep_ud.assign(40320, std::vector<int>(10, -1)); move_ep_slice.assign(24, std::vector<int>(10, -1));

    auto bfs_p1 = [&](auto& table, auto get_coord, int size) {
        std::vector<bool> vis(size, false);
        std::queue<State> q;
        q.push(solved); vis[get_coord(solved)] = true;
        while(!q.empty()) {
            State s = q.front(); q.pop();
            int c = get_coord(s);
            for(int m = 0; m < 18; ++m) {
                State next_s = apply_move(s, m);
                int next_c = get_coord(next_s);
                table[c][m] = next_c;
                if(!vis[next_c]) { vis[next_c] = true; q.push(next_s); }
            }
        }
    };
    bfs_p1(move_co, get_co, 2187); bfs_p1(move_eo, get_eo, 2048); bfs_p1(move_slice, get_slice, 495);

    auto bfs_p2 = [&](auto& table, auto get_coord, int size) {
        std::vector<bool> vis(size, false);
        std::queue<State> q;
        q.push(solved); vis[get_coord(solved)] = true;
        while(!q.empty()) {
            State s = q.front(); q.pop();
            int c = get_coord(s);
            for(int idx = 0; idx < 10; ++idx) {
                State next_s = apply_move(s, p2_moves[idx]);
                int next_c = get_coord(next_s);
                table[c][idx] = next_c;
                if(!vis[next_c]) { vis[next_c] = true; q.push(next_s); }
            }
        }
    };
    bfs_p2(move_cp, get_cp, 40320); bfs_p2(move_ep_ud, get_ep_ud, 40320); bfs_p2(move_ep_slice, get_ep_slice, 24);

    pdb_co_slice.assign(2187 * 495, 255); pdb_eo_slice.assign(2048 * 495, 255);
    int goal_co = get_co(solved), goal_eo = get_eo(solved), goal_slice = get_slice(solved);

    auto build_pdb_p1 = [&](auto& pdb, const auto& table, int goal, int states) {
        std::queue<std::pair<int, int>> q;
        pdb[goal * 495 + goal_slice] = 0;
        q.push({goal, goal_slice});
        while(!q.empty()) {
            auto [c, s] = q.front(); q.pop();
            uint8_t d = pdb[c * 495 + s];
            for(int m = 0; m < 18; ++m) {
                int inv_m = (m / 3) * 3 + (m % 3 == 0 ? 2 : (m % 3 == 2 ? 0 : 1));
                int prev_c = table[c][inv_m], prev_s = move_slice[s][inv_m];
                if(pdb[prev_c * 495 + prev_s] == 255) { pdb[prev_c * 495 + prev_s] = d + 1; q.push({prev_c, prev_s}); }
            }
        }
    };
    build_pdb_p1(pdb_co_slice, move_co, goal_co, 2187); build_pdb_p1(pdb_eo_slice, move_eo, goal_eo, 2048);

    pdb_cp_slice.assign(40320 * 24, 255); pdb_ep_ud_slice.assign(40320 * 24, 255);
    int goal_cp = get_cp(solved), goal_ep_ud = get_ep_ud(solved), goal_ep_slice = get_ep_slice(solved);

    auto build_pdb_p2 = [&](auto& pdb, const auto& table, int goal) {
        std::queue<std::pair<int, int>> q;
        pdb[goal * 24 + goal_ep_slice] = 0;
        q.push({goal, goal_ep_slice});
        while(!q.empty()) {
            auto [c, s] = q.front(); q.pop();
            uint8_t d = pdb[c * 24 + s];
            for(int i = 0; i < 10; ++i) {
                int inv_i = p2_inv_idx[i], prev_c = table[c][inv_i], prev_s = move_ep_slice[s][inv_i];
                if(pdb[prev_c * 24 + prev_s] == 255) { pdb[prev_c * 24 + prev_s] = d + 1; q.push({prev_c, prev_s}); }
            }
        }
    };
    build_pdb_p2(pdb_cp_slice, move_cp, goal_cp); build_pdb_p2(pdb_ep_ud_slice, move_ep_ud, goal_ep_ud);
}

int RubikCubeSolver::search_p1(int co, int eo, int slice, int g, int bound, int last_face, std::vector<int>& path) {
    int h = std::max(pdb_co_slice[co * 495 + slice], pdb_eo_slice[eo * 495 + slice]);
    if (g + h > bound) return g + h;
    if (g + h >= min_total_length) return 1e9;

    int min_f = 1e9;

    if (h == 0) {
        State state = currentCube.getStateArray();
        for (int m : path) state = apply_move(state, m);

        int cp = get_cp(state), ep_ud = get_ep_ud(state), ep_slice = get_ep_slice(state);
        int bound2 = std::max(pdb_cp_slice[cp * 24 + ep_slice], pdb_ep_ud_slice[ep_ud * 24 + ep_slice]);

        while (bound2 < min_total_length - g) {
            p2_path.clear();
            int res2 = search_p2(cp, ep_ud, ep_slice, 0, bound2, last_face, p2_path);
            if (res2 == -1) {
                best_solution = path;
                best_solution.insert(best_solution.end(), p2_path.begin(), p2_path.end());
                min_total_length = g + bound2;
                if (min_total_length <= 22) return -2;
                break;
            }
            bound2 = res2;
        }

    }

    for (int m = 0; m < 18; ++m) {
        int face = m / 3;
        if (face == last_face) continue;
        if ((face ^ 1) == last_face && face > last_face) continue;

        if (h == 0) {
            if (face == 0 || face == 1) continue;
            if (m == 7 || m == 10 || m == 13 || m == 16) continue;
        }

        path.push_back(m);
        int res = search_p1(move_co[co][m], move_eo[eo][m], move_slice[slice][m], g + 1, bound, face, path);
        if (res == -2) return -2;
        min_f = std::min(min_f, res);
        path.pop_back();
    }
    return min_f;
}

int RubikCubeSolver::search_p2(int cp, int ep_ud, int ep_slice, int g, int bound, int last_face, std::vector<int>& path) {
    int h = std::max(pdb_cp_slice[cp * 24 + ep_slice], pdb_ep_ud_slice[ep_ud * 24 + ep_slice]);
    if (g + h > bound) return g + h;
    if (h == 0) return -1;

    int min_f = 1e9;
    for (int idx = 0; idx < 10; ++idx) {
        int m = p2_moves[idx], face = m / 3;
        if (face == last_face) continue;
        if ((face ^ 1) == last_face && face > last_face) continue;

        path.push_back(m);
        int res = search_p2(move_cp[cp][idx], move_ep_ud[ep_ud][idx], move_ep_slice[ep_slice][idx], g + 1, bound, face, path);
        if (res == -1) return -1;
        min_f = std::min(min_f, res);
        path.pop_back();
    }
    return min_f;
}

std::vector<std::string> RubikCubeSolver::optimizeMoves(const std::vector<std::string>& moves) const {
    std::vector<std::string> stack;
    for (const auto& move : moves) {
        if (move.empty() || move == "|") continue;
        if (!stack.empty()) {
            std::string top = stack.back();
            if (top[0] == move[0]) {
                stack.pop_back();
                int t_rot = (top.length() == 1) ? 1 : (top[1] == '\'' ? 3 : 2);
                int m_rot = (move.length() == 1) ? 1 : (move[1] == '\'' ? 3 : 2);
                int sum = (t_rot + m_rot) % 4;
                if (sum == 1) stack.push_back(std::string(1, top[0]));
                else if (sum == 2) stack.push_back(std::string(1, top[0]) + "2");
                else if (sum == 3) stack.push_back(std::string(1, top[0]) + "'");
                continue;
            }
        }
        stack.push_back(move);
    }
    return stack;
}

std::vector<std::string> RubikCubeSolver::solve() {
    if (currentCube.solved()) return {};

    State state = currentCube.getStateArray();
    int co = get_co(state), eo = get_eo(state), slice = get_slice(state);

    std::vector<int> p1_path;
    best_solution.clear();
    min_total_length = 24;

    int bound = std::max(pdb_co_slice[co * 495 + slice], pdb_eo_slice[eo * 495 + slice]);

    while (bound < min_total_length) {
        int res = search_p1(co, eo, slice, 0, bound, -1, p1_path);
        if (res == -2 || res >= 1e9) break;
        bound = res;
    }

    std::vector<std::string> solution_str;
    for (int m : best_solution) solution_str.push_back(move_names[m]);

    return optimizeMoves(solution_str);
}