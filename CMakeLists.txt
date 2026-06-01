cmake_minimum_required(VERSION 4.2)
project(cubic_rubic)

set(CMAKE_CXX_STANDARD 20)

add_executable(cubic_rubic main.cpp
        include/RubikCube/RubikCube.h
        include/RubikCubeSolver/RubikCubeSolver.h
        src/RubikCubeSolver/RubikCubeSolver.cpp
        src/RubikCube/RubikCube.cpp
        utils/Common.h
        utils/Constants.h
        tests/TestDriver.h)

target_include_directories(cubic_rubic PRIVATE utils include)