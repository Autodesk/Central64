#pragma once
#ifndef CENTRAL64_GREEDY_SMOOTHING
#define CENTRAL64_GREEDY_SMOOTHING

#include <central64/smoothing/AbstractSmoothing.hpp>

namespace central64 {

/// A derived class for the greedy smoothing method.
template <int L>
class GreedySmoothing : public AbstractSmoothing<L>
{
public:
    explicit GreedySmoothing(const Grid2D<L>& grid) : AbstractSmoothing<L>{ grid } {}  ///< Create a greedy smoothing object that references an existing grid object.

    GreedySmoothing(GreedySmoothing&&) = default;             ///< Ensure the default move constructor is public.
    GreedySmoothing& operator=(GreedySmoothing&&) = default;  ///< Ensure the default move assignment operator is public.

    virtual void SmoothPath(std::vector<Offset2D>& pathVertices);  ///< Perform the smoothing operation on a path represented as a vector of 2D offsets (`pathVertices`).

    virtual std::string MethodName() const { return "Greedy Smoothing"; }  ///< Get the name of the smoothing method.

    const Grid2D<L>& Grid() const { return AbstractSmoothing<L>::Grid(); }
};

template <int L>
void GreedySmoothing<L>::SmoothPath(std::vector<Offset2D>& pathVertices)
{
    // Create a vector of the path vertices to be retained.
    std::vector<Offset2D> retainedVertices{};
    
    // Create an index A that advances from the first
    // path vertex to just beyond the final path vertex.
    int indexA = 0;
    int vertexCount = int(pathVertices.size());
    while (indexA < vertexCount) {
        // Retain the vertex at index A.
        retainedVertices.push_back(pathVertices[indexA]);

        // If index A points to the final path vertex, advance it.
        // Otherwise, use line-of-sight checks to find the next vertex to retain.
        if (indexA == vertexCount - 1) {
            ++indexA;
        }
        else {
            // Create an index B that advances from the first position after
            // A to the first vertex for which the line of sight is broken.
            int indexB = indexA + 1;
            Offset2D uniqueOffset = pathVertices[indexB] - pathVertices[indexA];
            bool lineOfSight = true;
            while (lineOfSight && indexB < vertexCount) {
                // Obtain the offset between the vertex of B and the preceding vertex.
                const Offset2D offset = pathVertices[indexB] - pathVertices[indexB - 1];

                // Perform the line-of-sight check only if the offsets have not all been identical.
                if (offset != uniqueOffset) {
                    uniqueOffset = { 0, 0 };
                    lineOfSight = Grid().LineOfSight(pathVertices[indexA], pathVertices[indexB]);
                }

                // If the line of sight is not yet broken, advance B.
                if (lineOfSight) {
                    ++indexB;
                }
            }

            // Advance A to the first position before the vertex of B.
            indexA = indexB - 1;
        }
    }

    // Replace the original path vertices with the retained vertices.
    pathVertices = retainedVertices;
}

}  // namespace

#endif
