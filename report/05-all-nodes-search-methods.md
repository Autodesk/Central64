| [Prev](04-search-methods.md) | [Next](06-library-design.md) |
## 5. All-Nodes Search Methods

An all-nodes (or single-source) search is a procedure in which all reachable nodes are searched from a common source vertex. Once an all-nodes search is complete, shortest grid paths can be efficiency sampled between the source vertex and any reachable sample vertex. All five search methods in the previous section have a corresponding all-nodes method, implemented in Central64 using mostly the same code.

### 5.1 Dijkstra Search

The classic search algorithm by [Dijkstra (1959)](https://link.springer.com/article/10.1007/BF01386390) is the all-nodes version of A* Search, and the foundation of all of the path planning methods in the Central64 library. In contrast to A*, nodes in Dijkstra Search are expanded in order of increasing g-cost rather than f-cost.

### 5.2 Canonical Dijkstra Search

Canonical Dijkstra, the all-nodes version of Jump Point Search, was introduced by [Sturtevant & Rabin (2016)](https://www.ijcai.org/Abstract/16/103).

### 5.3 Bounded Canonical Dijkstra Search

Bounded Canonical Dijkstra is Canonical Dijkstra with a finite jump cost. It is the all-nodes version of Bounded Jump Point Search. The method is unconventional, since there is no clear benefit to bounding the jump cost if all reachable nodes must be searched anyway.

### 5.4 Mixed Dijkstra Search

Mixed Dijkstra is the all-nodes version of Mixed A*. Blocks in Mixed Dijkstra are processed in order of increasing g-cost instead of f-cost.

### 5.5 Mixed Canonical Dijkstra Search

Mixed Canonical Dijkstra is the all-nodes version of Mixed Jump Point Search. Blocks in Mixed Canonical Dijkstra are processed in order of increasing g-cost instead of f-cost.

| [Prev](04-search-methods.md) | [Next](06-library-design.md) |
