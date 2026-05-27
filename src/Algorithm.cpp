#include "Algorithm.h"
#include <algorithm>
#include <iostream>


Algorithm::Algorithm(Cube& cube)
    : cubeRef(cube),
      targetState(createTargetState()),
      baseMoves(createBaseMoves()),
      macros(createMacros()) {}

State Algorithm::createTargetState() {
    State s{};
    for (int i = 0; i < 54; ++i) s[i] = i;
    return s;
}

std::unordered_map<std::string, State> Algorithm::createBaseMoves() {
    std::unordered_map<std::string, State> map;
    constexpr int N = 54;

    auto registerMove = [&](const std::string& name, const std::array<int, N>& base) {
        map[name] = base;

        State inv{};
        for (int i = 0; i < N; ++i) inv[base[i]] = i;
        map[name + "'"] = inv;

        State dbl{};
        for (int i = 0; i < N; ++i) dbl[i] = base[base[i]];
        map[name + "2"] = dbl;
    };

    auto initIdentity = [&] { State s{}; std::iota(s.begin(), s.end(), 0); return s; };
    
    State U = initIdentity(), L = initIdentity(), F = initIdentity();
    State R = initIdentity(), B = initIdentity(), D = initIdentity();

    auto applyPerm = [](State& s, std::initializer_list<std::pair<int,int>> p) {
        for(auto [from, to] : p) s[from] = to;
    };

    applyPerm(U, { {0,6},{1,3},{2,0},{3,7},{5,1},{6,8},{7,5},{8,2},
                   {9,45},{10,46},{11,47},{18,9},{19,10},{20,11},
                   {36,18},{37,19},{38,20},{45,36},{46,37},{47,38} });
    applyPerm(R, { {2,20},{5,23},{8,26},{9,15},{10,12},{11,9},{12,16},{14,10},
                   {15,17},{16,14},{17,11},{20,29},{23,32},{26,35},{29,45},
                   {32,48},{35,51},{45,8},{48,5},{51,2} });
    applyPerm(F, { {6,44},{7,41},{8,38},{9,6},{12,7},{15,8},{18,24},{19,21},
                   {20,18},{21,25},{23,19},{24,26},{25,23},{26,20},{27,15},
                   {28,12},{29,9},{38,29},{41,28},{44,27} });
    applyPerm(D, { {15,24},{16,25},{17,26},{24,42},{25,43},{26,44},{27,33},
                   {28,30},{29,27},{30,34},{32,28},{33,35},{34,32},{35,29},
                   {42,51},{43,52},{44,53},{51,15},{52,16},{53,17} });
    applyPerm(L, { {0,53},{3,50},{6,47},{18,0},{21,3},{24,6},{27,18},{30,21},
                   {33,24},{36,42},{37,39},{38,36},{39,43},{41,37},{42,44},
                   {43,41},{44,38},{47,33},{50,30},{53,27} });
    applyPerm(B, { {0,11},{1,14},{2,17},{11,33},{14,34},{17,35},{33,42},{34,39},
                   {35,36},{36,2},{39,1},{42,0},{45,51},{46,48},{47,45},{48,52},
                   {50,46},{51,53},{52,50},{53,47} });

    registerMove("U", U); registerMove("R", R); registerMove("F", F);
    registerMove("D", D); registerMove("L", L); registerMove("B", B);

    return map;
}

std::unordered_map<std::string, Sequence> Algorithm::createMacros() {
    std::unordered_map<std::string, Sequence> m;

    m["A-Perm"] = {"R", "U'", "L", "U2", "R'", "U'", "R", "U2", "L2"};

    m["U-Perm"] = {"R2", "U", "R", "U", "R'", "U'", "R'", "U'", "R'", "U", "R'"};

    m["T-Perm"] = {"R", "U", "R'", "U'", "R'", "F", "R2", "U'", "R'", "U'", "R", "U", "R'", "F'"};

    return m;
}

Algorithm::Cycles Algorithm::analyzeCycles() const {
    return cubeRef.getCycles();
}

bool isSequenceEmpty(const Algorithm::Sequence& seq) { return seq.empty(); }

Algorithm::Sequence Algorithm::solve() {
    Sequence fullSolution;

    auto append = [&](Sequence&& phase) {
        if (!isSequenceEmpty(phase)) {
            cubeRef.applySequence(phase);
            fullSolution.insert(fullSolution.end(), phase.begin(), phase.end());
        }
    };

    if (!cubeRef.solved()) {
        append(solveCross());
        append(solveF2L());
        append(solveOLL());
        append(solvePLL());
    }
    return optimizeSequence(fullSolution);
}

Algorithm::Sequence Algorithm::cycleToSequence(const Cycle& cycle) const {
    if (cycle.size() < 3) return {};
    auto it = macros_.find("A-Perm");
    return (it != macros_.end()) ? it->second : Sequence{"R", "U", "R'", "U'"};
}

Algorithm::Sequence Algorithm::optimizeSequence(const Sequence& seq) {
    Sequence optimized;
    optimized.reserve(seq.size());

    for (size_t i = 0; i < seq.size(); ++i) {
        if (i + 1 < seq.size()) {
            const auto& cur = seq[i];
            const auto& nxt = seq[i + 1];
            if (cur[0] == nxt[0]) {
                bool isInv = (cur.ends_with("'") && !nxt.ends_with("'") && !nxt.ends_with("2")) ||
                             (!cur.ends_with("'") && !cur.ends_with("2") && nxt.ends_with("'"));
                if (isInv) { i++; continue; }
                
                bool isDbl = (cur.ends_with("2") && !nxt.ends_with("2") && !nxt.ends_with("'")) ||
                             (!cur.ends_with("2") && !cur.ends_with("'") && nxt.ends_with("2"));
                if (isDbl) { 
                    optimized.push_back(cur.substr(0,1) + "2"); 
                    i++; continue; 
                }
                if (!cur.ends_with("'") && !cur.ends_with("2") && !nxt.ends_with("'") && !nxt.ends_with("2")) {
                    optimized.push_back(cur + "2");
                    i++; continue;
                }
            }
        }
        optimized.push_back(seq[i]);
    }
    return optimized;
}

Algorithm::Sequence Algorithm::solveCross() { return {}; }
Algorithm::Sequence Algorithm::solveF2L()   { return {}; }
Algorithm::Sequence Algorithm::solveOLL()   { return {}; }
Algorithm::Sequence Algorithm::solvePLL()   { return {}; }