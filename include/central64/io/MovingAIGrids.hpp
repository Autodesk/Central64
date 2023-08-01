#pragma once
#ifndef CENTRAL64_MOVING_AI_GRIDS
#define CENTRAL64_MOVING_AI_GRIDS

#include <central64/grid/Offset2D.hpp>
#include <fstream>

namespace central64 {

/// A file access object that reads a binary map representation from a Moving AI Lab grid map file.
/// File format information and sample files can be found here: https://movingai.com/benchmarks/grids.html
class MovingAIGridMapFile
{
public:
    MovingAIGridMapFile() = default;  ///< Create a grid map file access object.

    void Open(const std::string& filename) { mapFile_.open(filename); }  ///< Open the file with name `filename`.
    void Close() { mapFile_.close(); }                                   ///< Close the file.
    bool IsOpen() const { return mapFile_.is_open(); }                   ///< Check if the file is open.

    /// Read the map as a vector of vectors of traverable and obstructed cells.
    /// If `bool(map[y][x])`, then the cell at coordinates [x, y] is obstructed; otherwise it is traversable.
    std::vector<std::vector<bool>> Read();

private:
    std::ifstream mapFile_{};
};

/// A file access object that reads path planning scenarios from a Moving AI Lab grid map scenario file.
/// File format information and sample files can be found here: https://movingai.com/benchmarks/grids.html
class MovingAIGridScenarioFile
{
public:
    MovingAIGridScenarioFile() = default;  ///< Create a grid map scenario file access object.

    void Open(const std::string& filename) { scenFile_.open(filename); }  ///< Open the file with name `filename`.
    void Close() { scenFile_.close(); }                                   ///< Close the file.
    bool IsOpen() const { return scenFile_.is_open(); }                   ///< Check if the file is open.

    /// Read the scenarios as a vector of pairs of end point coordinates.
    std::vector<std::pair<Offset2D, Offset2D>> Read();

private:
    std::ifstream scenFile_{};
};

inline std::vector<std::vector<bool>> MovingAIGridMapFile::Read()
{
    std::vector<std::vector<bool>> inputCells{};
    if (IsOpen()) {
        std::string line;
        std::string token;
        std::getline(mapFile_, line);
        mapFile_ >> token;
        mapFile_ >> token;
        int ny = std::stoi(token);
        mapFile_ >> token;
        mapFile_ >> token;
        int nx = std::stoi(token);
        std::getline(mapFile_, line);
        std::getline(mapFile_, line);
        inputCells.assign(ny, std::vector<bool>(nx, true));
        for (int y = 0; y < ny; ++y) {
            std::getline(mapFile_, line);
            for (int x = 0; x < nx; ++x) {
                if (line[x] == '.') {
                    inputCells[y][x] = false;
                }
            }
        }
    }
    return inputCells;
}

inline std::vector<std::pair<Offset2D, Offset2D>> MovingAIGridScenarioFile::Read()
{
    std::vector<std::pair<Offset2D, Offset2D>> scenarios{};
    if (IsOpen()) {
        std::string line;
        std::string token;
        std::getline(scenFile_, line);
        while (scenFile_ >> token) {
            scenFile_ >> token;
            scenFile_ >> token;
            scenFile_ >> token;
            scenFile_ >> token;
            int xA = std::stoi(token);
            scenFile_ >> token;
            int yA = std::stoi(token);
            scenFile_ >> token;
            int xB = std::stoi(token);
            scenFile_ >> token;
            int yB = std::stoi(token);
            scenFile_ >> token;
            scenarios.push_back({ {xA, yA}, {xB, yB} });
        }
    }
    return scenarios;
}

}  // namespace

#endif
