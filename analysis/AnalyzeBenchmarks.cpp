#include "AnalyzeBenchmarks.hpp"
#include <chrono>

using namespace central64;

std::vector<std::vector<bool>> ReadMapFile(const std::filesystem::path& mapFilePath)
{
    std::vector<std::vector<bool>> inputCells{};
    MovingAIGridMapFile mapFile{};
    mapFile.Open(mapFilePath.string());
    if (!mapFile.IsOpen()) {
        printf("Failed to open map file \"%s\"\n", mapFilePath.string().c_str());
    } 
    else {
        inputCells = mapFile.Read();
    }
    mapFile.Close();
    return inputCells;
}

std::vector<std::pair<Offset2D, Offset2D>> ReadScenarioFile(const std::filesystem::path& scenFilePath)
{
    std::vector<std::pair<Offset2D, Offset2D>> scenarios{};
    MovingAIGridScenarioFile scenFile{};
    scenFile.Open(scenFilePath.string());
    if (!scenFile.IsOpen()) {
        printf("Failed to open scenario file \"%s\"\n", scenFilePath.string().c_str());
    } 
    else {
        scenarios = scenFile.Read();
    }
    scenFile.Close();
    return scenarios;
}

template <int L>
void PrintResults(const std::string& label,
                  PathPlanner<L>& planner,
                  bool allNodes,
                  BenchmarkResult result)
{
    printf("%s\n", label.c_str());
    printf("%d-Neighbor Grid with %s-Aligned Cells\n",
           planner.NeighborhoodSize(), 
           planner.Grid().Alignment() == CellAlignment::Center ? "Center" : "Corner");
    printf("Path%s produced by %s %s with %s\n", 
           (result.totalScenarios > 1) ? "s" : "",
           planner.Centralize() ? "Central" : "Regular",
           allNodes ? planner.Search().AllNodesMethodName().c_str() :
                      planner.Search().MethodName().c_str(),
           planner.Smoothing().MethodName().c_str());
    std::string resultPrefix = (result.totalScenarios > 1) ? "Mean " : "";
    printf("%sPath Length = %.6f\n", resultPrefix.c_str(), result.meanPathLength);
    printf("%sRuntime (us) = %.6f\n", resultPrefix.c_str(), result.meanRuntime);
    if (result.totalScenarios > 1) {
        printf("Valid Paths = %d\n", result.validPaths);
        printf("Total Scenarios = %d\n", result.totalScenarios);
    }
    printf("\n");
}

template <int L>
BenchmarkResult AnalyzeBenchmark(const std::filesystem::path& mapFilePath,
                                 bool allNodes,
                                 central64::CellAlignment alignment,
                                 central64::SearchMethod searchMethod,
                                 central64::SmoothingMethod smoothingMethod,
                                 bool centralize,
                                 int scenarioIndex)
{
    std::filesystem::path scenFilePath = mapFilePath.string() + ".scen";

    std::vector<std::vector<bool>> inputCells = ReadMapFile(mapFilePath);
    std::vector<std::pair<Offset2D, Offset2D>> scenarios = ReadScenarioFile(scenFilePath);

    BenchmarkResult result;

    if (!inputCells.empty() && !scenarios.empty()) {
        auto planner = PathPlanner<L>{ inputCells, alignment, searchMethod, smoothingMethod, centralize };
        if (scenarioIndex >= 0) {
            if (scenarioIndex >= int(scenarios.size())) {
                printf("Warning: Scenario index %d must be less than number of scenarios (%d)\n", scenarioIndex, int(scenarios.size()));
                printf("\n");
            }
            else {
                std::vector<Offset2D> pathVertices{};
                const auto& scen = scenarios[scenarioIndex];
                auto tA = std::chrono::steady_clock::now();
                if (allNodes) {
                    planner.SearchAllNodes(scen.first);
                    pathVertices = planner.SamplePath(scen.second);
                }
                else {
                    pathVertices = planner.PlanPath(scen.first, scen.second);
                }
                auto tB = std::chrono::steady_clock::now();
                result.meanPathLength = PathLength(pathVertices);
                result.meanRuntime = double(std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count());
                result.validPaths = pathVertices.empty() ? 0 : 1;
                result.totalScenarios = 1;
                std::string label = "Result for " + mapFilePath.filename().string() + ", scenario " + std::to_string(scenarioIndex);
                PrintResults(label, planner, allNodes, result);
                printf("%s\n", ToString(pathVertices).c_str());
                printf("%s\n", ToString(planner.Grid(), pathVertices).c_str());
            }
        }
        else {
            if (allNodes) {
                double totalPathLength = 0.0;
                double totalRuntime = 0;
                int validPaths = 0;
                int totalScenarios = 0;
                for (const auto& scen : scenarios) {
                    if (totalScenarios%25 == 0) {
                        auto tA = std::chrono::steady_clock::now();
                        planner.SearchAllNodes(scen.first);
                        auto tB = std::chrono::steady_clock::now();
                        totalRuntime += std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count();
                    }
                    else {
                        auto tA = std::chrono::steady_clock::now();
                        std::vector<Offset2D> pathVertices = planner.SamplePath(scen.first);
                        auto tB = std::chrono::steady_clock::now();
                        totalRuntime += std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count();
                        if (!pathVertices.empty()) {
                            totalPathLength += PathLength(pathVertices);
                            ++validPaths;
                        }
                        ++totalScenarios;
                    }
                    {
                        auto tA = std::chrono::steady_clock::now();
                        std::vector<Offset2D> pathVertices = planner.SamplePath(scen.second);
                        auto tB = std::chrono::steady_clock::now();
                        totalRuntime += std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count();
                        if (!pathVertices.empty()) {
                            totalPathLength += PathLength(pathVertices);
                            ++validPaths;
                        }
                        ++totalScenarios;
                    }
                }
                if (validPaths > 0) {
                    result.meanPathLength = totalPathLength/validPaths;
                    result.meanRuntime = double(totalRuntime)/validPaths;
                }
                result.validPaths = validPaths;
                result.totalScenarios = totalScenarios;
            }
            else {
                double totalPathLength = 0.0;
                double totalRuntime = 0;
                int validPaths = 0;
                int totalScenarios = 0;
                for (const auto& scen : scenarios) {
                    auto tA = std::chrono::steady_clock::now();
                    std::vector<Offset2D> pathVertices = planner.PlanPath(scen.first, scen.second);
                    auto tB = std::chrono::steady_clock::now();
                    totalRuntime += std::chrono::duration_cast<std::chrono::microseconds>(tB - tA).count();
                    if (!pathVertices.empty()) {
                        totalPathLength += PathLength(pathVertices);
                        ++validPaths;
                    }
                    ++totalScenarios;
                }
                if (validPaths > 0) {
                    result.meanPathLength = totalPathLength/validPaths;
                    result.meanRuntime = double(totalRuntime)/validPaths;
                }
                result.validPaths = validPaths;
                result.totalScenarios = totalScenarios;
            }
            std::string label = "Results for " + mapFilePath.filename().string();
            PrintResults(label, planner, allNodes, result);
        }
    }

    return result;
}

template <int L>
void AnalyzeBenchmarkSet(const std::filesystem::path& filePath,
                         bool allNodes,
                         central64::CellAlignment alignment,
                         central64::SearchMethod searchMethod,
                         central64::SmoothingMethod smoothingMethod,
                         bool centralize,
                         int scenarioIndex)
{
    if (!std::filesystem::is_directory(filePath)) {
        AnalyzeBenchmark<L>(filePath, allNodes, alignment, searchMethod, smoothingMethod, centralize, scenarioIndex);
    }
    else {
        if (scenarioIndex != -1) {
            printf("Warning: Ignoring scenario index %d because multiple maps are being analyzed\n", scenarioIndex);
            printf("\n");
        }
        double totalPathLength = 0.0;
        double totalRuntime = 0;
        int validPaths = 0;
        int totalScenarios = 0;
        int benchmarkCount = 0;
        for (const auto& directoryEntry : std::filesystem::directory_iterator{ filePath }) {
            const std::filesystem::path& directoryEntryPath = directoryEntry.path();
            if (!std::filesystem::is_directory(directoryEntryPath)) {
                if (directoryEntryPath.extension() == ".map") {
                    BenchmarkResult result = AnalyzeBenchmark<L>(directoryEntryPath, allNodes, alignment, searchMethod, smoothingMethod, centralize);
                    totalPathLength += result.meanPathLength;
                    totalRuntime += result.meanRuntime;
                    validPaths += result.validPaths;
                    totalScenarios += result.totalScenarios;
                    ++benchmarkCount;
                }
            }
        }

        BenchmarkResult overallResult;
        if (benchmarkCount > 0) {
            overallResult.meanPathLength = totalPathLength/benchmarkCount;
            overallResult.meanRuntime = totalRuntime/benchmarkCount;
        }
        overallResult.validPaths = validPaths;
        overallResult.totalScenarios = totalScenarios;

        auto planner = PathPlanner<L>{ {{0}}, alignment, searchMethod, smoothingMethod, centralize };
        std::string label = "Overall results for " + filePath.string();
        PrintResults(label, planner, allNodes, overallResult);
    }
}

void AnalyzeAllNeighborhoods(const std::filesystem::path& filePath,
                             bool allNodes,
                             central64::CellAlignment alignment,
                             central64::SearchMethod searchMethod,
                             central64::SmoothingMethod smoothingMethod,
                             bool centralize,
                             int scenarioIndex)
{
    AnalyzeBenchmarkSet< 4>(filePath, allNodes, alignment, searchMethod, smoothingMethod, centralize, scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, alignment, searchMethod, smoothingMethod, centralize, scenarioIndex);
    AnalyzeBenchmarkSet<16>(filePath, allNodes, alignment, searchMethod, smoothingMethod, centralize, scenarioIndex);
    AnalyzeBenchmarkSet<32>(filePath, allNodes, alignment, searchMethod, smoothingMethod, centralize, scenarioIndex);
    AnalyzeBenchmarkSet<64>(filePath, allNodes, alignment, searchMethod, smoothingMethod, centralize, scenarioIndex);
}

void AnalyzeAllSearchMethods(const std::filesystem::path& filePath,
                             bool allNodes,
                             central64::CellAlignment alignment,
                             central64::SmoothingMethod smoothingMethod,
                             bool centralize,
                             int scenarioIndex)
{
    AnalyzeAllNeighborhoods(filePath, allNodes, alignment, SearchMethod::AStar,            smoothingMethod, centralize, scenarioIndex);
    AnalyzeAllNeighborhoods(filePath, allNodes, alignment, SearchMethod::JumpPoint,        smoothingMethod, centralize, scenarioIndex);
    AnalyzeAllNeighborhoods(filePath, allNodes, alignment, SearchMethod::BoundedJumpPoint, smoothingMethod, centralize, scenarioIndex);
    AnalyzeAllNeighborhoods(filePath, allNodes, alignment, SearchMethod::MixedAStar,       smoothingMethod, centralize, scenarioIndex);
    AnalyzeAllNeighborhoods(filePath, allNodes, alignment, SearchMethod::MixedJumpPoint,   smoothingMethod, centralize, scenarioIndex);
}

void PerformPartialAnalysis(const std::filesystem::path& filePath,
                            bool allNodes,
                            int scenarioIndex)
{
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::No,       false, scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Greedy,   false, scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Greedy,   true,  scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Tentpole, false, scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::MixedAStar,       SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::JumpPoint,        SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet< 8>(filePath, allNodes, CellAlignment::Corner, SearchMethod::MixedJumpPoint,   SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet<16>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Tentpole, false, scenarioIndex);
    AnalyzeBenchmarkSet<16>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet<16>(filePath, allNodes, CellAlignment::Corner, SearchMethod::MixedAStar,       SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet<16>(filePath, allNodes, CellAlignment::Corner, SearchMethod::JumpPoint,        SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet<16>(filePath, allNodes, CellAlignment::Corner, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet<16>(filePath, allNodes, CellAlignment::Corner, SearchMethod::MixedJumpPoint,   SmoothingMethod::Tentpole, true,  scenarioIndex);
    AnalyzeBenchmarkSet<64>(filePath, allNodes, CellAlignment::Corner, SearchMethod::AStar,            SmoothingMethod::Tentpole, true,  scenarioIndex);
}

void PerformCompleteAnalysis(const std::filesystem::path& filePath,
                             bool allNodes,
                             int scenarioIndex)
{
    AnalyzeAllSearchMethods(filePath, allNodes, CellAlignment::Corner, SmoothingMethod::Greedy,   false, scenarioIndex);
    AnalyzeAllSearchMethods(filePath, allNodes, CellAlignment::Corner, SmoothingMethod::Tentpole, false, scenarioIndex);
    AnalyzeAllSearchMethods(filePath, allNodes, CellAlignment::Corner, SmoothingMethod::Greedy,   true,  scenarioIndex);
    AnalyzeAllSearchMethods(filePath, allNodes, CellAlignment::Corner, SmoothingMethod::Tentpole, true,  scenarioIndex);
}
