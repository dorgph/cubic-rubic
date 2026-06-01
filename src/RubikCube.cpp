#include "RubikCube.h"

RubikCube::RubikCube() : initialized(false) {
    for (auto i = 0; i < CUBE_SIZE; ++i) {
        state[i] = i;
    }

    initMoveTable();
}

void RubikCube::initMoveTable() {
    if (initialized) {
        return;
    }

    State U_moves{}, L_moves{}, F_moves{}, R_moves{}, B_moves{}, D_moves{};
    for (int i = 0; i < CUBE_SIZE; ++i) {
        U_moves[i] = L_moves[i] = F_moves[i] = R_moves[i] = B_moves[i] = D_moves[i] = i;
    }

    U_moves[0]=6; U_moves[1]=3; U_moves[2]=0; U_moves[3]=7; U_moves[5]=1; U_moves[6]=8; U_moves[7]=5; U_moves[8]=2;
    U_moves[9]=45; U_moves[10]=46; U_moves[11]=47;
    U_moves[18]=9; U_moves[19]=10; U_moves[20]=11;
    U_moves[36]=18; U_moves[37]=19; U_moves[38]=20;
    U_moves[45]=36; U_moves[46]=37; U_moves[47]=38;

    D_moves[27]=33; D_moves[28]=30; D_moves[29]=27; D_moves[30]=34; D_moves[32]=28; D_moves[33]=35; D_moves[34]=32; D_moves[35]=29;
    D_moves[15]=24; D_moves[16]=25; D_moves[17]=26;
    D_moves[51]=15; D_moves[52]=16; D_moves[53]=17;
    D_moves[42]=51; D_moves[43]=52; D_moves[44]=53;
    D_moves[24]=42; D_moves[25]=43; D_moves[26]=44;

    R_moves[9]=15; R_moves[10]=12; R_moves[11]=9; R_moves[12]=16; R_moves[14]=10; R_moves[15]=17; R_moves[16]=14; R_moves[17]=11;
    R_moves[45]=8; R_moves[48]=5; R_moves[51]=2;
    R_moves[35]=45; R_moves[32]=48; R_moves[29]=51;
    R_moves[26]=35; R_moves[23]=32; R_moves[20]=29;
    R_moves[8]=26; R_moves[5]=23; R_moves[2]=20;

    L_moves[36]=42; L_moves[37]=39; L_moves[38]=36; L_moves[39]=43; L_moves[41]=37; L_moves[42]=44; L_moves[43]=41; L_moves[44]=38;
    L_moves[18]=0; L_moves[21]=3; L_moves[24]=6;
    L_moves[27]=18; L_moves[30]=21; L_moves[33]=24;
    L_moves[53]=27; L_moves[50]=30; L_moves[47]=33;
    L_moves[0]=53; L_moves[3]=50; L_moves[6]=47;

    F_moves[18]=24; F_moves[19]=21; F_moves[20]=18; F_moves[21]=25; F_moves[23]=19; F_moves[24]=26; F_moves[25]=23; F_moves[26]=20;
    F_moves[9]=6; F_moves[12]=7; F_moves[15]=8;
    F_moves[29]=9; F_moves[28]=12; F_moves[27]=15;
    F_moves[44]=29; F_moves[41]=28; F_moves[38]=27;
    F_moves[6]=44; F_moves[7]=41; F_moves[8]=38;

    B_moves[45]=51; B_moves[46]=48; B_moves[47]=45; B_moves[48]=52; B_moves[50]=46; B_moves[51]=53; B_moves[52]=50; B_moves[53]=47;
    B_moves[36]=2; B_moves[39]=1; B_moves[42]=0;
    B_moves[33]=36; B_moves[34]=39; B_moves[35]=42;
    B_moves[17]=33; B_moves[14]=34; B_moves[11]=35;
    B_moves[2]=17; B_moves[1]=14; B_moves[0]=11;

    table["U"] = U_moves;
    table["L"] = L_moves;
    table["F"] = F_moves;
    table["R"] = R_moves;
    table["B"] = B_moves;
    table["D"] = D_moves;

    generateDerivedMoves();
    initialized = true;
}

void RubikCube::generateDerivedMoves() {
    const auto moves = std::vector<std::string>{ "U", "D", "L", "R", "F", "B" };

    for (const auto& move : moves) {
        if (!table.contains(move)) {
            continue;
        }

        const auto& base = table[move];
        State inverseP{  }, doubleP{  };

        for (auto i = 0; i < CUBE_SIZE; ++i) {
            inverseP[base[i]] = i;
        }
        table[move + "'"] = inverseP;

        for (auto i = 0; i < CUBE_SIZE; ++i) {
            doubleP[i] = base[base[i]];
        }
        table[move + "2"] = doubleP;
    }
}

std::vector<std::string> RubikCube::invert(const std::vector<std::string>& sequence) {
    std::vector<std::string> inverted;
    inverted.reserve(sequence.size());

    for (const auto& element : sequence) {
        if (element.ends_with("'")) {
            inverted.push_back(element.substr(0, element.length() - 1));
        } else if (element.ends_with("2")) {
            inverted.push_back(element);
        } else {
            inverted.push_back(element + "'");
        }
    }

    return inverted;
}

void RubikCube::applyMove(const std::string& name) {
    const auto it = table.find(name);
    if (it == table.end()) {
        // to write an error code here
    }

    const auto& P = it->second;
    State newStateArray{  };
    for (auto i = 0; i < CUBE_SIZE; ++i) {
        newStateArray[i] = state[P[i]];
    }
    state = newStateArray;
}

void RubikCube::applySequence(const std::vector<std::string>& moves) {
    for (const auto& move : moves) {
        applyMove(move);
    }
}

void RubikCube::applyConjugation(const std::vector<std::string>& initial, const std::vector<std::string>& original) {
    applySequence(initial);
    applySequence(original);
    applySequence(invert(initial));
}

void RubikCube::applyCommutator(const std::vector<std::string>& first, const std::vector<std::string>& second) {
    applySequence(first);
    applySequence(second);
    applySequence(invert(first));
    applySequence(invert(second));
}

bool RubikCube::solved() const {
    for (auto i = 0; i < CUBE_SIZE; ++i) {
        if (state[i] != i) {
            return false;
        }
    }
    return true;
}

Cycles RubikCube::getCycles() const {
    Cycles cycles;
    std::array<bool, CUBE_SIZE> visited{ false };

    for (auto i = 0; i < CUBE_SIZE; ++i) {
        if (visited[i] || state[i] == i) {
            continue;
        }

        Cycle current;

        auto idx = i;
        while (!visited[idx]) {
            visited[idx] = true;
            current.push_back(idx);
            idx = state[idx];
        }
        cycles.push_back(current);
    }
    return cycles;
}

const State& RubikCube::getStateArray() const {
    return state;
}