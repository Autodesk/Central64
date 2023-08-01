#pragma once
#ifndef CENTRAL64ANALYSIS_ANALYZE_BENCHMARKS
#define CENTRAL64ANALYSIS_ANALYZE_BENCHMARKS

#include <central64/io/MovingAIGrids.hpp>
#include <central64/PathPlanner.hpp>
#include <filesystem>

std::vector<std::vector<bool>> ReadMapFile(const std::filesystem::path& mapFilePath);

std::vector<std::pair<central64::Offset2D, central64::Offset2D>> ReadScenarioFile(const std::filesystem::path& scenFilePath);

struct BenchmarkResult
{
    double meanPathLength{ std::numeric_limits<double>::infinity() };
    double meanRuntime{ 0.0 };
    int validPaths{ 0 };
    int totalScenarios{ 0 };
};

template <int L>
void PrintResults(const std::string& label,
                  central64::PathPlanner<L>& planner,
                  bool allPaths,
                  BenchmarkResult result);

template <int L>
BenchmarkResult AnalyzeBenchmark(const std::filesystem::path& mapFilePath,
                                 bool allNodes,
                                 central64::CellAlignment alignment,
                                 central64::SearchMethod searchMethod,
                                 central64::SmoothingMethod smoothingMethod,
                                 bool centralize,
                                 int scenarioIndex = -1);

template <int L>
void AnalyzeBenchmarkSet(const std::filesystem::path& filePath,
                         bool allNodes,
                         central64::CellAlignment alignment,
                         central64::SearchMethod searchMethod,
                         central64::SmoothingMethod smoothingMethod,
                         bool centralize,
                         int scenarioIndex = -1);

void AnalyzeAllNeighborhoods(const std::filesystem::path& filePath,
                             bool allNodes,
                             central64::CellAlignment alignment,
                             central64::SearchMethod searchMethod,
                             central64::SmoothingMethod smoothingMethod,
                             bool centralize,
                             int scenarioIndex = -1);

void AnalyzeAllSearchMethods(const std::filesystem::path& filePath,
                             bool allNodes,
                             central64::CellAlignment alignment,
                             central64::SmoothingMethod smoothingMethod,
                             bool centralize,
                             int scenarioIndex);

void PerformPartialAnalysis(const std::filesystem::path& filePath,
                            bool allNodes,
                            int scenarioIndex = -1);

void PerformCompleteAnalysis(const std::filesystem::path& filePath,
                             bool allNodes,
                             int scenarioIndex = -1);

#endif
