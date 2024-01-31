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

    // Create an iterator A that advances from the first
    // path vertex to just beyond the final path vertex.
    auto iterA = std::begin(pathVertices);
    while (iterA != std::end(pathVertices)) {
        // Retain the vertex pointed to by iterator A.
        retainedVertices.push_back(*iterA);

        // If iterator A points to the final path vertex, advance it.
        // Otherwise, use line-of-sight checks to find the next vertex to retain.
        if (iterA == std::end(pathVertices) - 1) {
            ++iterA;
        }
        else {
            // Create an iterator B that advances from the first position after
            // iterator A to the first vertex for which the line of sight is broken.
            auto iterB = iterA + 1;
            Offset2D uniqueOffset = *iterB - *iterA;
            bool lineOfSight = true;
            while (lineOfSight && iterB != std::end(pathVertices)) {
                // Obtain the offset between the vertex of iterator B and the preceding vertex.
                const Offset2D offset = *iterB - *(iterB - 1);

                // Perform the line-of-sight check only if the offsets have not all been identical.
                if (offset != uniqueOffset) {
                    uniqueOffset = { 0, 0 };
                    lineOfSight = Grid().LineOfSight(*iterA, *iterB);
                }

                // If the line of sight is not yet broken, advance iterator B.
                if (lineOfSight) {
                    ++iterB;
                }
            }

            // Advance iterator A to the first position before the vertex of iterator B.
            iterA = iterB - 1;
        }
    }

    // Replace the original path vertices with the retained vertices.
    pathVertices = retainedVertices;
}

}  // namespace

#endif
