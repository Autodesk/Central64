#pragma once
#ifndef CENTRAL64_ABSTRACT_SMOOTHING
#define CENTRAL64_ABSTRACT_SMOOTHING

#include <central64/grid/Grid2D.hpp>

namespace central64 {

/// An abstract base class for `L`-neighbor grid path smoothing methods.
template <int L>
class AbstractSmoothing
{
public:
    virtual ~AbstractSmoothing() = default;  ///< Ensure the destructor is virtual, since the class is to be inherited.

    const Grid2D<L>& Grid() const { return grid_; }  ///< Obtain a const reference to the grid.

    virtual void SmoothPath(std::vector<Offset2D>& pathVertices) = 0;  ///< Perform the smoothing operation on a path represented as a vector of 2D offsets (`pathVertices`).

    virtual std::string MethodName() const = 0;  ///< Get the name of the smoothing method.

protected:
    explicit AbstractSmoothing(const Grid2D<L>& grid) : grid_{ grid } {}  ///< Create an abstract smoothing object that references an existing grid object.

    AbstractSmoothing(AbstractSmoothing&&) = default;             ///< Ensure the default move constructor is protected because the class is to be inherited.
    AbstractSmoothing& operator=(AbstractSmoothing&&) = default;  ///< Ensure the default move assignment operator is protected because the class is to be inherited.

private:
    AbstractSmoothing(const AbstractSmoothing&) = delete;
    AbstractSmoothing& operator=(const AbstractSmoothing&) = delete;

    Grid2D<L> grid_;
};

}  // namespace

#endif
