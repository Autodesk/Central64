#pragma once
#ifndef CENTRAL64_NO_SMOOTHING
#define CENTRAL64_NO_SMOOTHING

#include <central64/smoothing/AbstractSmoothing.hpp>

namespace central64 {

/// A derived class for the smoothing method that makes no change.
template <int L>
class NoSmoothing : public AbstractSmoothing<L>
{
public:
    explicit NoSmoothing(const Grid2D<L>& grid) : AbstractSmoothing{ grid } {}  ///< Create a no smoothing object that references an existing grid object.

    NoSmoothing(NoSmoothing&&) = default;             ///< Ensure the default move constructor is public.
    NoSmoothing& operator=(NoSmoothing&&) = default;  ///< Ensure the default move assignment operator is public.

    virtual void SmoothPath(std::vector<Offset2D>& pathVertices) {}  ///< Perform the smoothing operation on a path represented as a vector of 2D offsets (`pathVertices`).

    virtual std::string MethodName() const { return "No Smoothing"; }  ///< Get the name of the smoothing method.
};

}  // namespace

#endif
