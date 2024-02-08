#pragma once
#ifndef CENTRAL64_MIXED_SEARCH
#define CENTRAL64_MIXED_SEARCH

#include <central64/search/AbstractSearch.hpp>

namespace central64 {

/// An abstract base class for mixed `L`-neighbor grid path search methods.
/// A mixed search partitions the grid into `M`-by-`M` search blocks.
/// A heuristic search is employed at the outer level, between blocks.
/// An all-nodes search is employed at the inner level, within blocks.
template <int L, int M = 8>
class MixedSearch : public AbstractSearch<L>
{
    static_assert(M ==  1 || M == 2 || M == 4 || M == 8 || M == 16 || M == 32 || M == 64,
                  "Block size must be 1, 2, 4, 8, 16, 32, or 64.");

public:
    virtual ~MixedSearch() = default;  ///< Ensure the destructor is virtual, since the class is to be inherited.

    const Grid2D<L>& Grid() const { return AbstractSearch<L>::Grid(); }
    bool IsAllNodesSearch() const { return AbstractSearch<L>::IsAllNodesSearch(); }
    Offset2D SourceCoords() const { return AbstractSearch<L>::SourceCoords(); }
    Offset2D SampleCoords() const { return AbstractSearch<L>::SampleCoords(); }
    bool Centralize() const { return AbstractSearch<L>::Centralize(); }
    bool FromSource() const { return AbstractSearch<L>::FromSource(); }
    PathTree<L>& Tree() const { return AbstractSearch<L>::Tree(); }
    PathFlow<L>& Flow() const { return AbstractSearch<L>::Flow(); }

protected:
    explicit MixedSearch(const Grid2D<L>& grid);  ///< Create a mixed search object that references an existing grid object.

    MixedSearch(MixedSearch&&) = default;             ///< Ensure the default move constructor is protected because the class is to be inherited.
    MixedSearch& operator=(MixedSearch&&) = default;  ///< Ensure the default move assignment operator is protected because the class is to be inherited.

    void PerformSearch();                                ///< Perform the current path search, populating the shortest grid path tree.
    void ProcessSearchBlock(Offset2D compressedCoords);  ///< Process the block at compressed coordinates `compressedCoords`.

    virtual void ProcessSearchNode(Offset2D coords) = 0;  ///< Process the node at coordinates `coords`.

    bool IsSearchBlockInitialized(Offset2D compressedCoords) const;  ///< Check whether the block at compressed coordinates `compressedCoords` has been initialized.
    void InitializeSearchBlock(Offset2D compressedCoords);           ///< Initialize the block at compressed coordinates `compressedCoords`.

    void ExpandSearchNode(Offset2D coords, Connections<L> successors);  ///< Expand the node at coordinates `coords` according to the set of `successors`.
    void UpdateBorderNode(Offset2D coords);                             ///< Update the search based on the cost of the border node at coordinates `coords`.

    Offset2D CompressedDims() const { return compressedDims_; }  ///< Get the compressed coordinate dimensions, the number of blocks in each dimension.
    Offset2D ContainingDims(Offset2D compressedCoords) const;    ///< Obtain the dimensions of the sub-array that barely encompasses the block at compressed coordinates `compressedCoords`.
    Offset2D CompressCoords(Offset2D coords) const;              ///< Obtain the compressed coordinates of the block that contains the coordinates `coords`.
    Offset2D DecompressCoords(Offset2D compressedCoords) const;  ///< Obtain the coordinates of the node at the lower corner of the block at compressed coordinates `compressedCoords`.

    bool IsSearchNodeInitialized(Offset2D coords) const { return AbstractSearch<L>::IsSearchNodeInitialized(coords); }
    void InitializeDijkstraNode(Offset2D coords) { AbstractSearch<L>::InitializeDijkstraNode(coords); }
    void InitializeHeuristicNode(Offset2D coords) { AbstractSearch<L>::InitializeHeuristicNode(coords); }
    using DijkstraQueue = typename AbstractSearch<L>::DijkstraQueue;
    using HeuristicQueue = typename AbstractSearch<L>::HeuristicQueue;
    DijkstraQueue CreateDijkstraQueue() { return AbstractSearch<L>::CreateDijkstraQueue(); }
    HeuristicQueue CreateHeuristicQueue() { return AbstractSearch<L>::CreateHeuristicQueue(); }

private:
    static constexpr int innerBits = M - 1;                                      // The set of bits that identify nodes within blocks.
    static constexpr int outerBits = std::numeric_limits<int>::max() - (M - 1);  // The set of bits that identify the blocks themselves.
    
    Offset2D compressedDims_;  // The declaration of compressedDims_ must preceed the declaration of the Array2D member variables.

    HeuristicQueue outerQueue_;  // The main search queue, which prioritizes blocks.

    Array2D<PathCost> outerFCosts_;                     // A lowest f-cost for each block.
    Array2D<DijkstraQueue> innerSearchQueues_;          // A search queue for each block.
    Array2D<std::vector<Offset2D>> innerBorderQueues_;  // A first-in-first-out queue for border nodes of each block.
};

template <int L, int M>
MixedSearch<L, M>::MixedSearch(const Grid2D<L>& grid)
    : AbstractSearch<L>{ grid }
    , compressedDims_{ std::max(0, Grid().Dims().X() - 1)/M + 1, std::max(0, Grid().Dims().Y() - 1)/M + 1 }
    , outerQueue_{ CreateHeuristicQueue() }
    , outerFCosts_{ compressedDims_ }
    , innerSearchQueues_{ compressedDims_, CreateDijkstraQueue() }
    , innerBorderQueues_{ compressedDims_ }
{
}

template <int L, int M>
inline bool MixedSearch<L, M>::IsSearchBlockInitialized(Offset2D compressedCoords) const
{
    return IsSearchNodeInitialized(DecompressCoords(compressedCoords));
}

template <int L, int M>
inline void MixedSearch<L, M>::InitializeSearchBlock(Offset2D compressedCoords)
{
    const Offset2D cornerCoords = DecompressCoords(compressedCoords);
    const Offset2D containingDims = ContainingDims(compressedCoords);

    // Initialize every node in the block. Compute the heuristic
    // to prioritize blocks at the outer level of the search.
    for (int y = cornerCoords.Y(); y < containingDims.Y(); ++y) {
        for (int x = cornerCoords.X(); x < containingDims.X(); ++x) {
            InitializeHeuristicNode({ x, y });
        }
    }

    // Reset the minimum f-cost and the queues for this block. 
    outerFCosts_[compressedCoords] = PathCost::MaxCost();
    innerSearchQueues_[compressedCoords] = CreateDijkstraQueue();
    innerBorderQueues_[compressedCoords].clear();
}

template <int L, int M>
void MixedSearch<L, M>::PerformSearch()
{
    // Initialize the block that contains the source node.
    const Offset2D sourceCompressedCoords = CompressCoords(SourceCoords());
    InitializeSearchBlock(sourceCompressedCoords);

    // If the current search is not an all-nodes search, and if the block
    // that contains the sample coordinates is different from the block that 
    // contains the source, initialize the block that contains the sample.
    if (!IsAllNodesSearch()) {
        const Offset2D sampleCompressedCoords = CompressCoords(SampleCoords());
        if (sampleCompressedCoords != sourceCompressedCoords) {
            InitializeSearchBlock(sampleCompressedCoords);
        }
    }

    // Create a new main queue to ensure it is empty, and expand the source node.
    outerQueue_ = CreateHeuristicQueue();
    Tree().SetGCost(SourceCoords(), PathCost(0));
    ExpandSearchNode(SourceCoords(), Grid().Neighbors(SourceCoords()));

    // Repeatedly process one of the blocks with the lowest f-cost, until 
    // the queue is empty or the termination condition has been satisfied.
    bool searching = true;
    while (searching) {
        const std::optional<std::pair<Offset2D, PathCost>> outerNode = outerQueue_.Pop();
        searching = outerNode.has_value();
        if (searching) {
            const Offset2D outerCoords = outerNode->first;
            const PathCost outerFCost = outerNode->second;

            // Terminate the search if it is not an all-nodes search, and 
            // if the current f-cost has exceeded the g-cost of the sample.
            if (!IsAllNodesSearch()) {
                searching = (outerFCost <= Tree().GCost(SampleCoords()));
            }

            // If the search is to continue, process the current block and
            // reset its minimum f-cost.
            if (searching) {
                const Offset2D compressedCoords = CompressCoords(outerCoords);
                ProcessSearchBlock(compressedCoords);
                outerFCosts_[compressedCoords] = PathCost::MaxCost();
            }
        }
    }
}

template <int L, int M>
void MixedSearch<L, M>::ProcessSearchBlock(Offset2D compressedCoords)
{
    // Process all border nodes in first-in-first-out order.
    // Border nodes are nodes with a parent in a different block.
    // For neighborhoods larger than 8, these nodes are not
    // necessarily on the edge of a block.
    std::vector<Offset2D>& innerBorderQueue = innerBorderQueues_[compressedCoords];
    for (const Offset2D& coords : innerBorderQueue) {
        assert(CompressCoords(coords) == compressedCoords);
        ProcessSearchNode(coords);
    }
    innerBorderQueue.clear();

    // Repeatedly process one of the nodes in the block with the lowest g-cost,
    // until the inner queue is empty.
    bool searching = true;
    while (searching) {
        const std::optional<std::pair<Offset2D, PathCost>> innerSearchNode = innerSearchQueues_[compressedCoords].Pop();
        searching = innerSearchNode.has_value();
        if (searching) {
            const Offset2D coords = innerSearchNode->first;
            assert(CompressCoords(coords) == compressedCoords);
            ProcessSearchNode(coords);
        }
    }
}

template <int L, int M>
void MixedSearch<L, M>::ExpandSearchNode(Offset2D coords, Connections<L> successors)
{
    const PathCost gCost = Tree().GCost(coords);
    for (const Move<L>& move : Neighborhood<L>::Moves()) {
        if (successors.IsConnected(move)) {
            // Obtain the coordinates of the current successor,
            // and ensure the encompassing block is initialized.
            const Offset2D successorCoords = coords + move.Offset();
            const Offset2D successorCompressedCoords = CompressCoords(successorCoords);
            if (!IsSearchBlockInitialized(successorCompressedCoords)) {
                InitializeSearchBlock(successorCompressedCoords);
            }

            // Obtain the g-cost of a path to the successor via the node being expanded.
            // If this is now the shortest path to the successor, update the successor
            // node in the path tree and push it onto the inner queue. Also update the
            // minimum f-cost and queue at the outer level, if necessary.
            const PathCost successorGCost = gCost + move.Cost();
            if (successorGCost < Tree().GCost(successorCoords)) {
                Tree().SetGCost(successorCoords, successorGCost);
                Tree().SetParentMove(successorCoords, -move);
                innerSearchQueues_[successorCompressedCoords].Push(successorCoords);
                const PathCost successorFCost = successorGCost + Tree().HCost(successorCoords);
                if (successorFCost < outerFCosts_[successorCompressedCoords]) {
                    outerFCosts_[successorCompressedCoords] = successorFCost;
                    outerQueue_.Push(successorCoords);
                }
            }
        }
    }
}

template <int L, int M>
inline void MixedSearch<L, M>::UpdateBorderNode(Offset2D coords)
{
    // Push the border node onto the queue of the encompassing block.
    // Also update the minimum f-cost and queue at the outer level,
    // if necessary.
    const Offset2D compressedCoords = CompressCoords(coords);
    innerBorderQueues_[compressedCoords].push_back(coords);
    const PathCost fCost = Tree().GCost(coords) + Tree().HCost(coords);
    if (fCost < outerFCosts_[compressedCoords]) {
        outerFCosts_[compressedCoords] = fCost;
        outerQueue_.Push(coords);
    }
}

template <int L, int M>
inline Offset2D MixedSearch<L, M>::ContainingDims(Offset2D compressedCoords) const
{
    assert(Grid().Contains(DecompressCoords(compressedCoords)));

    return { std::min(Grid().Dims().X(), (compressedCoords.X() + 1)*M & outerBits), 
             std::min(Grid().Dims().Y(), (compressedCoords.Y() + 1)*M & outerBits) };
}

template <int L, int M>
inline Offset2D MixedSearch<L, M>::CompressCoords(Offset2D coords) const
{
    return { coords.X()/M, coords.Y()/M };
}

template <int L, int M>
inline Offset2D MixedSearch<L, M>::DecompressCoords(Offset2D compressedCoords) const
{
    return { compressedCoords.X()*M, compressedCoords.Y()*M };
}

}  // namespace

#endif
