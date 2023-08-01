| [Prev](03-paths.md) | [Next](05-all-nodes-search-methods.md) |
## 4. Search Methods

Central64 provides the following search methods: ***A\* Search***, ***Jump Point Search***, ***Bounded Jump Point Search***, ***Mixed A\* Search***, and ***Mixed Jump Point Search***. Each method can be applied using regular or central grid path planning, which is indicated by prefixing the method names with "Regular" or "Central". All of these are heuristic methods, meaning that they input both a source vertex and a sample (goal) vertex, and that they use a heuristic to process nodes from the source toward the sample. Central64 uses the standard grid distance, the length of the shortest possible grid path, as the heuristic.

### 4.1 A* Search

A* is the classic heuristic search method introduced by [Hart et al. (1968)](https://ieeexplore.ieee.org/document/4082128). Nodes stored in a priority queue so that they can be processed in order of increasing f-cost: the sum of the grid distance to the source vertex (g-cost) and a lower-bound estimate of the distance to the sample vertex (h-cost, or heuristic). For A*, central path planning requires a modification to the termination condition of the search. With Regular A*, the search is continued until all nodes with an f-cost less than the sample are processed. With Central A*, all nodes with an f-cost less than or equal to the sample must be processed.

An illustrated introduction to A* is provided by [Patel (2020b)](https://www.redblobgames.com/pathfinding/a-star/introduction.html).

### 4.2 Jump Point Search

Jump Point Search is a variant of A* that accelerates the search procedure by "jumping" over swathes of vertices when processing the next node in the priority queue. These swaths of vertices fan out over empty regions, terminating at obstacles and jump points. A jump point is a vertex where a shortest grid path may end up wrapping around a corner or surface of an obstacle. In the original Jump Point Search method introduced by [Harabor & Grastien (2011)](https://ojs.aaai.org/index.php/AAAI/article/view/7994), only jump points are added to the priority queue. In Central64, jump points are expanded immediately and their neighboring successors are added to the queue. In both conventions, vertices within each swath are traversed quickly in a series of scanlines. Jump Point Search inherently requires all nodes with an f-cost less than or equal to the sample vertex to be processed, so no change to the termination condition is needed to produce a central grid path.

An illustrated introduction to Jump Point Search is provided by [Witmer (2013)](https://zerowidth.com/2013/a-visual-explanation-of-jump-point-search.html).

### 4.3 Bounded Jump Point Search

Bounded Jump Point Search is similar to Jump Point Search, but imposes an arbitrary upper limit on the extent to which vertices are "jumped" over. This upper bound prevents the search from traversing vast swathes of obstacle-free space in cases where the sample vertex can be reached relatively directly. The Central64 library implements Bounded Jump Point Search by adding a "jump cost" parameter to the Jump Point Search method. The default jump cost of infinity yields Jump Point Search. A finite jump cost yields Bounded Jump Point Search. A jump cost of zero is a special case of Bounded Jump Point Search known as Canonical A*, which avoids jumping but nevertheless improves on A* by restricting the search to canonical paths.

Both Jump Point Search methods produce canonical paths, which place odd neighborhood moves closer to the source and even neighborhood moves closer to the sample. Central64 adopts the convention that the move [1, 0] is even, and other neighborhood moves alternate odd, even, odd, etc., in a clockwise or counterclockwise direction. Variations of Jump Point Search are traditionally implemented using the 8-neighborhood, in which case diagonal moves are placed closer to the source.

Bounded Jump Point Search, Canonical A*, and the general concept of canonical paths were introduced by [Sturtevant & Rabin (2016)](https://www.ijcai.org/Abstract/16/103) for the 8-neighborhood. [Rivera et al. (2020)](https://jair.org/index.php/jair/article/view/11383) extended Jump Point Search, Canonical A*, and the concept of canonical paths to larger standard rectangular grid neighborhoods. The Central64 library extends these methods in the other direction by including the 4-neighborhood as well.

### 4.4 Mixed A* Search

Mixed A* partitions the grid into square blocks, and searches exhaustively within the current block before progressing to the next. In essence, the method uses the non-heuristic Dijkstra Search within blocks and the heuristic A* Search between blocks. Mixed A* is similar to the Block A* method by [Yap et al. (2011)](https://www.aaai.org/ocs/index.php/AAAI/AAAI11/paper/view/3449), except that Block A* precalculates distances through each block whereas Mixed A* performs the entire search on the fly.

### 4.5 Mixed Jump Point Search

Mixed Jump Point Search is the same as Mixed A*, except that it uses the Canonical Dijkstra method within each block. The method is similar in principle to Bounded Jump Point Search, with blocks serving as an alternative way of bounding jump distances.

A theoretical advantage of Mixed A* and Mixed Jump Point Search, compared with A* and Bounded Jump Point Search respectively, is that only one node per block needs to be inserted and retrieved from the priority queue. A heap-based priority queue is common to all the search methods implemented in the Central64 library, so the mixed search methods promise to reduce the number of heap operations. Another theoretical advantage of the mixed approaches is an improvement in cache locality, since the search data associated with a single block will presumably be close in memory.

| [Prev](03-paths.md) | [Next](05-all-nodes-search-methods.md) |
