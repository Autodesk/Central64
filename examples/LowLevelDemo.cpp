#include "LowLevelDemo.hpp"

#include <central64/PathPlanner.hpp>

using namespace central64;

void LowLevelDemo()
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

    // Create 8-neighbor grid with vertices at cell centers.
    Grid2D<8> grid{ inputCells, CellAlignment::Center };

    // Print grid.
    printf("8-Neighbor Grid for Low-Level Demo:\n");
    printf("%s\n", ToString(grid).c_str());

    // Print connections clockwise from X axis around vertex [6, 2].
    Offset2D coordsA{ 6, 2 };
    printf("Neighbors of %s:\n", ToString(coordsA).c_str());
    for (const auto& move : Neighborhood<8>::Moves()) {
        Offset2D neighborCoords = coordsA + move.Offset();
        bool isConnected = grid.Neighbors({ 6, 2 }).IsConnected(move);
        printf("  %s is %sconnected\n", ToString(neighborCoords).c_str(), isConnected ? "" : "not ");
    }
    printf("\n");

    // Perform line-of-sight tests from [8, 1] to [13, y] for all y.
    Offset2D coordsB{ 8, 1 };
    printf("Sightlines from %s:\n", ToString(coordsB).c_str());
    for (int y = 0; y < grid.Dims().Y(); ++y) {
        Offset2D distantCoords{ 13, y };
        bool lineOfSight = grid.LineOfSight(coordsB, distantCoords);
        printf("  %s is %svisible\n", ToString(distantCoords).c_str(), lineOfSight ? "" : "not ");
    }
    printf("\n");

    // Compute an 8-neighbor central grid path from [0, 0] to [13, 6].
    auto search = AStarSearch{ grid };
    auto pathVertices = search.PlanGridPath({ 0, 0 }, { 13, 6 });
    printf("8-Neighbor Central Grid Path:\n");
    printf("%s\n", ToString(pathVertices).c_str());
    printf("\n");

    // Smooth the 8-neighbor central grid path.
    auto smoothing = GreedySmoothing{ grid };
    smoothing.SmoothPath(pathVertices);
    printf("Path produced by Central A* with Greedy Smoothing:\n");
    printf("%s\n", ToString(pathVertices).c_str());
    printf("\n");

    // Compute several 8-neighbor regular paths from common source [6, 3].
    search.SearchAllNodes({ 6, 3 });
    printf("8-Neighbor Regular Grid Paths:\n");
    printf("%s\n", ToString(search.SampleGridPath({ 0, 0 }, false)).c_str());
    printf("%s\n", ToString(search.SampleGridPath({ 0, 6 }, false)).c_str());
    printf("%s\n", ToString(search.SampleGridPath({ 13, 0 }, false)).c_str());
    printf("%s\n", ToString(search.SampleGridPath({ 13, 6 }, false)).c_str());
    printf("\n");
}
