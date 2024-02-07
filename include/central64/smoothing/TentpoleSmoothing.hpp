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
        // Create an index A that advances from the first path vertex to the
        // final path vertex, and an index C that remains ahead of A.
        int indexA = 0;
        int indexC = indexA + 1;
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
                // Determine if there is a single offset for all moves between A and C.
                Offset2D uniqueOffset = pathVertices[indexA + 1] - pathVertices[indexA];
                for (int indexB = indexA + 1; indexB < indexC; ++indexB) {
                    const Offset2D offset = pathVertices[indexB] - pathVertices[indexB - 1];
                    if (offset != uniqueOffset) {
                        uniqueOffset = { 0, 0 };
                    }
                }

                // Advance C to the first vertex for which the line of sight is broken.
                bool lineOfSight = true;
                while (lineOfSight && indexC < vertexCount) {
                    // Obtain the offset between the vertex of C and the preceding vertex.
                    Offset2D offset = pathVertices[indexC] - pathVertices[indexC - 1];

                    // Perform the line-of-sight check only if the offsets have not all been identical.
                    if (offset != uniqueOffset) {
                        uniqueOffset = { 0, 0 };
                        lineOfSight = Grid().LineOfSight(pathVertices[indexA], pathVertices[indexC]);
                    }

                    // If the line of sight is not yet broken, advance C.
                    if (lineOfSight) {
                        ++indexC;
                    }
                }

                // If C has advanced beyond the final path vertex,
                // advance A to the final path vertex. Otherwise,
                // create an index B that backtracks from C
                // in search of the optimal tentpole position.
                if (indexC == vertexCount) {
                    indexA = indexC - 1;
                }
                else {
                    // Create an index B that backtracks from the first position before
                    // C to the first vertex for which the line of sight between
                    // B and C is broken. Keep track of the tentpole location (`nextIndexA`)
                    // that yields the shortest two-segment path between A and C.
                    int indexB = indexC - 1;
                    uniqueOffset = pathVertices[indexB] - pathVertices[indexC];
                    lineOfSight = true;
                    double shortestDistance = std::numeric_limits<double>::infinity();
                    int nextIndexA = vertexCount;
                    while (lineOfSight && indexB != indexA) {
                        // Obtain the offset between the vertex of B and the successing vertex.
                        Offset2D offset = pathVertices[indexB] - pathVertices[indexB + 1];

                        // Perform the line-of-sight check only if the offsets have not all been identical.
                        if (offset != uniqueOffset) {
                            uniqueOffset = { 0, 0 };
                            lineOfSight = Grid().LineOfSight(pathVertices[indexB], pathVertices[indexC]);
                        }

                        // If the line of sight is not yet broken, the (1) check whether the new tentpole
                        // location produces the shortest path so far, and (2) move B backward.
                        if (lineOfSight) {
                            Offset2D offsetAB = pathVertices[indexB] - pathVertices[indexA];
                            Offset2D offsetBC = pathVertices[indexC] - pathVertices[indexB];
                            double distanceAB = sqrt(offsetAB.X()*offsetAB.X() + offsetAB.Y()*offsetAB.Y());
                            double distanceBC = sqrt(offsetBC.X()*offsetBC.X() + offsetBC.Y()*offsetBC.Y());
                            double distance = distanceAB + distanceBC;
                            if (distance < shortestDistance) {
                                shortestDistance = distance;
                                nextIndexA = indexB;
                            }
                            --indexB;
                        }
                    }

                    // Advance A to the optimal tentpole location.
                    indexA = nextIndexA;
                }
            }
        }
    }

    // Replace the original path vertices with the retained vertices.
    pathVertices = retainedVertices;
}

}  // namespace

#endif
