cmake_minimum_required(VERSION 3.17)
project(project_1v1)

set(CMAKE_CXX_STANDARD 14)

add_executable(project_1
        lodepng.cpp
        lodepng.h
        main.cpp
        pathfinder.cpp
        pathfinder.h
        picture.cpp
        picture.h)
