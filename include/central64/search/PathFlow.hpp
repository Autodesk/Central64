#pragma once
#ifndef CENTRAL64_PATH_COUNTING
#define CENTRAL64_PATH_COUNTING

#include <central64/search/PathTree.hpp>
#include <central64/grid/PathCount.hpp>

namespace central64 {

/// A representation of the directed acyclic or flow graph of all shortest grid paths between a source and sample point.
/// The data structure is used for counting the number of shortest grid paths that traverse each vertex.
/// A central grid path can then be extracted by selecting the vertices with the highest counts.
template <int L>
class PathFlow
{
public:
    explicit PathFlow(const Grid2D<L>& grid, PathTree<L>& pathTree);  ///< Create a path flow object that references existing grid and path tree objects.

    const Grid2D<L>& Grid() const { return *gridPtr_; }  ///< Obtain a const reference to the grid.

    Connections<L> Predecessors(Offset2D coords) const;  ///< Get the predecessors of the node at coordinates `coords`, the connections that lead to the source.
    Connections<L> Successors(Offset2D coords) const;    ///< Get the successors of the node at coordinates `coords`, the connections that lead to the sample.

    PathCount CountFromSource(Offset2D coords) const;  ///< Get the number of shortest grid paths between the node at coordinates `coords` and the source.
    PathCount CountFromSample(Offset2D coords) const;  ///< Get the number of shortest grid paths between the node at coordinates `coords` and the sample.

    std::vector<Offset2D> ExtractCentralPath();  ///< Compute the predecessors and successors from the current state of the path tree, and extract a central grid path.

private:
    ///< Obtain a reference to the path tree object.
    PathTree<L>& Tree() const { return *pathTreePtr_; }

    // Initialize the counting operation for the current search.
    void InitializeCounting() { ++currentCountingID_; }

    // Check if counting has been initialized for the node at coordinates `coords`.
    bool IsCountingNodeInitialized(Offset2D coords) const { return countingIDs_[coords] == currentCountingID_; }

    // Initialize the counting operation for the node at coordinates `coords`.
    void InitializeCountingNode(Offset2D coords);

    // Use the g-costs in the path tree to compute the predecessors and successors that form the flow graph.
    // The procedures begins at the sample coordinates `sampleCoords`.
    void ConnectAllPaths(Offset2D sampleCoords);

    // Count the number of shortest grid paths for each vertex, in one direction, recording the results in `counts`.
    // If the sample coordinates are assigned to `sourceCoords` and the predecessors are assigned to `successorGraph`,
    // then paths are counted from sample to source. This operation must be performed first.
    // If the source coordinates are assigned to `sourceCoords` and the successors are assigned to `successorGraph`,
    // then paths are counted from source to sample. This operation must be performed second.
    void CountPaths(Array2D<PathCount>& counts, Offset2D sourceCoords, const Array2D<Connections<L>>& successorGraph);

    const Grid2D<L>* gridPtr_;
    PathTree<L>* pathTreePtr_;
    Array2D<Connections<L>> predecessorGraph_;
    Array2D<Connections<L>> successorGraph_;
    Array2D<PathCount> countsFromSource_;
    Array2D<PathCount> countsFromSample_;
    Array2D<uint64_t> countingIDs_;
    uint64_t currentCountingID_;
};

template <int L>
PathFlow<L>::PathFlow(const Grid2D<L>& grid, PathTree<L>& pathTree)
    : gridPtr_{ &grid }
    , pathTreePtr_{ &pathTree }
    , predecessorGraph_{ grid.Dims() }
    , successorGraph_{ grid.Dims() }
    , countsFromSource_{ grid.Dims() }
    , countsFromSample_{ grid.Dims() }
    , countingIDs_{ grid.Dims(), 0 }
    , currentCountingID_{ 0 }
{
}

template <int L>
inline Connections<L> PathFlow<L>::Predecessors(Offset2D coords) const
{
    // If the node has not been initialized, the recorded predecessors are considered invalid and ignored.
    return IsCountingNodeInitialized(coords) ? predecessorGraph_[coords] : Connections<L>{};
}

template <int L>
inline Connections<L> PathFlow<L>::Successors(Offset2D coords) const
{
    // If the node has not been initialized, the recorded successors are considered invalid and ignored.
    return IsCountingNodeInitialized(coords) ? successorGraph_[coords] : Connections<L>{};
}

template <int L>
inline PathCount PathFlow<L>::CountFromSource(Offset2D coords) const
{
    // If the node has not been initialized, the path counts from the source are considered invalid and ignored.
    return IsCountingNodeInitialized(coords) ? countsFromSource_[coords] : PathCount{};
}

template <int L>
inline PathCount PathFlow<L>::CountFromSample(Offset2D coords) const
{
    // If the node has not been initialized, the path counts from the sample are considered invalid and ignored.
    return IsCountingNodeInitialized(coords) ? countsFromSample_[coords] : PathCount{};
}

template <int L>
std::vector<Offset2D> PathFlow<L>::ExtractCentralPath()
{
    std::vector<Offset2D> pathVertices{};
    InitializeCounting();

    // Proceed to extract a central grid path only if the search was successful.
    if (Tree().GCost(Tree().SampleCoords()) < PathCost::MaxCost()) {

        // Use the g-costs produced during the search to construct the flow graph representing all shortest grid paths.
        ConnectAllPaths(Tree().SampleCoords());
        assert(IsCountingNodeInitialized(Tree().SourceCoords()));  // The source should have been reached and initialized.

        // Count paths in both directions. Due to the trick of negating g-costs, paths must 
        // be counted first from sample to source, and then from source to sample.
        CountPaths(countsFromSample_, Tree().SampleCoords(), predecessorGraph_);
        CountPaths(countsFromSource_, Tree().SourceCoords(), successorGraph_);

        // Begin extracting the central grid path at the sample.
        Offset2D coords = Tree().SampleCoords();

        // Continue extracting the central grid path until the source is reached.
        while (coords != Tree().SourceCoords()) {
            assert(IsCountingNodeInitialized(coords));        // The current node should been initialized.
            assert(countsFromSource_[coords] > PathCount{});  // The current node should have at least one shortest grid path from the source.
            assert(countsFromSample_[coords] > PathCount{});  // The current node should have at least one shortest grid path from the sample.

            // Add the current vertex to the path.
            pathVertices.push_back(coords);

            // Find the predecessor vertex with the highest traversal count.
            PathCount highestPredecessorCount{};
            Offset2D nextCoords = coords;
            const Connections<L> predecessors = Predecessors(coords);
            for (const Move<L>& move : Neighborhood<L>::Moves()) {
                if (predecessors.IsConnected(move)) {
                    // Obtain the coordinates of the current predecessor.
                    const Offset2D predecessorCoords = coords + move.Offset();

                    assert(IsCountingNodeInitialized(predecessorCoords));        // The predecessor should have been initialized.
                    assert(countsFromSource_[predecessorCoords] > PathCount{});  // The predecessor should have at least one shortest grid path from the source.
                    assert(countsFromSample_[predecessorCoords] > PathCount{});  // The predecessor should have at least one shortest grid path from the sample.

                    // The number of paths that traverse this predecessor is the product of the counts in each direction.
                    const PathCount predecessorCount = countsFromSource_[predecessorCoords]*countsFromSample_[predecessorCoords];

                    // If the traversal count is the highest so far, record it along with the predecessor coordinates.
                    if (predecessorCount > highestPredecessorCount) {
                        highestPredecessorCount = predecessorCount;
                        nextCoords = predecessorCoords;
                    }
                }
            }
            assert(nextCoords != coords);
            coords = nextCoords;
        }
        pathVertices.push_back(coords);
    }
    return pathVertices;
}

template <int L>
inline void PathFlow<L>::InitializeCountingNode(Offset2D coords)
{
    countingIDs_[coords] = currentCountingID_;
    predecessorGraph_[coords].DisconnectAll();
    successorGraph_[coords].DisconnectAll();
    countsFromSource_[coords] = PathCount{};
    countsFromSample_[coords] = PathCount{};
}

template <int L>
void PathFlow<L>::ConnectAllPaths(Offset2D sampleCoords)
{
    // Initialize the sample and add it to a stack.
    InitializeCountingNode(sampleCoords);
    std::vector<Offset2D> stack{};
    stack.push_back(sampleCoords);

    // Process the next node in the stack until the stack is empty.
    while (!stack.empty()) {
        const Offset2D coords = stack.back();
        stack.pop_back();
        const PathCost gCost = Tree().GCost(coords);
        Connections<L>& predecessors = predecessorGraph_[coords];
        const Connections<L> neighbors = Grid().Neighbors(coords);
        for (const Move<L>& move : Neighborhood<L>::Moves()) {
            if (neighbors.IsConnected(move)) {
                // Obtain the coordinates and g-cost of the current neighbor.
                const Offset2D neighborCoords = coords + move.Offset();
                const PathCost neighborGCost = Tree().GCost(neighborCoords);

                assert(neighborGCost >= gCost - move.Cost());  // It should be impossible for the neighbor to have a g-cost less than the current g-cost minus the move cost.

                // If the neighbor has a g-cost equal to the current g-cost minus the move cost,
                // then (1) initialize the neighbor and add it the stack if needed, and (2) include
                // the connection as part of the flow graph representing all shortest grid paths.
                if (neighborGCost == gCost - move.Cost()) {
                    if (!IsCountingNodeInitialized(neighborCoords)) {
                        InitializeCountingNode(neighborCoords);
                        stack.push_back(neighborCoords);
                    }
                    predecessors.Connect(move);
                    successorGraph_[neighborCoords].Connect(-move);
                }
            }
        }
    }
}

template <int L>
void PathFlow<L>::CountPaths(Array2D<PathCount>& counts, Offset2D sourceCoords, const Array2D<Connections<L>>& successorGraph)
{
    // Create a queue to ensure nodes are counted in a viable order.
    // It is assumed that counting is performed first from sample to source,
    // and then from source to sample. With this assumption, negating the 
    // g-costs ensures that any node with the lowest g-cost is one for which
    // the preceding nodes have already been processed.
    auto queue = Tree().CreateDijkstraQueue();
    Tree().SetGCost(sourceCoords, -Tree().GCost(sourceCoords));  // Negate the g-cost.

    // Set the initial count to 1, and push the first node.
    counts[sourceCoords] = PathCount::UnitCount();
    queue.Push(sourceCoords);

    // Process the next node in the queue until the counting process is complete.
    bool counting = true;
    while (counting) {
        const std::optional<std::pair<Offset2D, PathCost>> node = queue.Pop();
        counting = node.has_value();
        if (counting) {
            const Offset2D coords = node->first;
            const PathCount count = counts[coords];
            const Connections<L> successors = successorGraph[coords];
            assert(IsCountingNodeInitialized(coords));
            assert(count > PathCount{});
            for (const Move<L>& move : Neighborhood<L>::Moves()) {
                if (successors.IsConnected(move)) {
                    // Obtain the coordinates of the current successor.
                    const Offset2D successorCoords = coords + move.Offset();

                    // If the successor has not yet been visited, (1) negate its g-cost,
                    // (2) transfer the current count to it, and (3) push it onto the queue.
                    // Otherwise, update its count.
                    if (counts[successorCoords] == PathCount{}) {
                        Tree().SetGCost(successorCoords, -Tree().GCost(successorCoords));
                        assert(Tree().GCost(successorCoords) == Tree().GCost(coords) + move.Cost());
                        counts[successorCoords] = count;
                        queue.Push(successorCoords);
                    }
                    else {
                        counts[successorCoords] += count;
                    }
                    assert(counts[successorCoords] > PathCount{});
                }
            }
        }
    }
}

}  // namespace

#endif
