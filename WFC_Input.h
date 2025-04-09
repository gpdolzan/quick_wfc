//
// Created by Gabriel Dolzan on 4/9/2025.
//

#ifndef WFC_INPUT_H
#define WFC_INPUT_H

#include <string>
#include <iostream>
#include <set>

class WFC_Input
{
public:
    static bool file_exists(const std::string& filename);
    static bool read_file(const std::string& filename);
    static std::set<std::string> read_tiles(std::ifstream &file);
};

#endif //WFC_INPUT_H
