cmake_minimum_required(VERSION 3.30)
project(quick_wfc)

set(CMAKE_CXX_STANDARD 26)

add_executable(quick_wfc inspiration/old_main.cpp
        oldcode/WFC.cpp
        oldcode/WFC.h
        oldcode/WFC_Set.cpp
        oldcode/WFC_Set.h
        oldcode/WFC_Input.cpp
        oldcode/WFC_Input.h
        stb_image_write.h
)
