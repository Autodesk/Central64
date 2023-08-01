#pragma once
#ifndef CENTRAL64_PATH_PLANNER
#define CENTRAL64_PATH_PLANNER

#include <central64/search/AStarSearch.hpp>
#include <central64/search/JumpPointSearch.hpp>
#include <central64/search/MixedAStarSearch.hpp>
#include <central64/search/MixedJumpPointSearch.hpp>
#include <central64/smoothing/NoSmoothing.hpp>
#include <central64/smoothing/GreedySmoothing.hpp>
#include <central64/smoothing/TentpoleSmoothing.hpp>

namespace central64 {

/// An enumeration of grid path search methods.
enum class SearchMethod
{
    AStar,             ///< A* Search
    JumpPoint,         ///< Jump Point Search
    BoundedJumpPoint,  ///< Bounded Jump Point Search
    MixedAStar,        ///< Mixed A* Search
    MixedJumpPoint     ///< Mixed Jump Point Search
};

/// An enumeration of grid path smoothing methods.
enum class SmoothingMethod
{
    No,       ///< No Smoothing
    Greedy,   ///< Greedy Smoothing
    Tentpole  ///< Tentpole Smoothing
};

/// A class for `L`-neighbor grid path planning that consolidates grid creation, search, and smoothing.
template <int L>
class PathPlanner
{
public:
    /// Create a path planning object from a set of traverable and obstructed cells (`inputCells`) and the specified cell `alignment` option.
    /// Path planning is performed using the specified search method (`searchMethod`),
    /// and followed by an application of the specified smoothing method (`smoothingMethod).
    /// If `centralize` is true, then the object produces central grid paths; otherwise it produces regular grid paths.
    /// If `fromSource` is true, then the object produces paths arranged from source to sample; otherwise from sample to source.
    PathPlanner(const std::vector<std::vector<bool>>& inputCells,
                CellAlignment alignment = CellAlignment::Center,
                SearchMethod searchMethod = SearchMethod::AStar,
                SmoothingMethod smoothingMethod = SmoothingMethod::Tentpole,
                bool centralize = true,
                bool fromSource = true);

    int NeighborhoodSize() const { return L; }  ///< Get the neighborhood size.

    const Grid2D<L>& Grid() const { return grid_; }               ///< Obtain a const reference to the grid.
    AbstractSearch<L>& Search() { return *searchPtr_; }           ///< Obtain a reference to the path search object.
    AbstractSmoothing<L>& Smoothing() { return *smoothingPtr_; }  ///< Obtain a reference to the path smoothing object.

    bool Centralize() const { return centralize_; }  ///< Check whether the object produces central grid paths.
    bool FromSource() const { return fromSource_; }  ///< Check whether the object produces paths arranged from source to sample.

    /// Compute a path between source coordinates `sourceCoords` and sample coordinates `sampleCoords`.
    /// If no path is found, return an empty vector.
    std::vector<Offset2D> PlanPath(Offset2D sourceCoords, Offset2D sampleCoords);

    /// Conduct an all-nodes search from source coordinates `sourceCoords` to all reachable nodes in the grid.
    /// Paths originating or terminating at the source can then be efficiently sampled using `SamplePath`.
    void SearchAllNodes(Offset2D sourceCoords);

    /// Sample a path between sample coordinates `sampleCoords` and the previously supplied source.
    /// If no path is found, return an empty vector.
    std::vector<Offset2D> SamplePath(Offset2D sampleCoords);

private:
    PathPlanner(const PathPlanner&) = delete;
    PathPlanner& operator=(const PathPlanner&) = delete;

    Grid2D<L> grid_;
    std::unique_ptr<AbstractSearch<L>> searchPtr_{};
    std::unique_ptr<AbstractSmoothing<L>> smoothingPtr_{};
    bool centralize_;
    bool fromSource_;
};

template <int L>
PathPlanner<L>::PathPlanner(const std::vector<std::vector<bool>>& inputCells,
                            CellAlignment alignment,
                            SearchMethod searchMethod,
                            SmoothingMethod smoothingMethod,
                            bool centralize,
                            bool fromSource)
    : grid_{ inputCells, alignment }
    , centralize_{ centralize }
    , fromSource_{ fromSource }
{
    switch (searchMethod) {
    case SearchMethod::AStar:            searchPtr_ = std::make_unique<         AStarSearch<L>>(grid_);              break;
    case SearchMethod::JumpPoint:        searchPtr_ = std::make_unique<     JumpPointSearch<L>>(grid_);              break;
    case SearchMethod::BoundedJumpPoint: searchPtr_ = std::make_unique<     JumpPointSearch<L>>(grid_, PathCost(8)); break;
    case SearchMethod::MixedAStar:       searchPtr_ = std::make_unique<    MixedAStarSearch<L>>(grid_);              break;
    case SearchMethod::MixedJumpPoint:   searchPtr_ = std::make_unique<MixedJumpPointSearch<L>>(grid_);              break;
    }
    switch (smoothingMethod) {
    case SmoothingMethod::No:       smoothingPtr_ = std::make_unique<      NoSmoothing<L>>(grid_); break;
    case SmoothingMethod::Greedy:   smoothingPtr_ = std::make_unique<  GreedySmoothing<L>>(grid_); break;
    case SmoothingMethod::Tentpole: smoothingPtr_ = std::make_unique<TentpoleSmoothing<L>>(grid_); break;
    }
    assert(searchPtr_);
    assert(smoothingPtr_);
}

template <int L>
std::vector<Offset2D> PathPlanner<L>::PlanPath(Offset2D sourceCoords, Offset2D sampleCoords)
{
    std::vector<Offset2D> pathVertices = searchPtr_->PlanGridPath(sourceCoords, sampleCoords, centralize_, fromSource_);
    smoothingPtr_->SmoothPath(pathVertices);
    return pathVertices;
}

template <int L>
void PathPlanner<L>::SearchAllNodes(Offset2D sourceCoords)
{
    searchPtr_->SearchAllNodes(sourceCoords);
}

template <int L>
std::vector<Offset2D> PathPlanner<L>::SamplePath(Offset2D sampleCoords)
{
    std::vector<Offset2D> pathVertices = searchPtr_->SampleGridPath(sampleCoords, centralize_, fromSource_);
    smoothingPtr_->SmoothPath(pathVertices);
    return pathVertices;
}

/// Create the recommended 4-neighbor path planner.
inline PathPlanner<4> CreateBestPathPlanner4(const std::vector<std::vector<bool>>& inputCells,
                                             CellAlignment alignment = CellAlignment::Center,
                                             bool fromSource = true)
{
    return PathPlanner<4>(inputCells, alignment, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, true, fromSource);
}

/// Create the recommended 8-neighbor path planner.
inline PathPlanner<8> CreateBestPathPlanner8(const std::vector<std::vector<bool>>& inputCells,
                                             CellAlignment alignment = CellAlignment::Center,
                                             bool fromSource = true)
{
    return PathPlanner<8>(inputCells, alignment, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, true, fromSource);
}

/// Create the recommended 16-neighbor path planner.
inline PathPlanner<16> CreateBestPathPlanner16(const std::vector<std::vector<bool>>& inputCells,
                                               CellAlignment alignment = CellAlignment::Center,
                                               bool fromSource = true)
{
    return PathPlanner<16>(inputCells, alignment, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, true, fromSource);
}

/// Create the recommended 32-neighbor path planner.
inline PathPlanner<32> CreateBestPathPlanner32(const std::vector<std::vector<bool>>& inputCells,
                                               CellAlignment alignment = CellAlignment::Center,
                                               bool fromSource = true)
{
    return PathPlanner<32>(inputCells, alignment, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, true, fromSource);
}

/// Create the recommended 64-neighbor path planner.
inline PathPlanner<64> CreateBestPathPlanner64(const std::vector<std::vector<bool>>& inputCells,
                                               CellAlignment alignment = CellAlignment::Center,
                                               bool fromSource = true)
{
    return PathPlanner<64>(inputCells, alignment, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, true, fromSource);
}

/// Create the overall recommended path planner.
inline auto CreateBestOverallPathPlanner(const std::vector<std::vector<bool>>& inputCells,
                                         CellAlignment alignment = CellAlignment::Center,
                                         bool fromSource = true)
{
    return CreateBestPathPlanner16(inputCells, alignment, fromSource);
}

/// Create the recommended path planner for obtaining the highest quality paths.
inline auto CreateBestQualityPathPlanner(const std::vector<std::vector<bool>>& inputCells,
                                         CellAlignment alignment = CellAlignment::Center,
                                         bool fromSource = true)
{
    return CreateBestPathPlanner64(inputCells, alignment, fromSource);
}

/// Create the recommended path planner for obtaining low quality paths at the highest possible speed.
inline auto CreateFastestOverallPathPlanner(const std::vector<std::vector<bool>>& inputCells,
                                            CellAlignment alignment = CellAlignment::Center,
                                            bool fromSource = true)
{
    return PathPlanner<8>(inputCells, alignment, SearchMethod::BoundedJumpPoint, SmoothingMethod::Tentpole, false, fromSource);
}

}  // namespace

#endif
