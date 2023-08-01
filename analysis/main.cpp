#include "AnalyzeBenchmarks.hpp"
#include <cstdlib>

void usage()
{
    printf("Usage: Central64Analysis <mode> <scope> <filepath> [scenario]\n");
    printf("\n");
    printf("The <mode> must be one of the following:\n");
    printf("   heuristic  Plan one path for each benchmark scenario.\n");
    printf("   dijkstra   Group benchmark scenarios in sets of 13 (26 points),\n");
    printf("              perform an all-nodes search on the first point, then\n");
    printf("              sample a path for each of the next 25 points.\n");
    printf("\n");
    printf("The <scope> must be one of the following:\n");
    printf("   partial    Compare a selected set of path planning methods.\n");
    printf("   complete   Compare a comprehensive set of path planning methods.\n");
}

int main(int argc, char* argv[])
{
    if (argc < 4) {
        usage();
        return 1;
    }

    std::string mode = argv[1];
    if (mode != "heuristic" && mode != "dijkstra") {
        usage();
        return 2;
    }

    std::string scope = argv[2];
    if (scope != "partial" && scope != "complete") {
        usage();
        return 3;
    }

    std::filesystem::path filePath = argv[3];
    int scenarioIndex = (argc > 4) ? std::strtol(argv[4], nullptr, 0) : -1;

    if (scope == "partial") {
        PerformPartialAnalysis(filePath, (mode == "dijkstra"), scenarioIndex);
    }
    else {
        PerformCompleteAnalysis(filePath, (mode == "dijkstra"), scenarioIndex);
    }

    return 0;
}
