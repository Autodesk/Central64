#pragma once
#ifndef CENTRAL64_A_STAR_SEARCH
#define CENTRAL64_A_STAR_SEARCH

#include <central64/search/BasicSearch.hpp>

namespace central64 {

/// A derived class for the `L`-neighbor A* search method.
template <int L>
class AStarSearch : public BasicSearch<L>
{
public:
    explicit AStarSearch(const Grid2D<L>& grid) : BasicSearch<L>{ grid } {}  ///< Create an A* search object that references an existing grid object.

    AStarSearch(AStarSearch&&) = default;             ///< Ensure the default move constructor is public.
    AStarSearch& operator=(AStarSearch&&) = default;  ///< Ensure the default move assignment operator is public.

    std::string MethodName() const { return "A* Search"; }                ///< Get the name of the search method.
    std::string AllNodesMethodName() const { return "Dijkstra Search"; }  ///< Get the name of the all-nodes version of the search method.

    const Grid2D<L>& Grid() const { return BasicSearch<L>::Grid(); }
    bool IsAllNodesSearch() const { return BasicSearch<L>::IsAllNodesSearch(); }
    Offset2D SourceCoords() const { return BasicSearch<L>::SourceCoords(); }
    Offset2D SampleCoords() const { return BasicSearch<L>::SampleCoords(); }
    bool Centralize() const { return BasicSearch<L>::Centralize(); }
    bool FromSource() const { return BasicSearch<L>::FromSource(); }
    PathTree<L>& Tree() const { return BasicSearch<L>::Tree(); }
    PathFlow<L>& Flow() const { return BasicSearch<L>::Flow(); }

private:
    // Process the node at coordinates `coords`, given its f-cost `fCost`.
    bool ProcessSearchNode(Offset2D coords, PathCost fCost);

    bool IsSearchNodeInitialized(Offset2D coords) const { return BasicSearch<L>::IsSearchNodeInitialized(coords); }
    void InitializeDijkstraNode(Offset2D coords) { BasicSearch<L>::InitializeDijkstraNode(coords); }
    void InitializeHeuristicNode(Offset2D coords) { BasicSearch<L>::InitializeHeuristicNode(coords); }
    using DijkstraQueue = typename BasicSearch<L>::DijkstraQueue;
    using HeuristicQueue = typename BasicSearch<L>::HeuristicQueue;
    DijkstraQueue CreateDijkstraQueue() { return BasicSearch<L>::CreateDijkstraQueue(); }
    HeuristicQueue CreateHeuristicQueue() { return BasicSearch<L>::CreateHeuristicQueue(); }

    void ExpandSearchNode(Offset2D coords, Connections<L> successors) { BasicSearch<L>::ExpandSearchNode(coords, successors); }
};

template <int L>
bool AStarSearch<L>::ProcessSearchNode(Offset2D coords, PathCost fCost)
{
    // Terminate the search if it is not an all-nodes search, and if the
    // current f-cost has reached or exceeded the g-cost of the sample.
    // A central grid path search must process all nodes with a cost less
    // than or equal to that of the sample. A regular grid path search
    // must process all nodes with a cost strictly less than the sample.
    const bool searching = IsAllNodesSearch() ? true :
                                                Centralize() ? (fCost <= Tree().GCost(SampleCoords())) :
                                                               (fCost < Tree().GCost(SampleCoords()));

    // If the search is to continue, expand the current node.
    if (searching) {
        ExpandSearchNode(coords, Grid().Neighbors(coords));
    }
    return searching;
}

}  // namespace

#endif
