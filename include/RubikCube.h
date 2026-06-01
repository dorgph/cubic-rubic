#pragma once

#include "Common.h"
#include "Constants.h"

class RubikCube {
public:
    explicit RubikCube();
    ~RubikCube() = default;

    void applyMove(const std::string& name);
    void applySequence(const std::vector<std::string>& moves);
    void applyConjugation(const std::vector<std::string>& initial, const std::vector<std::string>& original);
    void applyCommutator(const std::vector<std::string>& first, const std::vector<std::string>& second);

    [[nodiscard]] bool solved() const;
    [[nodiscard]] Cycles getCycles() const;
    [[nodiscard]] const State& getStateArray() const;
private:
    State state;
    Moves table;
    bool initialized;

    void initMoveTable();
    void generateDerivedMoves();
    std::vector<std::string> invert(const std::vector<std::string>& sequence);
};