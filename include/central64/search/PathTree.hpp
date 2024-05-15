#pragma once
#ifndef CENTRAL64_PATH_TREE
#define CENTRAL64_PATH_TREE

#include <central64/grid/Grid2D.hpp>
#include <optional>
#include <queue>

namespace central64 {

/// A representation of the hierarchy or tree of shortest grid paths created during a Dijkstra or heuristic path search.
/// The data structure also tracks the g-cost and h-cost of each node, 
/// and whether the node was initialized for the current search.
template <int L>
class PathTree
{
public:
    static constexpr Offset2D InvalidCoords() { return { -1, -1 }; }  ///< A nil value used to indicate source or sample coordinates that have not been specified.

    explicit PathTree(const Grid2D<L>& grid);  ///< Create a path tree object that references an existing grid object.

    const Grid2D<L>& Grid() const { return *gridPtr_; }  ///< Obtain a const reference to the grid.

    bool IsAllNodesSearch() const;  ///< Check if the current search is an all-nodes (e.g. Dijkstra) search.

    Offset2D SourceCoords() const;  ///< Get the coordinates of the current source vertex, the root of the shortest grid path tree.
    Offset2D SampleCoords() const;  ///< Get the coordinates of the current sample vertex, the other end of the path.

    PathCost GCost(Offset2D coords) const;  ///< Get the g-cost of the node at coordinates `coords`, the grid-based distance to the source.
    PathCost HCost(Offset2D coords) const;  ///< Get the h-cost of the node at coordinates `coords`, the heuristic or estimated distance to the sample.

    Move<L> ParentMove(Offset2D coords) const;  ///< Get the move pointing to the parent of the node at coordinates `coords`.

    void InitializeSearch(Offset2D sourceCoords, Offset2D sampleCoords);  ///< Reset the path tree and begin a new search with the specified source coordinates (`sourceCoords`) and sample coordinates (`sampleCoords`).
    void InitializeAllNodesSearch(Offset2D sourceCoords);                 ///< Reset the path tree and begin a new all-nodes search with the specified source coordinates (`sourceCoords`).

    bool IsSearchNodeInitialized(Offset2D coords) const;  ///< Check whether the node at coordinates `coords` has been initialized for the current search.

    void InitializeDijkstraNode(Offset2D coords);   ///< Initialize the node at coordinates `coords` for the current search, without computing the heuristic.
    void InitializeHeuristicNode(Offset2D coords);  ///< Initialize the node at coordinates `coords` for the current search, computing the heuristic if it is not an all-nodes search.

    void SetGCost(Offset2D coords, PathCost gCost);            ///< Set the g-cost `gCost` of the node at coordinates `coords`.
    void SetParentMove(Offset2D coords, const Move<L>& move);  ///< Set the `move` pointing to the parent of the node at coordinates `coords`.

    std::vector<Offset2D> ExtractRegularPath();   ///< Extract a shortest grid path following the parent moves from the sample vertex to the source vertex.
    void SetSampleCoords(Offset2D sampleCoords);  ///< Set the sample coordinates to `sampleCoords`.

    class DijkstraQueue;
    class HeuristicQueue;

    DijkstraQueue CreateDijkstraQueue() { return DijkstraQueue{ *this }; }     ///< Create a Dijkstra queue associated with this path tree.
    HeuristicQueue CreateHeuristicQueue() { return HeuristicQueue{ *this }; }  ///< Create a heuristic queue associated with this path tree.

private:
    // Check that the source coordinates are valid.
    // Also, if the current search is not an all-nodes search,
    // check that the sample coordinates are valid.
    bool IsSearchCriteriaValid() const;

    // A grouping of attributes associated with each node of the path tree.
    // The purpose of this data structure is to improve cache locality by
    // ensuring vertex-specific search information is located in a contiguous
    // block of memory.
    struct TreeNode {
        uint64_t searchID{ 0 };     // The ID of the search for which the node was last initialized.
        PathCost gCost{};           // The grid-based distance to the source.
        PathCost hCost{};           // The heuristic or estimated distance to the sample.
        int parentMoveIndex{};      // The index of the parent move.
    };

    const Grid2D<L>* gridPtr_;
    Array2D<TreeNode> nodes_;
    uint64_t currentSearchID_;
    Offset2D sourceCoords_;
    Offset2D sampleCoords_;
    bool allNodes_;
};

template <int L>
PathTree<L>::PathTree(const Grid2D<L>& grid)
    : gridPtr_{ &grid }
    , nodes_{ grid.Dims() }
    , currentSearchID_{ 0 }
    , sourceCoords_{ InvalidCoords() }
    , sampleCoords_{ InvalidCoords() }
    , allNodes_{ false }
{
}

template <int L>
inline bool PathTree<L>::IsAllNodesSearch() const
{
    assert(IsSearchCriteriaValid());

    return allNodes_;
}

template <int L>
inline Offset2D PathTree<L>::SourceCoords() const
{
    assert(IsSearchCriteriaValid());

    return sourceCoords_;
}

template <int L>
inline Offset2D PathTree<L>::SampleCoords() const
{
    assert(IsSearchCriteriaValid());

    return sampleCoords_;
}

template <int L>
inline PathCost PathTree<L>::GCost(Offset2D coords) const
{
    assert(IsSearchCriteriaValid());

    // If the node has not been initialized, the recorded g-cost is considered invalid and ignored.
    return IsSearchNodeInitialized(coords) ? nodes_[coords].gCost : PathCost::MaxCost();
}

template <int L>
inline PathCost PathTree<L>::HCost(Offset2D coords) const
{
    assert(IsSearchCriteriaValid());

    // If the node has not been initialized, the recorded h-cost is considered invalid and ignored.
    return IsSearchNodeInitialized(coords) ? nodes_[coords].hCost : PathCost::MaxCost();
}

template <int L>
inline Move<L> PathTree<L>::ParentMove(Offset2D coords) const
{
    assert(IsSearchCriteriaValid());
    assert(IsSearchNodeInitialized(coords));

    return Neighborhood<L>::Moves()[nodes_[coords].parentMoveIndex];
}

template <int L>
inline void PathTree<L>::InitializeSearch(Offset2D sourceCoords, Offset2D sampleCoords)
{
    assert(Grid().Contains(sourceCoords));
    assert(Grid().Contains(sampleCoords));

    ++currentSearchID_;
    sourceCoords_ = sourceCoords;
    sampleCoords_ = sampleCoords;
    allNodes_ = false;
}

template <int L>
inline void PathTree<L>::InitializeAllNodesSearch(Offset2D sourceCoords)
{
    assert(Grid().Contains(sourceCoords));

    ++currentSearchID_;
    sourceCoords_ = sourceCoords;
    sampleCoords_ = InvalidCoords();
    allNodes_ = true;
}

template <int L>
inline bool PathTree<L>::IsSearchNodeInitialized(Offset2D coords) const
{
    assert(IsSearchCriteriaValid());

    return nodes_[coords].searchID == currentSearchID_;
}

template <int L>
inline void PathTree<L>::InitializeDijkstraNode(Offset2D coords)
{
    assert(IsSearchCriteriaValid());

    TreeNode& node = nodes_[coords];
    node.searchID = currentSearchID_;
    node.gCost = PathCost::MaxCost();
    node.hCost = PathCost::MaxCost();
}

template <int L>
inline void PathTree<L>::InitializeHeuristicNode(Offset2D coords)
{
    assert(IsSearchCriteriaValid());

    TreeNode& node = nodes_[coords];
    node.searchID = currentSearchID_;
    node.gCost = PathCost::MaxCost();
    node.hCost = allNodes_ ? PathCost(0) : Neighborhood<L>::StandardCost(SampleCoords() - coords);
}

template <int L>
inline void PathTree<L>::SetGCost(Offset2D coords, PathCost gCost)
{
    assert(IsSearchCriteriaValid());
    assert(IsSearchNodeInitialized(coords));

    nodes_[coords].gCost = gCost;
}

template <int L>
inline void PathTree<L>::SetParentMove(Offset2D coords, const Move<L>& move)
{
    assert(IsSearchCriteriaValid());
    assert(IsSearchNodeInitialized(coords));

    nodes_[coords].parentMoveIndex = move.Index();
}

template <int L>
std::vector<Offset2D> PathTree<L>::ExtractRegularPath()
{
    assert(IsSearchCriteriaValid());

    std::vector<Offset2D> pathVertices{};
    if (Grid().Contains(SampleCoords())) {
        if (GCost(SampleCoords()) < PathCost::MaxCost()) {
            Offset2D coords = SampleCoords();
            while (coords != SourceCoords()) {
                pathVertices.push_back(coords);
                const Offset2D nextCoords = coords + ParentMove(coords).Offset();
                assert(Grid().Contains(nextCoords));
                assert(IsSearchNodeInitialized(nextCoords));
                assert(GCost(coords) - GCost(nextCoords) == ParentMove(coords).Cost());
                coords = nextCoords;
            }
            pathVertices.push_back(coords);
        }
    }
    return pathVertices;
}

template <int L>
void PathTree<L>::SetSampleCoords(Offset2D sampleCoords)
{
    assert(Grid().Contains(sampleCoords));

    sampleCoords_ = sampleCoords;
}

template <int L>
inline bool PathTree<L>::IsSearchCriteriaValid() const
{
    return Grid().Contains(sourceCoords_) && (Grid().Contains(sampleCoords_) || allNodes_);
}

/// A priority queue for which nodes are sorted by g-cost.
/// The same node may be pushed multiple times during the same search,
/// but the g-cost is assumed to decrease with each push.
template <int L>
class PathTree<L>::DijkstraQueue
{
    friend class PathTree<L>;

public:
    std::optional<std::pair<Offset2D, PathCost>> Pop();  ///< Pop the node with the lowest g-cost, and return the coordinates and g-cost.
    void Push(Offset2D coords);                          ///< Push a node with coordinates `coords` onto the queue, looking up its g-cost in the path tree.

private:
    struct QueueNode {
        Offset2D coords;
        PathCost gCost;
    };

    struct LowerPriority {
        bool operator()(const QueueNode& lhs, const QueueNode& rhs) const { return lhs.gCost > rhs.gCost; }
    };

    explicit DijkstraQueue(const PathTree<L>& pathTree) : pathTreePtr_{ &pathTree } {};

    std::priority_queue<QueueNode, std::vector<QueueNode>, LowerPriority> queue_{};

    const PathTree<L>* pathTreePtr_;
};

/// A priority queue for which nodes are sorted by f-cost, the sum of the g-cost and h-cost.
/// The same node may be pushed multiple times during the same search,
/// but the g-cost is assumed to decrease with each push.
template <int L>
class PathTree<L>::HeuristicQueue
{
    friend class PathTree<L>;

public:
    std::optional<std::pair<Offset2D, PathCost>> Pop();  ///< Pop the node with the lowest f-cost, and return the coordinates and f-cost.
    void Push(Offset2D coords);                          ///< Push a node with coordinates `coords` onto the queue, looking up its g-cost and h-cost in the path tree.

private:
    struct QueueNode {
        Offset2D coords;
        PathCost gCost;
        PathCost fCost;
    };

    struct LowerPriority {
        bool operator()(const QueueNode& lhs, const QueueNode& rhs) const { return lhs.fCost > rhs.fCost; }
    };

    explicit HeuristicQueue(const PathTree<L>& pathTree) : pathTreePtr_{ &pathTree } {};

    std::priority_queue<QueueNode, std::vector<QueueNode>, LowerPriority> queue_{};

    const PathTree<L>* pathTreePtr_;
};

template <int L>
inline std::optional<std::pair<Offset2D, PathCost>> PathTree<L>::DijkstraQueue::Pop()
{
    std::optional<std::pair<Offset2D, PathCost>> node{};

    // Loop over and discard all nodes for which the g-cost in the internal queue does not match the g-cost in the path tree.
    // It is assumed that such an entry exists in the internal queue because the node was subsequently pushed with a lower g-cost.
    while (!queue_.empty() ? queue_.top().gCost != pathTreePtr_->nodes_[queue_.top().coords].gCost : false) {
        assert(pathTreePtr_->nodes_[queue_.top().coords].gCost < queue_.top().gCost);  // Ensure the g-cost was not increased.
        queue_.pop();
    }

    // Pop and return the top entry in the internal queue, which is now guaranteed to have a g-cost that matches the one in the path tree.
    if (!queue_.empty()) {
        node = { queue_.top().coords, queue_.top().gCost };
        queue_.pop();
    }
    return node;
}

template <int L>
inline void PathTree<L>::DijkstraQueue::Push(Offset2D coords)
{
    const PathCost gCost = pathTreePtr_->nodes_[coords].gCost;
    queue_.push({ coords, gCost });
}

template <int L>
inline std::optional<std::pair<Offset2D, PathCost>> PathTree<L>::HeuristicQueue::Pop()
{
    std::optional<std::pair<Offset2D, PathCost>> node{};

    // Loop over and discard all nodes for which the g-cost in the internal queue does not match the g-cost in the path tree.
    // It is assumed that such an entry exists in the internal queue because the node was subsequently pushed with a lower g-cost.
    while (!queue_.empty() ? queue_.top().gCost != pathTreePtr_->nodes_[queue_.top().coords].gCost : false) {
        assert(pathTreePtr_->nodes_[queue_.top().coords].gCost < queue_.top().gCost);  // Ensure the g-cost was not increased.
        queue_.pop();
    }

    // Pop and return the top entry in the internal queue, which is now guaranteed to have a g-cost that matches the one in the path tree.
    if (!queue_.empty()) {
        node = { queue_.top().coords, queue_.top().fCost };
        queue_.pop();
    }
    return node;
}

template <int L>
inline void PathTree<L>::HeuristicQueue::Push(Offset2D coords)
{
    const PathCost gCost = pathTreePtr_->nodes_[coords].gCost;
    const PathCost hCost = pathTreePtr_->nodes_[coords].hCost;
    assert(gCost != PathCost::MaxCost());
    assert(hCost != PathCost::MaxCost());
    queue_.push({ coords, gCost, gCost + hCost });
}

}  // namespace

#endif
