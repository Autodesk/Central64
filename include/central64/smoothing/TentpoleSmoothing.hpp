#pragma once
#ifndef CENTRAL64_TENTPOLE_SMOOTHING
#define CENTRAL64_TENTPOLE_SMOOTHING

#include <central64/smoothing/AbstractSmoothing.hpp>

namespace central64 {

/// A derived class for the tentpole smoothing method.
template <int L>
class TentpoleSmoothing : public AbstractSmoothing<L>
{
public:
    explicit TentpoleSmoothing(const Grid2D<L>& grid) : AbstractSmoothing<L>{ grid } {}  ///< Create a tentpole smoothing object that references an existing grid object.

    TentpoleSmoothing(TentpoleSmoothing&&) = default;             ///< Ensure the default move constructor is public.
    TentpoleSmoothing& operator=(TentpoleSmoothing&&) = default;  ///< Ensure the default move assignment operator is public.

    virtual void SmoothPath(std::vector<Offset2D>& pathVertices);  ///< Perform the smoothing operation on a path represented as a vector of 2D offsets (`pathVertices`).

    virtual std::string MethodName() const { return "Tentpole Smoothing"; }  ///< Get the name of the smoothing method.

    const Grid2D<L>& Grid() const { return AbstractSmoothing<L>::Grid(); }
};

template <int L>
void TentpoleSmoothing<L>::SmoothPath(std::vector<Offset2D>& pathVertices)
{
    // Create a vector of the path vertices to be retained.
    std::vector<Offset2D> retainedVertices{};

    // Smooth the path only if it is not empty.
    if (!pathVertices.empty()) {
        // Create an iterator A that advances from the first path vertex to the
        // final path vertex, and an iterator C that remains ahead of iterator A.
        auto iterA = std::begin(pathVertices);
        auto iterC = iterA + 1;
        while (iterA != std::end(pathVertices)) {
            // Retain the vertex pointed to by iterator A.
            retainedVertices.push_back(*iterA);

            // If iterator A points to the final path vertex, advance it.
            // Otherwise, use line-of-sight checks to find the next vertex to retain.
            if (iterA == std::end(pathVertices) - 1) {
                ++iterA;
            }
            else {
                // Determine if these is a single offset for all moves between iterators A and C.
                Offset2D uniqueOffset = *(iterA + 1) - *iterA;
                for (auto iterB = iterA + 1; iterB < iterC; ++iterB) {
                    const Offset2D offset = *iterB - *(iterB - 1);
                    if (offset != uniqueOffset) {
                        uniqueOffset = { 0, 0 };
                    }
                }

                // Advance iterator C to the first vertex for which the line of sight is broken.
                bool lineOfSight = true;
                while (lineOfSight && iterC != std::end(pathVertices)) {
                    // Obtain the offset between the vertex of iterator C and the preceding vertex.
                    Offset2D offset = *iterC - *(iterC - 1);

                    // Perform the line-of-sight check only if the offsets have not all been identical.
                    if (offset != uniqueOffset) {
                        uniqueOffset = { 0, 0 };
                        lineOfSight = Grid().LineOfSight(*iterA, *iterC);
                    }

                    // If the line of sight is not yet broken, advance iterator C.
                    if (lineOfSight) {
                        ++iterC;
                    }
                }

                // If iterator C has advanced beyond the final path vertex,
                // advance iterator A to the final path vertex. Otherwise,
                // create an iterator B that backtracks from iterator C
                // in search of the optimal tentpole position.
                if (iterC == std::end(pathVertices)) {
                    iterA = iterC - 1;
                }
                else {
                    // Create an iterator B that backtracks from the first position before
                    // iterator C to the first vertex for which the line of sight between
                    // B and C is broken. Keep track of the tentpole location (`nextIterA`)
                    // that yields the shortest two-segment path between iterators A and C.
                    auto iterB = iterC - 1;
                    uniqueOffset = *iterB - *iterC;
                    lineOfSight = true;
                    double shortestDistance = std::numeric_limits<double>::infinity();
                    auto nextIterA = std::end(pathVertices);
                    while (lineOfSight && iterB != iterA) {
                        // Obtain the offset between the vertex of iterator B and the successing vertex.
                        Offset2D offset = *iterB - *(iterB + 1);

                        // Perform the line-of-sight check only if the offsets have not all been identical.
                        if (offset != uniqueOffset) {
                            uniqueOffset = { 0, 0 };
                            lineOfSight = Grid().LineOfSight(*iterB, *iterC);
                        }

                        // If the line of sight is not yet broken, the (1) check whether the new tentpole
                        // location produces the shortest path so far, and (2) move iterator B backward.
                        if (lineOfSight) {
                            Offset2D offsetAB = *iterB - *iterA;
                            Offset2D offsetBC = *iterC - *iterB;
                            double distanceAB = sqrt(offsetAB.X()*offsetAB.X() + offsetAB.Y()*offsetAB.Y());
                            double distanceBC = sqrt(offsetBC.X()*offsetBC.X() + offsetBC.Y()*offsetBC.Y());
                            double distance = distanceAB + distanceBC;
                            if (distance < shortestDistance) {
                                shortestDistance = distance;
                                nextIterA = iterB;
                            }
                            --iterB;
                        }
                    }

                    // Advance iterator A to the optimal tentpole location.
                    iterA = nextIterA;
                }
            }
        }
    }

    // Replace the original path vertices with the retained vertices.
    pathVertices = retainedVertices;
}

}  // namespace

#endif
