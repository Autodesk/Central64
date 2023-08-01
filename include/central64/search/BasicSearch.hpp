#pragma once
#ifndef CENTRAL64_BASIC_SEARCH
#define CENTRAL64_BASIC_SEARCH

#include <central64/search/AbstractSearch.hpp>

namespace central64 {

/// An abstract base class for basic `L`-neighbor grid path search methods.
template <int L>
class BasicSearch : public AbstractSearch<L>
{
public:
    virtual ~BasicSearch() = default;  ///< Ensure the destructor is virtual, since the class is to be inherited.

protected:
    explicit BasicSearch(const Grid2D<L>& grid);  ///< Create a basic search object that references an existing grid object.

    BasicSearch(BasicSearch&&) = default;             ///< Ensure the default move constructor is protected because the class is to be inherited.
    BasicSearch& operator=(BasicSearch&&) = default;  ///< Ensure the default move assignment operator is protected because the class is to be inherited.

    void PerformSearch();  ///< Perform the current path search, populating the shortest grid path tree.

    void ExpandSearchNode(Offset2D coords, Connections<L> successors);  ///< Expand the node at coordinates `coords` according to the set of `successors`.

    virtual bool ProcessSearchNode(Offset2D coords, PathCost fCost) = 0;  ///< Process the node at coordinates `coords`, given its f-cost `fCost`.

private:
    HeuristicQueue queue_;
};

template <int L>
BasicSearch<L>::BasicSearch(const Grid2D<L>& grid)
    : AbstractSearch{ grid }
    , queue_{ CreateHeuristicQueue() }
{
}

template <int L>
void BasicSearch<L>::PerformSearch()
{
    // Initialize the source node.
    InitializeHeuristicNode(SourceCoords());

    // If the current search is not an all-nodes search, and if the sample
    // coordinates are different than the source, initialize the sample node.
    if (!IsAllNodesSearch()) {
        if (SampleCoords() != SourceCoords()) {
            InitializeHeuristicNode(SampleCoords());
        }
    }

    // Create a new queue to ensure it is empty, and expand the source node.
    queue_ = CreateHeuristicQueue();
    Tree().SetGCost(SourceCoords(), PathCost(0));
    ExpandSearchNode(SourceCoords(), Grid().Neighbors(SourceCoords()));

    // Repeatedly process one of the nodes with the lowest f-cost, until 
    // the queue is empty or the termination conditions have been satisfied.
    bool searching = true;
    while (searching) {
        const std::optional<std::pair<Offset2D, PathCost>> node = queue_.Pop();
        searching = node.has_value();
        if (searching) {
            const Offset2D coords = node->first;
            const PathCost fCost = node->second;
            searching = ProcessSearchNode(coords, fCost);
        }
    }
}

template <int L>
void BasicSearch<L>::ExpandSearchNode(Offset2D coords, Connections<L> successors)
{
    const PathCost gCost = Tree().GCost(coords);
    for (const Move<L>& move : Neighborhood<L>::Moves()) {
        if (successors.IsConnected(move)) {
            // Obtain the coordinates of the current successor, and ensure the node is initialized.
            const Offset2D successorCoords = coords + move.Offset();
            if (!IsSearchNodeInitialized(successorCoords)) {
                InitializeHeuristicNode(successorCoords);
            }

            // Obtain the g-cost of a path to the successor via the node being expanded.
            // If this is now the shortest path to the successor, update the successor
            // node in the path tree and push it onto the queue.
            const PathCost successorGCost = gCost + move.Cost();
            if (successorGCost < Tree().GCost(successorCoords)) {
                Tree().SetGCost(successorCoords, successorGCost);
                Tree().SetParentMove(successorCoords, -move);
                queue_.Push(successorCoords);
            }
        }
    }
}

}  // namespace

#endif
