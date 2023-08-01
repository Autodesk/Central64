#include "MidLevelDemo.hpp"

#include <central64/PathPlanner.hpp>

using namespace central64;

void MidLevelDemo()
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

    // Create an 8-neighbor Central A* path planner with Tentpole Smoothing
    auto planner = PathPlanner<8>{ inputCells,
                                   CellAlignment::Corner,
                                   SearchMethod::AStar,
                                   SmoothingMethod::Tentpole,
                                   true,
                                   true };

    // Print grid.
    printf("Grid for Mid-Level Demo:\n");
    printf("%s\n", ToString(planner.Grid()).c_str());

    // Create path planning scenarios.
    std::vector<std::pair<Offset2D, Offset2D>> planningScenarios = {
        {{ 9,  1}, {13,  6}},
        {{ 0,  0}, {14,  7}},
        {{ 0,  0}, {13,  7}},
        {{ 0,  7}, {14,  4}},
        {{ 2,  7}, {14,  0}}
    };

    // Solve path planning scenarios.
    std::vector<std::vector<Offset2D>> plannedPaths{};
    for (const auto& [sourceCoords, sampleCoords] : planningScenarios) {
        std::vector<Offset2D> pathVertices = planner.PlanPath(sourceCoords, sampleCoords);
        plannedPaths.push_back(pathVertices);
    }

    // Print planned paths.
    printf("Paths planned by 8-Neighbor Central A* with Tentpole Smoothing:\n");
    for (const auto& pathVertices : plannedPaths) {
        printf("%s\n", ToString(pathVertices).c_str());
    }
    printf("\n");

    // Print grid with last planned path.
    printf("Grid with last planned path:\n");
    printf("%s\n", ToString(planner.Grid(), plannedPaths.back()).c_str());

    // Create path sampling scenarios.
    Offset2D commonSourceCoords{ 7, 4 };
    std::vector<Offset2D> samplePoints = {
        {  0,   0},
        {  0,   7},
        {  2,   7},
        {  9,   1},
        { 14,   0},
        { 14,   4},
        { 14,   7}
    };

    // Solve path sampling scenarios.
    std::vector<std::vector<Offset2D>> sampledPaths{};
    planner.SearchAllNodes(commonSourceCoords);
    for (const auto& sampleCoords : samplePoints) {
        std::vector<Offset2D> pathVertices = planner.SamplePath(sampleCoords);
        sampledPaths.push_back(pathVertices);
    }

    // Print sampled paths.
    printf("Paths sampled by 8-Neighbor Central Dijkstra with Tentpole Smoothing:\n");
    for (const auto& pathVertices : sampledPaths) {
        printf("%s\n", ToString(pathVertices).c_str());
    }
    printf("\n");

    // Print grid with last sampled path.
    printf("Grid with last sampled path:\n");
    printf("%s\n", ToString(planner.Grid(), sampledPaths.back()).c_str());
}
