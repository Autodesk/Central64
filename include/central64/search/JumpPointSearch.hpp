#pragma once
#ifndef CENTRAL64_JUMP_POINT_SEARCH
#define CENTRAL64_JUMP_POINT_SEARCH

#include <central64/search/BasicSearch.hpp>

namespace central64 {

/// A derived class for the `L`-neighbor jump point search method.
template <int L>
class JumpPointSearch : public BasicSearch<L>
{
public:
    /// Create a jump point search object that references an existing grid object,
    /// where the jump cost is optionally bounded by `jumpCost`. The default maximum
    /// jump cost specifies a conventional jump point search, whereas a lower jump cost
    /// specifies a bounded jump point search. A bounded jump point search with a jump
    /// cost of zero is also known as Canonical A*.
    explicit JumpPointSearch(const Grid2D<L>& grid, PathCost jumpCost = PathCost::MaxCost());

    JumpPointSearch(JumpPointSearch&&) = default;             ///< Ensure the default move constructor is public.
    JumpPointSearch& operator=(JumpPointSearch&&) = default;  ///< Ensure the default move assignment operator is public.

    const PathCost JumpCost() const { return jumpCost_; }  ///< Get the jump cost.

    std::string MethodName() const;          ///< Get the name of the search method, which depends on whether the search is bounded.
    std::string AllNodesMethodName() const;  ///< Get the name of the all-nodes version of the search method, which depends on whether the search is bounded.

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

    // Perform a jump traversal starting from the initial coordinates `initialCoords`,
    // and advancing until the maximum g-cost `maxGCost` is reached. If the active
    // frontier parameter `activeFrontier` is true, then the nodes at the end of the
    // jump must be expanded.
    void Jump(Offset2D initialCoords, PathCost maxGCost, bool activeFrontier);

    bool IsSearchNodeInitialized(Offset2D coords) const { return BasicSearch<L>::IsSearchNodeInitialized(coords); }
    void InitializeDijkstraNode(Offset2D coords) { BasicSearch<L>::InitializeDijkstraNode(coords); }
    void InitializeHeuristicNode(Offset2D coords) { BasicSearch<L>::InitializeHeuristicNode(coords); }
    using DijkstraQueue = typename BasicSearch<L>::DijkstraQueue;
    using HeuristicQueue = typename BasicSearch<L>::HeuristicQueue;
    DijkstraQueue CreateDijkstraQueue() { return BasicSearch<L>::CreateDijkstraQueue(); }
    HeuristicQueue CreateHeuristicQueue() { return BasicSearch<L>::CreateHeuristicQueue(); }

    void ExpandSearchNode(Offset2D coords, Connections<L> successors) { BasicSearch<L>::ExpandSearchNode(coords, successors); }

    PathCost jumpCost_;
};

template <int L>
JumpPointSearch<L>::JumpPointSearch(const Grid2D<L>& grid, PathCost jumpCost)
    : BasicSearch<L>{ grid }
    , jumpCost_{ jumpCost }
{
}

template <int L>
inline std::string JumpPointSearch<L>::MethodName() const
{
    return (jumpCost_ == PathCost::MaxCost()) ? "Jump Point Search" :
                                                "Bounded Jump Point Search";
}

template <int L>
inline std::string JumpPointSearch<L>::AllNodesMethodName() const
{
    return (jumpCost_ == PathCost::MaxCost()) ? "Canonical Dijkstra Search" : 
                                                "Bounded Canonical Dijkstra Search";
}

template <int L>
bool JumpPointSearch<L>::ProcessSearchNode(Offset2D coords, PathCost fCost)
{
    // Obtain the current g-cost of the sample node, if it exists.
    const PathCost sampleCost = IsAllNodesSearch() ? PathCost::MaxCost() : Tree().GCost(SampleCoords());

    // Terminate the search if the current f-cost has exceeded the g-cost of the sample.
    const bool searching = (fCost <= sampleCost);

    // If the search is to continue, initiate a jump traversal.
    if (searching) {
        const PathCost maxGCost = std::min(sampleCost, Tree().GCost(coords) + jumpCost_);
        const bool activeFrontier = (maxGCost < sampleCost);
        Jump(coords, maxGCost, activeFrontier);
    }
    return searching;
}

template <int L>
void JumpPointSearch<L>::Jump(Offset2D initialCoords, PathCost maxGCost, bool activeFrontier)
{
    const Move<L>& parentMove = Tree().ParentMove(initialCoords);
    const Move<L> forwardMove = -parentMove;
    Offset2D coords = initialCoords;
    bool jumping = true;
    while (jumping) {
        const Connections<L> canonicalSuccessors = Grid().CanonicalSuccessors(coords, parentMove);
        const Connections<L> forcedSuccessors = Grid().ForcedSuccessors(coords, parentMove);
        PathCost gCost = Tree().GCost(coords);
        if (!forcedSuccessors.IsAllDisconnected() || gCost >= maxGCost) {
            // Either forced successors have been encountered, or the maximum g-cost for
            // this jump has been reached. If forced successors have been encountered,
            // or if the frontier is active, then expand the current node. Either way,
            // terminate the jump traversal.
            if (gCost < maxGCost || activeFrontier) {
                ExpandSearchNode(coords, forcedSuccessors | canonicalSuccessors);
            }
            jumping = false;
        }
        else {
            // The jump traversal is to continue. If the forward direction of the traversal 
            // is odd, then initialize and update any canonical successors that are not in
            // the forward direction. Initiate a jump traversal in these even directions.
            if (forwardMove.IsOdd()) {
                for (const Move<L>& move : Neighborhood<L>::Moves()) {
                    if (canonicalSuccessors.IsConnected(move)) {
                        if (move != forwardMove) {
                            const Offset2D successorCoords = coords + move.Offset();
                            if (!IsSearchNodeInitialized(successorCoords)) {
                                InitializeHeuristicNode(successorCoords);
                            }
                            const PathCost successorGCost = gCost + move.Cost();
                            if (successorGCost < Tree().GCost(successorCoords)) {
                                Tree().SetGCost(successorCoords, successorGCost);
                                Tree().SetParentMove(successorCoords, -move);
                                Jump(successorCoords, maxGCost, activeFrontier);
                            }
                        }
                    }
                }
            }

            // If there is no canonical successor in the forward direction,
            // terminate the jump traversal. Otherwise, advance by one move
            // in the forward direction.
            if (!canonicalSuccessors.IsConnected(forwardMove)) {
                jumping = false;
            }
            else {
                // Advance to the next node and ensure it is initialized.
                coords += forwardMove.Offset();
                if (!IsSearchNodeInitialized(coords)) {
                    InitializeHeuristicNode(coords);
                }
                
                // Obtain the g-cost of a path to the current node from the previous node.
                // If this is now the shortest path to the current node, update the path tree
                // and continue the jump traversal.
                gCost += forwardMove.Cost();
                jumping = (gCost < Tree().GCost(coords));
                if (jumping) {
                    Tree().SetGCost(coords, gCost);
                    Tree().SetParentMove(coords, parentMove);
                }
            }
        }
    }
}

}  // namespace

#endif
