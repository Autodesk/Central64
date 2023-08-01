| [Prev](05-all-nodes-search-methods.md) | [Next](07-experiment-design.md) |
## 6. Library Design

The Central64 library includes a path planning class called `PathPlanner<L>`, which builds upon the `Neighborhood<L>` abstract class, the `Grid2D<L>` class, the search method class hierarchy, and the smoothing method class hierarchy.

### 6.1 Path Planners

A path planner bundles the various path planning elements of the Central64 library into a single object. It can be created using an instruction of the form below.

```cpp
    auto planner = PathPlanner<L>{ inputCells,
                                   alignment,
                                   searchMethod,
                                   smoothingMethod,
                                   centralize,
                                   fromSource }; 
```

The template parameter `L` is the size of the neighborhood, either `4`, `8`, `16`, `32`, or `64`.

The `inputCells` argument is a vector of vector of booleans representing the blocked (`true` or `1`) and traversable (`false` or `0`) cells of the environment. The code below defines the input cells for the grid shown in Section 2.

```cpp
    std::vector<std::vector<bool>> inputCells = {
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
        {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    };
```

The `alignment` argument is one of the following enumeration values: `CellAlignment::Center` (default) or `CellAlignment::Corner`.

The `searchMethod` argument is one of the following: `SearchMethod::AStar` (default), `SearchMethod::JumpPoint`, `SearchMethod::BoundedJumpPoint`, `SearchMethod::MixedAStar`, or `SearchMethod::MixedJumpPoint`.

The `smoothingMethod` argument is one of the following: `SmoothingMethod::No` (no smoothing), `SmoothingMethod::Greedy`, or `SmoothingMethod::Tentpole` (default).

The `centralize` argument is set to `true` (default) to turn on path centralization by path counting, which results in a (possibly smoothed) central grid path. Otherwise the result is a (possibly smoothed) regular grid path.

The `fromSource` argument is set to `true` (default) to produce a path from the source vertex to the sample vertex. Otherwise the path vertices are arranged from sample to source.

An alternative way to construct a path planner object is to use the function `CreateBestOverallPathPlanner`, which accepts the `inputCells`, `alignment`, and `fromSource` arguments and sets all other parameters to the recommended combination. As explained in later sections, this recommended combination is 16-Neighbor Central Bounded Jump Point Search with Tentpole Smoothing.

Once constructed, the path planner object can be used to obtains paths between pairs of vertices.

```cpp
    auto path0 = planner.PlanPath({8, 1}, {13, 6});
    auto path1 = planner.PlanPath({0, 0}, {13, 6});
    auto path2 = planner.PlanPath({3, 6}, {10, 4});
    auto path3 = planner.PlanPath({0, 6}, {13, 4});
```

If it is known that many paths will share a common endpoint, then the same path planner object can be used to conduct an all-nodes search from that source vertex. Paths can then be efficiently sampled by supplying sample points.

```cpp
    planner.SearchAllNodes({6, 3});
    auto path4 = planner.SamplePath({0, 0});
    auto path5 = planner.SamplePath({0, 6});
    auto path6 = planner.SamplePath({13, 0});
    auto path7 = planner.SamplePath({13, 6});
```

The all-nodes search is conducted using the method corresponding with the selected heuristic search. For example, a path planner constructed using `SearchMethod::JumpPoint` (Jump Point Search) will conduct all-nodes searches using Canonical Dijkstra.

The resulting paths are represented as vectors of vertex coordinates. If no path is found between the specified endpoints, the vector is empty.

### 6.2 Neighborhoods and Grids

If needed, the various path planning elements of the library can be used directly instead of via a path planner object. These elements include the neighborhoods and grids described here, as well as the search and smoothing methods described further below.

The `Neighborhood<L>` class is an abstract class that stores information about each standard rectangular grid neighborhood of size `L`. This information includes the list of `L` neighborhood moves and the travel cost associated with each move. The travel cost of each neighborhood move is computed at compile time.

The travel cost between two neighboring vertices is represented internally as an integer. The values of these integers are closely proportional to the straight-line distance between the vertices, but they are scaled such that a grid spacing of 1.0 corresponds with the integer 2040. The reason for using integers internally is to avoid floating-point rounding errors that would otherwise make equally long grid paths seem different in length. The reason why 2040 was chosen as the scale factor is because it is a multiple of 408, and it is known that 577/408 is an excellent approximation of the square root of 2. Any multiple of 408 will support highly accurate grid path length comparisons for the 8-neighborhood, and multiplying by 5 improves accuracy for the larger neighborhoods.

The neighborhood class can be used to compute standard grid distances between arbitrary pairs of vertices, the shortest possible distance using only neighborhood moves and assuming no obstacles.

The `Grid2D<L>` class represents a grid-based environment. It is constructed using the same `inputCells` and `alignment` arguments as the path planner.

```cpp
    Grid2D<L> grid{ inputCells, alignment };
```

A grid object cannot perform path planning on its own, but it can be used to perform line-of-sight tests between arbitrary pairs of vertices.

### 6.3 Search Methods

The classes which perform path search operations are implemented using inheritance. The search classes are constructed using a `Grid2D<L>` object, and provide functions that produce grid paths. The search method hierarchy has the following inheritance structure:

- `AbstractSearch<L>`
  - `BasicSearch<L>`
    - `AStarSearch<L>`
    - `JumpPointSearch<L>`
  - `MixedSearch<L, M>`
    - `MixedAStarSearch<L, M>`
    - `MixedJumpPointSearch<L, M>`

The `AbstractSearch<L>` class is an abstract base class for all search methods. It contains a `PathTree<L>` object that records the data produced by each search. It also contains a `PathFlow<L>` object that (1) uses this data to generate a directed acyclic graph of all shortest grid paths, (2) counts the number of paths that traverse each vertex, and (3) uses the traversal counts to produce a central grid path.

The `BasicSearch<L>` class is an abstract class for basic or single-level search methods. It implements the outer loop of the search procedure, which repeatedly selects the next node to be processed. The class also provides a function that expands a search node given a set of neighboring successors.

The `AStarSearch<L>` class inherits from `BasicSearch<L>` and implements A* and Dijkstra's algorithm. The class essentially indicates that processing the next node is equivalent to expanding it.

The `JumpPointSearch<L>` class inherits from `BasicSearch<L>` and implements Jump Point Search, Bounded Jump Point Search, Canonical Dijkstra, and Bounded Canonical Dijkstra. The class provides the jumping operation that traverses a swath of the environment each time a node is processed.

The `MixedSearch<L, M>` class is an abstract class of mixed (or two-level) search methods. It introduces a template parameter `M` that describes the width of each search block as a number of vertices. This block size parameter must be `1`, `2`, `4`, `8`, `16`, `32`, or `64`. The class also provides various functions for identifying and working with `M`-by-`M` blocks, as well as a function for expanding a search node.

The `MixedAStarSearch<L, M>` class inherits from `MixedSearch<L, M>` and implements Mixed A* and Mixed Dijkstra. The class essentially indicates that processing the next node is equivalent to expanding it.

The `MixedJumpPointSearch<L, M>` class inherits from `MixedSearch<L, M>` and implements Mixed Jump Point Search and Mixed Canonical Dijkstra. The class provides the jumping operation confined to the current block.

### 6.4 Smoothing Methods

After a regular or central grid path is produced using one of the above path search classes, the path can be smoothed. The classes which perform path smoothing operations are also implemented using inheritance. The smoothing classes are constructed using a `Grid2D<L>` object. They provide a function that smooths a supplied path by removing vertices from it while ensuring the path remains valid. The smoothing method hierarchy has the following inheritance structure:

- `AbstractSmoothing<L>`
  - `NoSmoothing<L>`
  - `GreedySmoothing<L>`
  - `TentpoleSmoothing<L>`

The `AbstractSmoothing<L>` class is an abstract base class for all smoothing methods. The derived classes `NoSmoothing<L>`, `GreedySmoothing<L>`, and `TentpoleSmoothing<L>` perform no smoothing, greedy smoothing, and tentpole smoothing respectively.

| [Prev](05-all-nodes-search-methods.md) | [Next](07-experiment-design.md) |
