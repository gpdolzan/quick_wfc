//
// Created by Gabriel Dolzan on 4/9/2025.
//

#include "WFC_Input.h"
#include <fstream>

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

std::set<std::string> WFC_Input::read_tiles(std::ifstream &file)
{
    std::set<std::string> tiles_set = {};
    std::string line;
    std::streampos pos;

    std::getline(file, line);

    if (line != "[Tiles]")
    {
        std::cerr << "Didn't find [Tiles] Header, returning empty Tile Set" << std::endl;
        return tiles_set;
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

    return tiles_set;
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
    std::set<std::string> tiles_set;

    /* Aqui vem o codigo de como ler o arquivo wfc.in */
    std::getline(file, line);

    if (line != "[WFCINPUT]")
    {
        std::cerr << "WFC_Input Couldn't find [WFCINPUT] header in: " << filename << "\n";
        return false;
    }

    tiles_set = read_tiles(file);

    for (const auto& tile : tiles_set) {
        std::cout << tile << std::endl;
    }

    file.close();
    return true;
}