#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <array>
#include <vector>
#include <string>
#include "Cube.h"

class Algorithm {
public:

    using State = std::array<int, 54>;

    using Move = std::string;

    using Sequence = std::vector<Move>;

    using Cycle = std::vector<int>;
    using Cycles = std::vector<Cycle>;

    explicit Algorithm(Cube& cube);
    ~Algorithm() = default;

    [[nodiscard]] Sequence solve();

    [[nodiscard]] Cycles analyzeCycles() const;

    [[nodiscard]] Sequence cycleToSequence(const Cycle& cycle) const;

    [[nodiscard]] static Sequence optimizeSequence(const Sequence& seq);

    [[nodiscard]] const State& getTargetState() const { return targetState; }
    [[nodiscard]] const std::unordered_map<std::string, State>& getBaseMoves() const { return baseMoves; }
    [[nodiscard]] const std::unordered_map<std::string, Sequence>& getMacros() const { return macros; }


private:
    Cube& cubeRef;

    const State targetState_;
    const std::unordered_map<std::string, State> baseMoves;
    const std::unordered_map<std::string, Sequence> macros;

    static State createTargetState();
    static std::unordered_map<std::string, State> createBaseMoves();
    static std::unordered_map<std::string, Sequence> createMacros();

    Sequence solveCross();
    Sequence solveF2L();
    Sequence solveOLL();
    Sequence solvePLL();
};

#endif 