// wfc.cpp
// Compile with: g++ -std=c++11 wfc.cpp -o wfc
// Make sure to have stb_image_write.h in your include path.
// You can obtain stb_image_write.h from: https://github.com/nothings/stb

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <map>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

using namespace std;

//------------------------------------------------------------------------------
// Enum for Directions.
enum Direction { NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3 };

// Helper to convert a direction string (case-insensitive) to Direction enum.
bool parseDirection(const string& dirStr, Direction &dir) {
    string d = dirStr;
    for (auto &c : d)
        c = toupper(c);
    if (d == "NORTH") { dir = NORTH; return true; }
    if (d == "EAST")  { dir = EAST;  return true; }
    if (d == "SOUTH") { dir = SOUTH; return true; }
    if (d == "WEST")  { dir = WEST;  return true; }
    return false;
}

//------------------------------------------------------------------------------
// Structure for tile definitions.
struct WFCTileDefinition {
    string name;
    int r, g, b;
};

//------------------------------------------------------------------------------
// Structure to temporarily hold constraint entries from the input file.
struct ConstraintEntry {
    string baseTileName;
    string dirStr;
    vector<string> allowedNames;
};

//------------------------------------------------------------------------------
// WFCConstraint: Stores allowed neighbor tile types for each direction.
class WFCConstraint {
public:
    // For each of the 4 directions, store allowed tile IDs.
    vector<int> allowedTiles[4];

    WFCConstraint() {}

    // Utility: Check if a candidate neighbor tile (tileID) is allowed in the given direction.
    bool allows(Direction dir, int tileID) const {
        const vector<int>& allowed = allowedTiles[dir];
        return (find(allowed.begin(), allowed.end(), tileID) != allowed.end());
    }
};

//------------------------------------------------------------------------------
// WFCTile: Represents one cell in the grid with a set of possible tile IDs.
class WFCTile {
public:
    vector<int> possibilities;  // Possible tile type IDs for this cell.
    bool collapsed;             // Whether the cell has been collapsed.
    int finalTile;              // Final tile type ID (if collapsed).
    string finalName;           // Final tile's name (for identification).

    WFCTile(int numTileTypes) : collapsed(false), finalTile(-1), finalName("") {
        for (int i = 0; i < numTileTypes; i++) {
            possibilities.push_back(i);
        }
    }

    // Collapse the cell by choosing a random possibility.
    // tileDefs: list of tile definitions used to look up the tile's name.
    void collapse(const vector<WFCTileDefinition>& tileDefs) {
        if (!possibilities.empty() && !collapsed) {
            int index = rand() % possibilities.size();
            finalTile = possibilities[index];
            possibilities.clear();
            possibilities.push_back(finalTile);
            collapsed = true;
            finalName = tileDefs[finalTile].name;
        }
    }
};

//------------------------------------------------------------------------------
// WFC: Core class that holds the grid, tile definitions, constraints, and runs the algorithm.
class WFC {
private:
    int width, height;
    int tileSize;  // Pixel size for output image tiles.

public:
    vector<vector<WFCTile>> grid;
    vector<WFCConstraint> tileConstraints;          // Global constraints for each tile type.
    vector<WFCTileDefinition> tileDefinitions;        // Tile definitions (name, color).
    map<string, int> tileNameToID;                    // Mapping from tile name to tile ID.

    // Constructor: grid dimensions, tile size, and the input file.
    WFC(int w, int h, int tSize, const string &inputFile)
        : width(w), height(h), tileSize(tSize)
    {
        srand(static_cast<unsigned>(time(0)));

        if (!loadFromFile(inputFile)) {
            cerr << "Error loading input file: " << inputFile << endl;
            exit(1);
        }

        // Resize and initialize the constraints vector.
        int numTileTypes = tileDefinitions.size();
        tileConstraints.resize(numTileTypes);
        for (int i = 0; i < numTileTypes; i++) {
            for (int d = 0; d < 4; d++) {
                // By default, allow every tile type.
                for (int j = 0; j < numTileTypes; j++)
                    tileConstraints[i].allowedTiles[d].push_back(j);
            }
        }

        // Initialize the grid.
        grid.reserve(height);
        for (int y = 0; y < height; y++) {
            vector<WFCTile> row;
            row.reserve(width);
            for (int x = 0; x < width; x++) {
                row.push_back(WFCTile(numTileTypes));
            }
            grid.push_back(row);
        }
    }

    // Parses a .wfcin input file.
    // Expected file structure:
    //   [WFINPUT]
    //   [Tiles]
    //   Red 255 0 0
    //   Green 0 255 0
    //   Blue 0 0 255
    //
    //   [Constraints]
    //   Red NORTH Green Blue
    //   Red EAST Blue
    //   Green SOUTH Red Blue
    //   Green WEST Red Blue
    //   Blue NORTH Red
    //   Blue EAST Green
    //
    // Lines starting with '#' or ';' are treated as comments.
    bool loadFromFile(const string &filename) {
        ifstream infile(filename);
        if (!infile.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
            return false;
        }

        string line;
        enum Section { NONE, TILES, CONSTRAINTS } currentSection = NONE;
        bool headerRead = false;
        vector<ConstraintEntry> constraintEntries;

        while (getline(infile, line)) {
            // Trim whitespace.
            if(line.empty()) continue;
            // Skip comment lines.
            if(line[0] == '#' || line[0] == ';')
                continue;

            // Check for section headers.
            if (line[0] == '[') {
                if (line.find("WFINPUT") != string::npos) {
                    headerRead = true;
                    currentSection = NONE;
                } else if (line.find("Tiles") != string::npos) {
                    if(!headerRead) {
                        cerr << "Missing [WFINPUT] header." << endl;
                        return false;
                    }
                    currentSection = TILES;
                } else if (line.find("Constraints") != string::npos) {
                    currentSection = CONSTRAINTS;
                } else {
                    currentSection = NONE;
                }
                continue;
            }

            // Process lines according to the current section.
            istringstream iss(line);
            if (currentSection == TILES) {
                // Format: <TileName> <R> <G> <B>
                string tileName;
                int r, g, b;
                if (!(iss >> tileName >> r >> g >> b)) {
                    cerr << "Invalid tile definition: " << line << endl;
                    continue;
                }
                WFCTileDefinition def;
                def.name = tileName;
                def.r = r;
                def.g = g;
                def.b = b;
                tileNameToID[tileName] = tileDefinitions.size();
                tileDefinitions.push_back(def);
            } else if (currentSection == CONSTRAINTS) {
                // Format: <TileName> <Direction> <AllowedTileName1> [AllowedTileName2] ...
                ConstraintEntry entry;
                if (!(iss >> entry.baseTileName >> entry.dirStr)) {
                    cerr << "Invalid constraint line (missing fields): " << line << endl;
                    continue;
                }
                string allowedTileName;
                while (iss >> allowedTileName) {
                    entry.allowedNames.push_back(allowedTileName);
                }
                constraintEntries.push_back(entry);
            }
        }
        infile.close();

        if (tileDefinitions.empty()) {
            cerr << "No tile definitions were loaded." << endl;
            return false;
        }
        // Now that we have tile definitions, apply the constraints.
        int numTileTypes = tileDefinitions.size();
        // At this point, tileConstraints is resized and filled with default values by the constructor.
        // We now override defaults from the input file.
        for (auto &entry : constraintEntries) {
            Direction d;
            if (!parseDirection(entry.dirStr, d)) {
                cerr << "Invalid direction in constraint: " << entry.dirStr << endl;
                continue;
            }
            if (tileNameToID.find(entry.baseTileName) == tileNameToID.end()) {
                cerr << "Unknown tile name in constraints: " << entry.baseTileName << endl;
                continue;
            }
            int baseTileID = tileNameToID[entry.baseTileName];
            // Clear default allowed list for the given direction.
            tileConstraints[baseTileID].allowedTiles[d].clear();
            for (const auto &allowedName : entry.allowedNames) {
                if (tileNameToID.find(allowedName) == tileNameToID.end()) {
                    cerr << "Unknown allowed tile name: " << allowedName << endl;
                    continue;
                }
                int allowedID = tileNameToID[allowedName];
                tileConstraints[baseTileID].allowedTiles[d].push_back(allowedID);
            }
        }
        return true;
    }

    // Runs the collapse and propagation process until all cells are collapsed.
    void run() {
        while (!isComplete()) {
            int minEntropy = std::numeric_limits<int>::max();
            int chosenX = -1, chosenY = -1;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (!grid[y][x].collapsed) {
                        int possCount = grid[y][x].possibilities.size();
                        if (possCount < minEntropy && possCount > 0) {
                            minEntropy = possCount;
                            chosenX = x;
                            chosenY = y;
                        }
                    }
                }
            }
            if (chosenX == -1 || chosenY == -1) {
                cout << "No valid cell to collapse. A conflict may have occurred." << endl;
                return;
            }
            grid[chosenY][chosenX].collapse(tileDefinitions);
            propagate();
        }
    }

    // Propagates constraints to update possible tile values.
    void propagate() {
        bool changed = true;
        int numTileTypes = tileDefinitions.size();
        while (changed) {
            changed = false;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    if (!grid[y][x].collapsed) {
                        vector<int> newPossibilities;
                        for (int candidate : grid[y][x].possibilities) {
                            bool valid = true;
                            // North neighbor
                            if (y > 0 && grid[y-1][x].collapsed) {
                                int neighborTile = grid[y-1][x].finalTile;
                                if (!tileConstraints[candidate].allows(NORTH, neighborTile))
                                    valid = false;
                            }
                            // South neighbor
                            if (y < height-1 && grid[y+1][x].collapsed) {
                                int neighborTile = grid[y+1][x].finalTile;
                                if (!tileConstraints[candidate].allows(SOUTH, neighborTile))
                                    valid = false;
                            }
                            // West neighbor
                            if (x > 0 && grid[y][x-1].collapsed) {
                                int neighborTile = grid[y][x-1].finalTile;
                                if (!tileConstraints[candidate].allows(WEST, neighborTile))
                                    valid = false;
                            }
                            // East neighbor
                            if (x < width-1 && grid[y][x+1].collapsed) {
                                int neighborTile = grid[y][x+1].finalTile;
                                if (!tileConstraints[candidate].allows(EAST, neighborTile))
                                    valid = false;
                            }
                            if (valid)
                                newPossibilities.push_back(candidate);
                        }
                        if (newPossibilities.size() < grid[y][x].possibilities.size()) {
                            grid[y][x].possibilities = newPossibilities;
                            changed = true;
                            if (grid[y][x].possibilities.size() == 1)
                                grid[y][x].collapse(tileDefinitions);
                        }
                    }
                }
            }
        }
    }

    // Returns true if every cell in the grid is collapsed.
    bool isComplete() const {
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++)
                if (!grid[y][x].collapsed)
                    return false;
        return true;
    }

    // Generates an image (PNG) based on the final collapsed grid.
    void generateImage(const string& filename) {
        int imageWidth = width * tileSize;
        int imageHeight = height * tileSize;
        int channels = 3; // RGB

        vector<unsigned char> image(imageWidth * imageHeight * channels, 255);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int tileID = grid[y][x].finalTile;
                int r = 200, g = 200, b = 200;
                if (tileID >= 0 && tileID < (int)tileDefinitions.size()) {
                    r = tileDefinitions[tileID].r;
                    g = tileDefinitions[tileID].g;
                    b = tileDefinitions[tileID].b;
                }
                for (int ty = 0; ty < tileSize; ty++) {
                    int py = y * tileSize + ty;
                    for (int tx = 0; tx < tileSize; tx++) {
                        int px = x * tileSize + tx;
                        int index = (py * imageWidth + px) * channels;
                        image[index+0] = r;
                        image[index+1] = g;
                        image[index+2] = b;
                    }
                }
            }
        }
        if (stbi_write_png(filename.c_str(), imageWidth, imageHeight, channels, image.data(), imageWidth * channels))
            cout << "Image generated: " << filename << endl;
        else
            cerr << "Error writing image file." << endl;
    }
};

//------------------------------------------------------------------------------
// Main Function: Create a WFC object, run the algorithm, and generate the output image.
int main() {
    // Modify grid parameters as desired.
    int gridWidth = 20;
    int gridHeight = 20;
    int tilePixelSize = 32;
    string inputFile = "input.wfcin"; // Ensure this file exists in your working directory.

    WFC wfc(gridWidth, gridHeight, tilePixelSize, inputFile);
    wfc.run();

    if (!wfc.isComplete()) {
        cout << "WFC algorithm did not complete successfully (a conflict may have occurred)." << endl;
        return 1;
    }
    wfc.generateImage("output.png");

    return 0;
}