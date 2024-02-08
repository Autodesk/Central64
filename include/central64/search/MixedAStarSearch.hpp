#pragma once
#ifndef CENTRAL64_MIXED_A_STAR_SEARCH
#define CENTRAL64_MIXED_A_STAR_SEARCH

#include <central64/search/MixedSearch.hpp>

namespace central64 {

/// A derived class for the `L`-neighbor mixed A* search method with `M`-by-`M` blocks.
template <int L, int M = 8>
class MixedAStarSearch : public MixedSearch<L, M>
{
public:
    explicit MixedAStarSearch(const Grid2D<L>& grid) : MixedSearch<L>{ grid } {}  ///< Create a mixed A* search object that references an existing grid object.

    MixedAStarSearch(MixedAStarSearch&&) = default;             ///< Ensure the default move constructor is public.
    MixedAStarSearch& operator=(MixedAStarSearch&&) = default;  ///< Ensure the default move assignment operator is public.

    std::string MethodName() const { return "Mixed A* Search"; }                ///< Get the name of the search method.
    std::string AllNodesMethodName() const { return "Mixed Dijkstra Search"; }  ///< Get the name of the all-nodes version of the search method.

    const Grid2D<L>& Grid() const { return MixedSearch<L>::Grid(); }
    bool IsAllNodesSearch() const { return MixedSearch<L>::IsAllNodesSearch(); }
    Offset2D SourceCoords() const { return MixedSearch<L>::SourceCoords(); }
    Offset2D SampleCoords() const { return MixedSearch<L>::SampleCoords(); }
    bool Centralize() const { return MixedSearch<L>::Centralize(); }
    bool FromSource() const { return MixedSearch<L>::FromSource(); }
    PathTree<L>& Tree() const { return MixedSearch<L>::Tree(); }
    PathFlow<L>& Flow() const { return MixedSearch<L>::Flow(); }

private:
    // Process the node at coordinates `coords`.
    void ProcessSearchNode(Offset2D coords);

    bool IsSearchNodeInitialized(Offset2D coords) const { return MixedSearch<L>::IsSearchNodeInitialized(coords); }
    void InitializeDijkstraNode(Offset2D coords) { MixedSearch<L>::InitializeDijkstraNode(coords); }
    void InitializeHeuristicNode(Offset2D coords) { MixedSearch<L>::InitializeHeuristicNode(coords); }
    using DijkstraQueue = typename MixedSearch<L>::DijkstraQueue;
    using HeuristicQueue = typename MixedSearch<L>::HeuristicQueue;
    DijkstraQueue CreateDijkstraQueue() { return MixedSearch<L>::CreateDijkstraQueue(); }
    HeuristicQueue CreateHeuristicQueue() { return MixedSearch<L>::CreateHeuristicQueue(); }

    bool IsSearchBlockInitialized(Offset2D compressedCoords) const { return MixedSearch<L>::IsSearchBlockInitialized(compressedCoords); }
    void InitializeSearchBlock(Offset2D compressedCoords) { MixedSearch<L>::InitializeSearchBlock(compressedCoords); }
    void ExpandSearchNode(Offset2D coords, Connections<L> successors) { MixedSearch<L>::ExpandSearchNode(coords, successors); }
    void UpdateBorderNode(Offset2D coords) { MixedSearch<L>::UpdateBorderNode(coords); }
    Offset2D CompressedDims() const { return MixedSearch<L>::CompressedDims(); }
    Offset2D ContainingDims(Offset2D compressedCoords) const { return MixedSearch<L>::ContainingDims(compressedCoords); }
    Offset2D CompressCoords(Offset2D coords) const { return MixedSearch<L>::CompressCoords(coords); }
    Offset2D DecompressCoords(Offset2D compressedCoords) const { return MixedSearch<L>::DecompressCoords(compressedCoords); }
};

template <int L, int M>
void MixedAStarSearch<L, M>::ProcessSearchNode(Offset2D coords)
{
    // Simply expand the current node. There is no need to
    // check termination conditions because all reachable
    // nodes in the current block are to be processed.
    ExpandSearchNode(coords, Grid().Neighbors(coords));
}

}  // namespace

#endif
