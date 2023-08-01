#pragma once
#ifndef CENTRAL64_PATH_COUNT
#define CENTRAL64_PATH_COUNT

#include <central64/grid/Types.hpp>

namespace central64 {

/// A data type for path counts supporting logarithm-based arithmetic.
/// Path counts are stored internally as logarithms to allow very large path counts
/// to be represented. These logarithms are hidden from the public interface.
/// The use of logarithms for internal computations means that path count addition
/// and multiplication operations yield approximate results.
class PathCount
{
public:
    static constexpr PathCount UnitCount() { return PathCount(0); }  ///< The minimum representable path count.

    constexpr PathCount() : logCount_{ -std::numeric_limits<double>::infinity() } {}  ///< Create a path count of zero.

    constexpr const PathCount operator+(PathCount rhs) const;                                                    ///< Return the sum of two path counts.
    constexpr const PathCount operator*(PathCount rhs) const { return PathCount{ logCount_ + rhs.logCount_ }; }  ///< Return the product of two path counts.

    constexpr const PathCount& operator+=(PathCount rhs) { *this = *this + rhs; return *this; }  ///< Add the path count on the right-hand side.
    constexpr const PathCount& operator*=(PathCount rhs) { *this = *this * rhs; return *this; }  ///< Multiply by the path count on the right-hand side.

    const double operator/(PathCount rhs) const { return pow(2.0, logCount_ - rhs.logCount_); }  ///< Compute the ratio between two path counts.

    constexpr bool operator==(PathCount rhs) const { return logCount_ == rhs.logCount_; }  ///< Check whether two path counts are equal.
    constexpr bool operator!=(PathCount rhs) const { return logCount_ != rhs.logCount_; }  ///< Check whether two path counts are different.
    constexpr bool operator<(PathCount rhs) const { return logCount_ < rhs.logCount_; }    ///< Check whether two path counts are increasing.
    constexpr bool operator>(PathCount rhs) const { return logCount_ > rhs.logCount_; }    ///< Check whether two path counts are decreasing.
    constexpr bool operator<=(PathCount rhs) const { return logCount_ <= rhs.logCount_; }  ///< Check whether two path counts are ascending.
    constexpr bool operator>=(PathCount rhs) const { return logCount_ >= rhs.logCount_; }  ///< Check whether two path counts are descending.

private:
    // Create a path count directly from the logarithm of the represented count.
    constexpr explicit PathCount(double logCount) : logCount_{ logCount } {}

    double logCount_;
};

constexpr const PathCount PathCount::operator+(PathCount rhs) const
{
    // Given log(A) and log(B), compute log(A + B) while avoiding overflow.
    const double logA = std::max(logCount_, rhs.logCount_);
    const double logB = std::min(logCount_, rhs.logCount_);
    return PathCount{ logA + log2(1.0 + pow(2.0, logB - logA)) };
}

}  // namespace

#endif
