#pragma once
#ifndef CENTRAL64_OFFSET2D
#define CENTRAL64_OFFSET2D

#include <central64/grid/Types.hpp>

namespace central64 {

/// A data type for 2D offsets with integer X and Y coordinates.
class Offset2D
{
public:
    constexpr Offset2D() : x_{ 0 }, y_{ 0 } {}              ///< Create a 2D offset with value [0, 0].
    constexpr Offset2D(int x, int y) : x_{ x }, y_{ y } {}  ///< Create a 2D offset with value [`x`, `y`].

    constexpr int X() const { return x_; }  ///< Get the X coordinate.
    constexpr int Y() const { return y_; }  ///< Get the Y coordinate.

    constexpr const Offset2D operator+() const { return { +x_, +y_ }; }                              ///< Return the 2D offset with the X and Y coordinates unchanged.
    constexpr const Offset2D operator-() const { return { -x_, -y_ }; }                              ///< Return the 2D offset with the X and Y coordinates negated.
    constexpr const Offset2D operator+(Offset2D rhs) const { return { x_ + rhs.x_, y_ + rhs.y_ }; }  ///< Return the vector sum of two 2D offsets.
    constexpr const Offset2D operator-(Offset2D rhs) const { return { x_ - rhs.x_, y_ - rhs.y_ }; }  ///< Return the vector difference between two 2D offsets.
    constexpr const Offset2D operator*(int rhs) const { return { x_*rhs, y_*rhs }; }                 ///< Return the pointwise product of a 2D offset and an integer.
    constexpr const Offset2D operator/(int rhs) const { return { x_/rhs, y_/rhs }; }                 ///< Return the pointwise quotient of a 2D offset and an integer.

    constexpr const Offset2D operator+=(Offset2D rhs) { *this = *this + rhs; return *this; }  ///< Add the 2D offset on the right-hand side.
    constexpr const Offset2D operator-=(Offset2D rhs) { *this = *this - rhs; return *this; }  ///< Subtract the 2D offset on the right-hand side.
    constexpr const Offset2D operator*=(int rhs) { *this = (*this)*rhs; return *this; }       ///< Multiply by the 2D offset on the right-hand side.
    constexpr const Offset2D operator/=(int rhs) { *this = (*this)/rhs; return *this; }       ///< Divide by the 2D offset on the right-hand side.

    constexpr bool operator==(Offset2D rhs) const { return (x_ == rhs.x_) && (y_ == rhs.y_); }                             ///< Check whether two 2D offsets are equal.
    constexpr bool operator!=(Offset2D rhs) const { return (x_ != rhs.x_) || (y_ != rhs.y_); }                             ///< Check whether two 2D offsets are different.
    constexpr bool operator<(Offset2D rhs) const { return (y_ < rhs.y_) ? true : (y_ > rhs.y_) ? false : (x_ < rhs.x_); }  ///< Check whether two 2D offsets are increasing, prioritizing the Y coordinate.
    constexpr bool operator>(Offset2D rhs) const { return (y_ > rhs.y_) ? true : (y_ < rhs.y_) ? false : (x_ > rhs.x_); }  ///< Check whether two 2D offsets are decreasing, prioritizing the Y coordinate.
    constexpr bool operator<=(Offset2D rhs) const { return !(*this > rhs); }                                               ///< Check whether two 2D offsets are ascending, prioritizing the Y coordinate.
    constexpr bool operator>=(Offset2D rhs) const { return !(*this < rhs); }                                               ///< Check whether two 2D offsets are descending, prioritizing the Y coordinate.

private:
    int x_;
    int y_;
};

/// Return the product of an integer and a 2D offset.
constexpr const Offset2D operator*(int lhs, Offset2D rhs)
{
    return rhs*lhs;
}

/// Convert the `offset` to a string.
/// The left bracket (`leftBracket`), right bracket (`rightBracket`),
/// and coordinate delimiter (`coordDelimiter`) are customizable.
inline std::string ToString(Offset2D offset,
                            std::string leftBracket = "[",
                            std::string rightBracket = "]",
                            std::string coordDelimiter = ", ")
{
    return leftBracket + 
           std::to_string(offset.X()) + 
           coordDelimiter + 
           std::to_string(offset.Y()) + 
           rightBracket;
}

/// Convert a path represented as a vector of 2D offsets (`pathVertices`) to a string. 
/// The left bracket (`leftBracket`), right bracket (`rightBracket`), coordinate delimiter
/// (`coordDelimiter`), and vertex delimiter (`vertexDelimiter`) are customizable.
inline std::string ToString(const std::vector<Offset2D>& pathVertices,
                            std::string leftBracket = "[",
                            std::string rightBracket = "]",
                            std::string coordDelimiter = ", ",
                            std::string vertexDelimiter = ", ")
{
    std::string pathString{};
    int vertexCount = int(pathVertices.size());
    for (int i = 0; i < vertexCount; ++i) {
        if (i > 0) {
            pathString += vertexDelimiter;
        }
        pathString += ToString(pathVertices[i]);
    }
    return pathString;
}

/// Compute the length of a path represented as a vector of 2D offsets (`pathVertices`). 
inline double PathLength(const std::vector<Offset2D>& pathVertices)
{
    double pathLength = 0.0;
    if (pathVertices.empty()) {
        pathLength = std::numeric_limits<double>::infinity();
    }
    else {
        int vertexCount = int(pathVertices.size());
        for (int i = 0; i < vertexCount - 1; ++i) {
            const Offset2D offset = pathVertices[i + 1] - pathVertices[i];
            pathLength += sqrt(offset.X()*offset.X() + offset.Y()*offset.Y());
        }
    }
    return pathLength;
}

}  // namespace

#endif
