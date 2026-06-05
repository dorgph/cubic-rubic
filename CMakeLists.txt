cmake_minimum_required(VERSION 3.20)
project(cubic_rubic LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF) 

add_executable(cubic_rubic
        main.cpp
        src/RubikCube.cpp
        src/RubikCubeSolver.cpp
        src/Visualizer.cpp          
        include/RubikCube.h
        include/RubikCubeSolver.h
        include/Visualizer.h       
        utils/Common.h
        utils/Constants.h
        tests/TestDriver.h)

target_include_directories(cubic_rubic PRIVATE
        utils
        include
        include/RubikCube
        include/RubikCubeSolver
        include/Visualizer)
