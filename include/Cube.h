#ifndef COURSE_WORK_PROJECT_CUBE_H
#define COURSE_WORK_PROJECT_CUBE_H

#include "common.h"
#include "constants.h"

class Cube {
public:
    explicit Cube();
    ~Cube() = default;

    void applySequence(const std::vector<std::string>& moves);
    void applyConjugation(const std::vector<std::string>& initial,
                          const std::vector<std::string>& original);
    void applyCommutator(const std::vector<std::string>& first,
                         const std::vector<std::string>& second);

    [[nodiscard]] bool solved() const;
    [[nodiscard]] Cycles getCycles() const;
    [[nodiscard]] const State& getStateArray() const;
private:
    State state;
    Moves table;
    bool initialized;

    void initMoveTable();
    void generateDerivedMoves();
    void applyMove(const std::string& name);
    std::vector<std::string> invert(const std::vector<std::string>& sequence);
};

#endif
