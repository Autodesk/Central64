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
    explicit MixedAStarSearch(const Grid2D<L>& grid) : MixedSearch{ grid } {}  ///< Create a mixed A* search object that references an existing grid object.

    MixedAStarSearch(MixedAStarSearch&&) = default;             ///< Ensure the default move constructor is public.
    MixedAStarSearch& operator=(MixedAStarSearch&&) = default;  ///< Ensure the default move assignment operator is public.

    std::string MethodName() const { return "Mixed A* Search"; }                ///< Get the name of the search method.
    std::string AllNodesMethodName() const { return "Mixed Dijkstra Search"; }  ///< Get the name of the all-nodes version of the search method.

private:
    // Process the node at coordinates `coords`.
    void ProcessSearchNode(Offset2D coords);
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
