| Prev | [Next](01-introduction.md) |
# The Central64 Library: An Overview and Comparison of Grid Path Planning Methods

Technical Report, Version 1

Rhys Goldstein, Autodesk Research

August 2023

## Executive Summary

This report explains the main concepts underlying the Central64 library, and presents an empirical comparison of the various supported path planning methods and variations. The project has several notable features. First, it demonstrates how central grid path planning can be applied not only to A* and Dijkstra's algorithm, but also to variations of Jump Point Search. Second, the library allows the standard rectangular grid neighborhoods up to 64 neighbors to be used in conjunction with all of the implemented methods. Third, a new path smoothing method called Tentpole Smoothing is introduced as an improvement over the traditional Greedy Smoothing algorithm. Based on the empirical study, 16-Neighbor Central Bounded Jump Point Search with Tentpole Smoothing is recommended as the combined method that provides the best overall balance of quality and speed. Several additional recommendations are made based on the empirical results and lessons learned from the project.

**Suggested citation:**

> Goldstein, R. (2023). The Central64 Library: An Overview and Comparison of Grid Path Planning Methods. Technical Report, Version 1. Autodesk Research. Code and report available at [https://github.com/Autodesk/Central64](https://github.com/Autodesk/Central64).

**Additional notes:**

A PDF version of this report can be found via the URL above. The experiment was run with `v1.0.0` of the code. Thanks to Jenmy Zhang from Autodesk Research for helping to clarify and improve this work.

## Table of Contents

[1. Introduction](01-introduction.md)<br/>
[2. Grids](02-grids.md)<br/>
[3. Paths](03-paths.md)<br/>
[4. Search Methods](04-search-methods.md)<br/>
[5. All-Nodes Search Methods](05-all-nodes-search-methods.md)<br/>
[6. Library Design](06-library-design.md)<br/>
[7. Experiment Design](07-experiment-design.md)<br/>
[8. Search Results](08-search-results.md)<br/>
[9. All-Nodes Search Results](09-all-nodes-search-results.md)<br/>
[10. Findings and Recommendations](10-findings-and-recommendations.md)<br/>
[References](11-references.md)<br/>
[Appendix](12-appendix.md)<br/>

| Prev | [Next](01-introduction.md) |
