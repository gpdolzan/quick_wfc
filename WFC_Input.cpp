//
// Created by Gabriel Dolzan on 4/9/2025.
//

#include "WFC_Input.h"
#include <fstream>
#include <iostream>

void WFC_Input::initialize_variables()
{
    tiles_set = {};
    constraints_map = {};
}


bool WFC_Input::file_exists(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "WFC_Input Couldn't find file: " << filename << std::endl;
        return false;
    }

    file.close();
    return true;
}

bool WFC_Input::read_tiles(std::ifstream &file)
{
    std::string line;
    std::streampos pos;

    std::getline(file, line);

    if (line != "[Tiles]")
    {
        std::cerr << "Didn't find [Tiles] Header, returning empty Tile Set" << std::endl;
        return false;
    }

    while (true)
    {
        if (!std::getline(file, line))
            break;

        pos = file.tellg();

        if (line == "[Constraints]")
        {
            file.seekg(pos);
            break;
        }

        auto result = tiles_set.insert(line);
        if (!result.second)
        {
            std::cerr << "Tile: " << line << " was already added, maybe the input file is wrong. Execution will continue" << std::endl;
        }
    }

    return true;
}

bool read_constraints(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);

    if (line != "[Constraints]")
    {
        std::cerr << "Didn't find [Tiles] Header, returning empty Tile Set" << std::endl;
        return false;
    }

    return true;
}

bool WFC_Input::read_file(const std::string& filename)
{
    if (!file_exists(filename))
    {
        std::cerr << "WFC_Input Couldn't read file: " << filename << std::endl;
        return false;
    }

    std::ifstream file(filename);
    std::string line;

    std::getline(file, line);

    if (line != "[WFCINPUT]")
    {
        std::cerr << "WFC_Input Couldn't find [WFCINPUT] header in: " << filename << "\n";
        return false;
    }

    /* Reading the tiles */
    if (!read_tiles(file))
        return false;

    std::cout << "Number of tiles in set: " << tiles_set.size() << std::endl;

    /* Now Reading the Constraints */

    file.close();
    return true;
}