#pragma once

#include "Common.h"
#include "Constants.h"
#include "RubikCube.h"

class RubikCubeSolver {
public:
    explicit RubikCubeSolver(const RubikCube& initialCube);
    std::vector<std::string> solve();
private:
    RubikCube currentCube;

    std::vector<int> best_solution;
    std::vector<int> p2_path;
    int min_total_length;

    static bool tables_built;
    static State base_moves[18];

    static std::vector<std::vector<int>> move_co;
    static std::vector<std::vector<int>> move_eo;
    static std::vector<std::vector<int>> move_slice;
    static std::vector<std::vector<int>> move_cp;
    static std::vector<std::vector<int>> move_ep_ud;
    static std::vector<std::vector<int>> move_ep_slice;

    static std::vector<uint8_t> pdb_co_slice;
    static std::vector<uint8_t> pdb_eo_slice;
    static std::vector<uint8_t> pdb_cp_slice;
    static std::vector<uint8_t> pdb_ep_ud_slice;

    static int C_arr[13][13];
    static int fact[9];

    static int sticker_to_corner[CUBE_SIZE];
    static int pos_to_ori[CUBE_SIZE];
    static int sticker_to_edge[CUBE_SIZE];
    static int pos_to_eori[CUBE_SIZE];
    static int pos_to_edge[CUBE_SIZE];

    static int get_co(const State& s);
    static int get_eo(const State& s);
    static int get_slice(const State& s);
    static int get_cp(const State& s);
    static int get_ep_ud(const State& s);
    static int get_ep_slice(const State& s);
    static int get_perm_rank(const std::vector<int>& perm);
    void build_tables();
    [[nodiscard]] State apply_move(const State& s, int m) const;
    [[nodiscard]] std::vector<std::string> optimizeMoves(const std::vector<std::string>& moves) const;
    int search_p1(int co, int eo, int slice, int g, int bound, int last_face, std::vector<int>& path);
    int search_p2(int cp, int ep_ud, int ep_slice, int g, int bound, int last_face, std::vector<int>& path);
};