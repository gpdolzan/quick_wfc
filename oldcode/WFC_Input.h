//
// Created by Gabriel Dolzan on 4/9/2025.
//

#ifndef WFC_INPUT_H
#define WFC_INPUT_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>

class WFC_Input
{
public:
    std::set<std::string> tiles_set;
    std::map<std::string, std::vector<std::vector<std::string>>> constraints_map;

    void initialize_variables();
    static bool file_exists(const std::string& filename);
    bool find_in_set(std::string element, std::string err_msg);
    bool compare_line(std::string section, std::string line, std::string err_msg);
    bool read_file(const std::string& filename);
    bool read_tiles(std::ifstream &file);
    bool read_constraints(std::ifstream &file);
};

#endif //WFC_INPUT_H
