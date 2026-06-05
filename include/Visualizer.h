#pragma once

#include "RubikCube.h"
#include "Constants.h"
#include <string>
#include <vector>

class Visualizer {
public:
    static void enableAnsi();        
    static void hideCursor();
    static void showCursor();
    static void clearScreen();
    static std::string buildNet(const RubikCube& cube, const std::string& header);
    static std::string buildCube3D(const RubikCube& cube, const std::string& header);
    static void present(const std::string& frame);
    static void animate(RubikCube& cube,
                        const std::vector<std::string>& moves,
                        int delayMs,
                        bool threeD,
                        const std::string& title = "");
private:
    static int  colorCode(int faceIndex);
    static std::string block(int faceIndex);
};
