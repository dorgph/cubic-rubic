#pragma once

#include "Common.h"
#include "RubikCubeSolver/RubikCubeSolver.h"

class TestDriver {
public:
    explicit TestDriver() = default;

    void init() {
        std::cout << "Добро пожаловать в компьютерную сборку Кубика Рубика!\n\n";
        std::cout << "Пожалуйста, выберите интересующие Вас тесты:\n\n";
        std::cout << "[1]: Простые комбинации кубика Рубика;\n";
        std::cout << "[2]: Более продвинутые комбинации кубика Рубика;\n";
        std::cout << "[3]: Сложные комбинации кубика Рубика;\n";
        std::cout << "[0]: Остановить сборку кубика;\n";

        while (true) {
            std::cout << GREEN << "> " << RESET;

            std::string option;
            std::getline(std::cin, option);

            if (option == "1") {
                runEasyTests();
            } else if (option == "2") {
                runMediumTests();
            } else if (option == "3") {
                runHardTests();
            } else if (option == "0") {
                std::cout << "До скорой встречи!\n";
                break;
            }
            else {
                raiseError();
            }
        }
    }
private:
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string RESET = "\033[0m";

    void printSequence(const std::vector<std::string>& sequence) {
        if (sequence.empty()) {
            std::cout << "(Пусто)\n";
            return;
        }

        for (const auto& item : sequence)
            std::cout << item << " ";
        std::cout << "\n";
    }

    void run(const std::string& name, const std::vector<std::string>& scramble) {
        std::cout << BLUE << "===============================================\n" << RESET;
        std::cout << YELLOW << "[ТЕСТ] " << name << "\n";

        RubikCube cube;

        if (!scramble.empty()) {
            std::cout << "Текущая последовательность: ";
            printSequence(scramble);
            cube.applySequence(scramble);
        }

        RubikCubeSolver solver(cube);

        auto start = std::chrono::high_resolution_clock::now();
        const auto solution = solver.solve();
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "Решение (" << solution.size() << " ходов): ";
        printSequence(solution);
        std::cout << "Время сборки: " << duration.count() << " мс\n";

        cube.applySequence(solution);
        if (cube.solved()) {
            std::cout << GREEN << "[РЕЗУЛЬТАТ] УСПЕХ: Кубик собран!\n";
        } else {
            std::cerr << RED << "[РЕЗУЛЬТАТ] ОШИБКА: Кубик остался разобранным.\n";
        }
        std::cout << RESET << BLUE << "===============================================\n" << RESET;
    }

    void runEasyTests() {
        std::cout << GREEN << "Простые тесты" << "\n\n" << RESET;

        run("Собранный кубик", {

        });
        run("Один ход", {
            "R"
        });
        run("Пиф-Паф", {
            "R", "U", "R'", "U'"
        });
        run("Конец F2L", {
            "R", "U", "R'", "U'", "R", "U", "R'"
        });
        run("Кувалда", {
            "R'", "F", "R", "F'"
        });
        run("OLL Палка", {
            "F", "R", "U", "R'", "U'", "F'"
        });
    }

    void runMediumTests() {
        std::cout << YELLOW << "Более продвинутые тесты" << "\n\n" << RESET;

        run("3-цикл (U-Perm)", {
            "R", "U'", "R", "U", "R", "U", "R", "U'", "R'", "U'", "R2"
        });
        run("Путаница в 10 ходов", {
            "F", "U", "R", "U'", "R'", "F'", "U", "R", "U'", "R'"
        });
        run("T-Perm", {
            "R", "U", "R'", "U'", "R'", "F", "R2", "U'",
            "R'", "U'", "R", "U", "R'", "F'"
        });
        run("Путаница в 14 ходов", {
            "D2", "L2", "B2", "R2", "U", "L2", "U'", "R2",
            "B2", "D'", "B", "L", "U", "R"
        });
    }

    void runHardTests() {
        std::cout << RED << "Сложные тесты" << "\n\n" << RESET;

        run("Случайная путаница", {
            "F2", "R'", "B'", "U", "R'", "L", "F'", "L", "F'", "B", "D'", "R",
            "B", "L2", "U2", "B2", "U2", "R2", "D2", "U'"
        });
        run("WCA Scramble 1", {
            "U2", "F2", "R2", "D'", "B2", "D", "R2", "F2", "U", "L2",
            "U'", "B'", "L", "U", "R'", "D'", "L2", "F'", "D", "R2"
        });
        run("WCA Scramble 2", {
            "D'", "B2", "D2", "L2", "D'", "R2", "F2", "U'", "L2", "U2",
            "B'", "D'", "R", "U'", "L", "B2", "U'", "B", "L'", "F'"
        });
        run("WCA Scramble 3", {
            "R2", "U2", "R2", "D", "R2", "D'", "R2", "F2", "L2", "U'",
            "L2", "B", "R'", "F", "L", "U", "B'", "L'", "D", "R'"
        });
        run("WCA Scramble 4", {
            "F2", "R2", "B2", "L2", "U2", "B", "L2", "B2", "U2", "F'",
            "U'", "R'", "D2", "F", "U'", "B'", "D'", "R'", "F", "U2"
        });
        run("WCA Scramble 5", {
            "D2", "B2", "L", "B2", "L2", "U2", "F2", "L'", "B2", "R'",
            "B2", "D'", "F'", "L'", "D", "R2", "F", "D", "B", "U'", "L'"
        });
        run("WCA Scramble 6", {
            "U'", "R2", "U'", "F2", "D'", "R2", "F2", "U'", "L2", "D",
            "B2", "L'", "F", "L2", "R", "B'", "U'", "L", "U", "B'", "L'"
        });
        run("WCA Scramble 7", {
            "L2", "D2", "B2", "U", "L2", "R2", "U'", "B2", "D", "B2",
            "U'", "L'", "D'", "B", "D", "R", "U'", "B'", "R2", "B", "F"
        });
        run("WCA Scramble 8", {
            "F'", "U2", "R2", "D2", "B2", "L", "D2", "B2", "L'", "R2",
            "U2", "B'", "D", "U2", "L", "B'", "D'", "R'", "U'", "F2"
        });
    }

    void raiseError() {
        const auto message = "Похоже, Вы ввели то, чего не существует в нашей Вселенной! Пожалуйста, попробуйте еще раз!\n";
        std::cout << RED << message << RESET;
    }
};
