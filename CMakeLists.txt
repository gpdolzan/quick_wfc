cmake_minimum_required(VERSION 3.30)
project(quick_wfc)

set(CMAKE_CXX_STANDARD 26)

add_executable(quick_wfc main.cpp
        WFC.cpp
        WFC.h
        WFC_Set.cpp
        WFC_Set.h
        WFC_Input.cpp
        WFC_Input.h)
