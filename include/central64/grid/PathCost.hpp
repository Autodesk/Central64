#pragma once
#ifndef CENTRAL64_PATH_COST
#define CENTRAL64_PATH_COST

#include <central64/grid/Types.hpp>

namespace central64 {

/// A data type for path costs supporting fixed-point arithmetic.
/// A path cost is stored internally as a multiplier based on a fixed level of precision.
/// This internal representation ensures that operations resulting in new path costs are exact.
/// Exact path costs are convenient for counting all shortest grid paths between two points.
class PathCost
{
public:    
    static constexpr PathCost MaxCost() { return PathCost(std::numeric_limits<double>::infinity()); }  ///< The maximum representable path cost.
    static constexpr PathCost MinCost() { return PathCost(1, 0); }                                     ///< The minimum representable positive path cost.

    constexpr PathCost() : multiplier_{ 0 } {}     ///< Create a path cost of zero.
    constexpr explicit PathCost(double distance);  ///< Create a path cost representing the specified `distance` in grid spacings.

    constexpr double Distance() const { return double(multiplier_)/double(precision); }  ///< Compute the distance represented by the path cost.

    constexpr const PathCost operator+() const { return *this; }                        ///< Return a copy of the path cost.
    constexpr const PathCost operator-() const { return PathCost{ -multiplier_, 0 }; }  ///< Return the negation of the path cost.

    constexpr const PathCost operator+(PathCost rhs) const { return PathCost{ multiplier_ + rhs.multiplier_, 0 }; }  ///< Return the sum of two path costs.
    constexpr const PathCost operator-(PathCost rhs) const { return PathCost{ multiplier_ - rhs.multiplier_, 0 }; }  ///< Return the difference between two path costs.
    constexpr const PathCost operator*(int rhs) const { return PathCost{ multiplier_*rhs, 0 }; }                     ///< Return the product of a path cost and an integer.

    constexpr const PathCost& operator+=(PathCost rhs) { *this = *this + rhs; return *this; }  ///< Add the path cost on the right-hand side.
    constexpr const PathCost& operator-=(PathCost rhs) { *this = *this - rhs; return *this; }  ///< Subtract the path cost on the right-hand side.
    constexpr const PathCost& operator*=(int rhs) { *this = *this * rhs; return *this; }       ///< Multiply by the integer on the right-hand side.

    constexpr double operator/(PathCost rhs) const { return double(multiplier_)/double(rhs.multiplier_); }  ///< Compute the ratio between two path costs.

    constexpr bool operator==(PathCost rhs) const { return multiplier_ == rhs.multiplier_; }  ///< Check whether two path costs are equal.
    constexpr bool operator!=(PathCost rhs) const { return multiplier_ != rhs.multiplier_; }  ///< Check whether two path costs are different.
    constexpr bool operator<(PathCost rhs) const { return multiplier_ < rhs.multiplier_; }    ///< Check whether two path costs are increasing.
    constexpr bool operator>(PathCost rhs) const { return multiplier_ > rhs.multiplier_; }    ///< Check whether two path costs are decreasing.
    constexpr bool operator<=(PathCost rhs) const { return multiplier_ <= rhs.multiplier_; }  ///< Check whether two path costs are ascending.
    constexpr bool operator>=(PathCost rhs) const { return multiplier_ >= rhs.multiplier_; }  ///< Check whether two path costs are descending.

private:
    static const int64_t precision = 2040;                 // The number of distinct path cost values per grid spacing.
    static const int64_t max = precision*1000000*1000000;  // The maximum internal path cost multiplier.

    // Create a path cost directly from a multiplier, rather than the represented distance.
    // The second argument is not used, and exists only to distinguish this internal
    // constructor from the public constructor.
    constexpr PathCost(int64_t multiplier, int);

    int64_t multiplier_;
};

constexpr PathCost::PathCost(double distance)
    : multiplier_{ (distance >= 0) ? (distance*precision >= max)  ? max : 
                                                                    int64_t(distance*precision + 0.5) :
                                     (distance*precision <= -max) ? -max :
                                                                    -int64_t(-distance*precision + 0.5) }
{
}

constexpr PathCost::PathCost(int64_t multiplier, int)
    : multiplier_{ (multiplier >= 0) ? (multiplier >= max)  ? max :
                                                              int64_t(multiplier + 0.5) :
                                       (multiplier <= -max) ? -max :
                                                              -int64_t(-multiplier + 0.5) }
{
}

/// Return the product of an integer and a path cost.
constexpr const PathCost operator*(int lhs, PathCost rhs)
{
    return rhs*lhs;
}

}  // namespace

#endif
