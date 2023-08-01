#include "TestScenarios.hpp"

using namespace central64;

template <int L>
void TestScenario(CellAlignment alignment,
                  SearchMethod searchMethod,
                  SmoothingMethod smoothingMethod,
                  bool centralize,
                  Offset2D sourceCoords,
                  Offset2D sampleCoords)
{
    // Prepare grid data.
    const std::vector<std::vector<bool>> inputCells = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    };

    // Plan path.
    auto planner = PathPlanner<L>{ inputCells, alignment, searchMethod, smoothingMethod, centralize };
    std::vector<Offset2D> pathVertices = planner.PlanPath(sourceCoords, sampleCoords);

    // Print scenario heading.
    printf("%d-Neighbor Grid with %s-Aligned Cells\n",
           planner.NeighborhoodSize(), 
           planner.Grid().Alignment() == CellAlignment::Center ? "Center" : "Corner");
    printf("Path produced by %s %s with %s\n", 
           planner.Centralize() ? "Central" : "Regular",
           planner.Search().MethodName().c_str(),
           planner.Smoothing().MethodName().c_str());

    // Print scenario.
    printf("%s\n", ToString(planner.Grid(), pathVertices).c_str());
}

void TestScenarios()
{
    TestScenario< 4>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::No,       false, {  4,  0 }, { 14,  7 });
    TestScenario< 4>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::No,       true,  {  4,  0 }, { 14,  7 });
    TestScenario< 8>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario< 8>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::No,       true,  {  0,  7 }, { 14,  4 });
    TestScenario< 8>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Greedy,   true,  {  0,  7 }, { 14,  4 });
    TestScenario< 8>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Tentpole, true,  {  0,  7 }, { 14,  4 });
    TestScenario<16>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario<16>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::No,       true,  {  0,  7 }, { 14,  4 });
    TestScenario<16>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Greedy,   true,  {  0,  7 }, { 14,  4 });
    TestScenario<16>(CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Tentpole, true,  {  0,  7 }, { 14,  4 });
    TestScenario< 4>(CellAlignment::Center, SearchMethod::AStar,            SmoothingMethod::No,       false, {  0,  6 }, { 13,  0 });
    TestScenario< 4>(CellAlignment::Center, SearchMethod::MixedAStar,       SmoothingMethod::No,       false, {  0,  6 }, { 13,  0 });
    TestScenario< 4>(CellAlignment::Corner, SearchMethod::JumpPoint,        SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario< 4>(CellAlignment::Corner, SearchMethod::BoundedJumpPoint, SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario< 4>(CellAlignment::Corner, SearchMethod::MixedJumpPoint,   SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario< 8>(CellAlignment::Corner, SearchMethod::JumpPoint,        SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario< 8>(CellAlignment::Corner, SearchMethod::BoundedJumpPoint, SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario< 8>(CellAlignment::Corner, SearchMethod::MixedJumpPoint,   SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario<16>(CellAlignment::Corner, SearchMethod::JumpPoint,        SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario<16>(CellAlignment::Corner, SearchMethod::BoundedJumpPoint, SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
    TestScenario<16>(CellAlignment::Corner, SearchMethod::MixedJumpPoint,   SmoothingMethod::No,       false, {  0,  7 }, { 14,  4 });
}
