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

bool WFC_Input::find_in_set(std::string element, std::string err_msg)
{
    if (tiles_set.find(element) == tiles_set.end())
    {
        std::cerr << err_msg << ": " << element << std::endl;
        return false;
    }
    return true;
}

bool WFC_Input::compare_line(std::string section, std::string line, std::string err_msg)
{
    if (section != line)
    {
        std::cerr << err_msg << std::endl;
        return false;
    }
    return true;
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

    std::getline(file, line);
    if (!compare_line("[Begin]", line, "Didn't find [Begin] Header inside [Tiles]"))
        return false;

    while (true)
    {
        if (!std::getline(file, line))
            break;

        if (line == "[End]")
            break;

        auto result = tiles_set.insert(line);
        if (!result.second)
        {
            std::cerr << "Tile: " << line << " was already added, maybe the input file is wrong. Execution will continue" << std::endl;
        }
    }

    return true;
}

bool WFC_Input::read_constraints(std::ifstream &file)
{
    std::string line;
    std::getline(file, line);

    if (!compare_line("[Begin]", line, "Didn't find [Begin] Header inside [Constraints]"))
        return false;

    while (true)
    {
        if (!std::getline(file, line))
            break;

        if (line == "[End]")
            break;

        std::istringstream iss(line);
        std::vector<std::string> words;
        std::string word;

        while (iss >> word)
        {
            words.push_back(word);
        }

        // Agora words contem todas as palavras daquela linha, precisamos processá-las de acordo.
        for (int i = 0; i < (int)words.size(); i++)
        {
            // i == 0, verificamos se a cor está cadastrada nos dados de Tiles
            if (i == 0)
            {
                if (!find_in_set(words.at(i), "Trying to add constraints to an unexisting tile"))
                    return false;
            }
            else if (i == 1) // i == 1, determinamos quais direções vão ser adicionadas
            {
                if (words.at(i) == "LURD")
                    std::cerr << "Ok ta indo" << std::endl;
            }
        }
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

    if (!compare_line("[WFCINPUT]", line, "Couldn't find [WFCINPUT]"))
        return false;

    std::getline(file, line);

    if (!compare_line("[Tiles]", line, "Couldn't find [Tiles]"))
        return false;

    /* Reading the tiles */
    if (!read_tiles(file))
        return false;

    std::cout << "Number of tiles in set: " << tiles_set.size() << std::endl;

    std::getline(file, line);

    if (!compare_line("[Constraints]", line, "Couldn't find [Constraints]"))
        return false;

    /* Now Reading the Constraints */
    if (!read_constraints(file))
        return false;

    file.close();
    return true;
}