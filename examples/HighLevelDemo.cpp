#include "HighLevelDemo.hpp"

#include <central64/PathPlanner.hpp>

using namespace central64;

void HighLevelDemo()
{
    // Prepare grid data.
    std::vector<std::vector<bool>> inputCells = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    };

    // Create the recommended path planner
    auto planner = CreateBestOverallPathPlanner(inputCells);

    // Print grid.
    printf("Grid for High-Level Demo:\n");
    printf("%s\n", ToString(planner.Grid()).c_str());

    // Create path planning scenarios.
    std::vector<std::pair<Offset2D, Offset2D>> scenarios = {
        {{ 8,  1}, {13,  6}},
        {{ 0,  0}, {13,  6}},
        {{ 3,  6}, {10,  4}},
        {{ 0,  6}, {13,  4}}
    };

    // Solve path planning scenarios.
    std::vector<std::vector<Offset2D>> paths{};
    for (const auto& [sourceCoords, sampleCoords] : scenarios) {
        std::vector<Offset2D> pathVertices = planner.PlanPath(sourceCoords, sampleCoords);
        paths.push_back(pathVertices);
    }

    // Print paths.
    printf("Paths produced by recommended path planner:\n");
    for (const auto& pathVertices : paths) {
        printf("%s\n", ToString(pathVertices).c_str());
    }
    printf("\n");

    // Print grid with last planned path.
    printf("Grid with last planned path:\n");
    printf("%s\n", ToString(planner.Grid(), paths.back()).c_str());
}
