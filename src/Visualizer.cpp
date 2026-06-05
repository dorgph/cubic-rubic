#include "Visualizer.h"

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
#endif

int Visualizer::colorCode(int faceIndex) {
    static const int codes[6] = { 15, 196, 46, 226, 208, 21 };
    return (faceIndex < 0 || faceIndex > 5) ? 0 : codes[faceIndex];
}

std::string Visualizer::block(int faceIndex) {
    return "\033[48;5;" + std::to_string(colorCode(faceIndex)) + "m  \033[0m";
}

void Visualizer::enableAnsi() {
#ifdef _WIN32
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode))
        SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
}

void Visualizer::hideCursor() {
    std::cout << "\033[?25l";
}

void Visualizer::showCursor() {
    std::cout << "\033[?25h";
}

void Visualizer::clearScreen() {
    std::cout << "\033[2J\033[H";
}

std::string Visualizer::buildNet(const RubikCube& cube, const std::string& header) {
    const State& s = cube.getStateArray();
    auto col = [&](int base, int k) {
        return s[base + k] / 9;
    };

    std::ostringstream out;
    auto line = [&](const std::string& content) {
        out << content << "\033[K\n";   
    };

    line(header);
    line("");

    const std::string pad = "       ";
    for (int r = 0; r < 3; ++r) {
        std::string l = pad;
        for (int c = 0; c < 3; ++c) {
            l += block(col(0, r * 3 + c));
        }
        line(l);
    }
    for (int r = 0; r < 3; ++r) {
        std::string l;
        for (int c = 0; c < 3; ++c) {
            l += block(col(36, r * 3 + c));
        }

        l += " ";
        for (int c = 0; c < 3; ++c) {
            l += block(col(18, r * 3 + c));
        }

        l += " ";
        for (int c = 0; c < 3; ++c) {
            l += block(col(9,  r * 3 + c));
        }

        l += " ";
        for (int c = 0; c < 3; ++c) {
            l += block(col(45, r * 3 + c));
        }

        line(l);
    }

    for (int r = 0; r < 3; ++r) {
        std::string l = pad;
        for (int c = 0; c < 3; ++c) {
            l += block(col(27, r * 3 + c));
        }
        line(l);
    }

    return out.str();
}

std::string Visualizer::buildCube3D(const RubikCube& cube, const std::string& header) {
    const State& s = cube.getStateArray();

    const int W = 30, H = 30;
    std::vector<std::vector<int>> canvas(H, std::vector<int>(W, -1));

    const double axx = 4, axy = 2;
    const double azx = 4, azy = -2;
    const double ayx = 0, ayy = 4;
    const double baseX = 2, baseY = 8;

    auto fill = [&](char which, double Ox, double Oy, double Ax, double Ay, double Bx, double By) {
        double det = Ax * By - Ay * Bx;

        for (int py = 0; py < H; ++py)
            for (int px = 0; px < W; ++px) {
                double rx = px - Ox, ry = py - Oy;
                double u = (By * rx - Bx * ry) / det;
                double v = (-Ay * rx + Ax * ry) / det;
                if (u < 0 || u >= 3 || v < 0 || v >= 3) {
                    continue;
                }

                int cu = (int)u, cv = (int)v, base, k;
                if (which == 'U') {
                    base = 0;
                    k = (2 - cv) * 3 + cu;
                } else if (which == 'R') {
                    base = 9;
                    k = cv * 3 + cu;
                } else {
                    base = 18;
                    k = cv * 3 + cu;
                }

                canvas[py][px] = s[base + k] / 9;
            }
    };

    fill('U', baseX,           baseY,           axx, axy, azx, azy);
    fill('R', baseX + 3 * axx, baseY + 3 * axy, azx, azy, ayx, ayy);
    fill('F', baseX,           baseY,           axx, axy, ayx, ayy);

    std::ostringstream out;
    out << header << "\033[K\n" << "\033[K\n";

    for (int py = 0; py < H; ++py) {
        std::string l;
        int cur = -2;
        bool any = false;
        for (int px = 0; px < W; ++px) {
            int c = canvas[py][px];
            if (c != cur) {
                l += (c < 0) ? std::string("\033[0m")
                             : "\033[48;5;" + std::to_string(colorCode(c)) + "m";
                cur = c;
            }

            if (c >= 0) {
                any = true;
            }
            l += ' ';
        }

        l += "\033[0m";
        if (any) {
            out << l << "\033[K\n";
        }
    }
    return out.str();
}

void Visualizer::present(const std::string& frame) {
    std::cout << "\033[H" << frame << "\033[J" << std::flush;
}

void Visualizer::animate(RubikCube& cube, const std::vector<std::string>& moves, int delayMs, bool threeD, const std::string& title) {
    auto frame = [&](const std::string& moveLabel) {
        std::string header = title + (title.empty() ? "" : "   ") + "Ход: " + (moveLabel.empty() ? "—" : moveLabel);
        std::string buf = threeD ? buildCube3D(cube, header) : buildNet(cube, header);
        present(buf);
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    };

    frame("");
    for (const auto& m : moves) {
        cube.applyMove(m);
        frame(m);
    }
}