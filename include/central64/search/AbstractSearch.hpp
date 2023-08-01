#pragma once
#ifndef CENTRAL64_ABSTRACT_SEARCH
#define CENTRAL64_ABSTRACT_SEARCH

#include <central64/search/PathTree.hpp>
#include <central64/search/PathFlow.hpp>

namespace central64 {

/// An abstract base class for `L`-neighbor grid path search methods.
template <int L>
class AbstractSearch
{
public:
    virtual ~AbstractSearch() = default;  ///< Ensure the destructor is virtual, since the class is to be inherited.

    const Grid2D<L>& Grid() const { return grid_; }  ///< Obtain a const reference to the grid.

    /// Compute a path between source coordinates `sourceCoords` and sample coordinates `sampleCoords`.
    /// If `centralize` is true, then compute a central grid path; otherwise compute a regular grid path.
    /// If `fromSource` is true, then arrange the path from source to sample; otherwise from sample to source.
    /// If no path is found, return an empty vector.
    std::vector<Offset2D> PlanGridPath(Offset2D sourceCoords,
                                       Offset2D sampleCoords,
                                       bool centralize = true,
                                       bool fromSource = true);

    /// Conduct an all-nodes search from source coordinates `sourceCoords` to all reachable nodes in the grid.
    /// Paths originating or terminating at the source can then be efficiently sampled using `SampleGridPath`.
    void SearchAllNodes(Offset2D sourceCoords);

    /// Sample a path between sample coordinates `sampleCoords` and the previously supplied source.
    /// If `centralize` is true, then compute a central grid path; otherwise compute a regular grid path.
    /// If `fromSource` is true, then arrange the path from source to sample; otherwise from sample to source.
    /// If no path is found, return an empty vector.
    std::vector<Offset2D> SampleGridPath(Offset2D sampleCoords,
                                         bool centralize = true,
                                         bool fromSource = true);

    bool IsAllNodesSearch() const { return Tree().IsAllNodesSearch(); }  ///< Check if the current search is an all-nodes (e.g. Dijkstra) search.

    Offset2D SourceCoords() const { return Tree().SourceCoords(); }  ///< Get the coordinates of the current source vertex, the root of the shortest grid path tree.
    Offset2D SampleCoords() const { return Tree().SampleCoords(); }  ///< Get the coordinates of the current sample vertex, the other end of the path.

    bool Centralize() const { return centralize_; }  ///< Check whether the most recently generated path is a central grid path.
    bool FromSource() const { return fromSource_; }  ///< Check whether the most recently generated path is arranged from source to sample.

    PathTree<L>& Tree() const { return *pathTreePtr_; }  ///< Obtain a reference to the path tree object.
    PathFlow<L>& Flow() const { return *pathFlowPtr_; }  ///< Obtain a reference to the path flow object.

    virtual std::string MethodName() const = 0;          ///< Get the name of the search method.
    virtual std::string AllNodesMethodName() const = 0;  ///< Get the name of the all-nodes version of the search method.

protected:
    explicit AbstractSearch(const Grid2D<L>& grid);  ///< Create an abstract search object that references an existing grid object.

    AbstractSearch(AbstractSearch&&) = default;             ///< Ensure the default move constructor is protected because the class is to be inherited.
    AbstractSearch& operator=(AbstractSearch&&) = default;  ///< Ensure the default move assignment operator is protected because the class is to be inherited.

    bool IsSearchNodeInitialized(Offset2D coords) const { return Tree().IsSearchNodeInitialized(coords); }  ///< Check whether the node at coordinates `coords` has been initialized for the current search.

    void InitializeDijkstraNode(Offset2D coords) { Tree().InitializeDijkstraNode(coords); }    ///< Initialize the node at coordinates `coords` for the current search, without computing the heuristic.
    void InitializeHeuristicNode(Offset2D coords) { Tree().InitializeHeuristicNode(coords); }  ///< Initialize the node at coordinates `coords` for the current search, computing the heuristic if it is not an all-nodes search.

    using DijkstraQueue = typename PathTree<L>::DijkstraQueue;    ///< A type alias for Dijkstra queues.
    using HeuristicQueue = typename PathTree<L>::HeuristicQueue;  ///< A type alias for heuristic queues.

    DijkstraQueue CreateDijkstraQueue() { return Tree().CreateDijkstraQueue(); }     ///< Create a Dijkstra queue associated with this path tree.
    HeuristicQueue CreateHeuristicQueue() { return Tree().CreateHeuristicQueue(); }  ///< Create a heuristic queue associated with this path tree.

    virtual void PerformSearch() = 0;  ///< Perform the current path search, populating the shortest grid path tree.

private:
    AbstractSearch(const AbstractSearch&) = delete;
    AbstractSearch& operator=(const AbstractSearch&) = delete;

    Grid2D<L> grid_;
    std::unique_ptr<PathTree<L>> pathTreePtr_;
    std::unique_ptr<PathFlow<L>> pathFlowPtr_;
    bool centralize_;
    bool fromSource_;
};

template <int L>
std::vector<Offset2D> AbstractSearch<L>::PlanGridPath(Offset2D sourceCoords,
                                                      Offset2D sampleCoords,
                                                      bool centralize,
                                                      bool fromSource)
{
    std::vector<Offset2D> pathVertices{};
    centralize_ = centralize;
    fromSource_ = fromSource;
    Tree().InitializeSearch(sourceCoords, sampleCoords);
    if (Grid().Contains(SourceCoords()) && Grid().Contains(SampleCoords())) {
        // Perform the path search as specified in the derived class.
        PerformSearch();

        // Extract either a central grid path using the path flow object, or a regular grid path using the path tree object.
        if (Centralize()) {
            pathVertices = Flow().ExtractCentralPath();
        }
        else {
            pathVertices = Tree().ExtractRegularPath();
        }

        // Reverse the path if needed so that it starts at the source and ends at the sample.
        if (FromSource()) {
            std::reverse(std::begin(pathVertices), std::end(pathVertices));
        }
    }
    return pathVertices;
}

template <int L>
void AbstractSearch<L>::SearchAllNodes(Offset2D sourceCoords)
{
    Tree().InitializeAllNodesSearch(sourceCoords);
    if (Grid().Contains(SourceCoords())) {
        // Perform the path search as specified in the derived class.
        PerformSearch();
    }
}

template <int L>
std::vector<Offset2D> AbstractSearch<L>::SampleGridPath(Offset2D sampleCoords,
                                                        bool centralize,
                                                        bool fromSource)
{
    std::vector<Offset2D> pathVertices{};
    centralize_ = centralize;
    fromSource_ = fromSource;
    Tree().SetSampleCoords(sampleCoords);
    if (Grid().Contains(SourceCoords()) && Grid().Contains(SampleCoords())) {
        // Extract either a central grid path using the path flow object, or a regular grid path using the path tree object.
        if (Centralize()) {
            pathVertices = Flow().ExtractCentralPath();
        }
        else {
            pathVertices = Tree().ExtractRegularPath();
        }

        // Reverse the path if needed so that it starts at the source and ends at the sample.
        if (FromSource()) {
            std::reverse(std::begin(pathVertices), std::end(pathVertices));
        }
    }
    return pathVertices;
}

template <int L>
AbstractSearch<L>::AbstractSearch(const Grid2D<L>& grid)
    : grid_{ grid }
    , pathTreePtr_{}
    , pathFlowPtr_{}
    , centralize_{ true }
    , fromSource_{ true }
{
    pathTreePtr_ = std::make_unique<PathTree<L>>(grid);
    pathFlowPtr_ = std::make_unique<PathFlow<L>>(grid, *pathTreePtr_);
}

}  // namespace

#endif
