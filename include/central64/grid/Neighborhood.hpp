#pragma once
#ifndef CENTRAL64_NEIGHBORHOOD
#define CENTRAL64_NEIGHBORHOOD

#include <central64/grid/Offset2D.hpp>
#include <central64/grid/PathCost.hpp>

namespace central64 {

/// An abstract class representing a standard rectangular 2D grid neighborhood with `L` neighbors.
template <int L>
class Neighborhood
{
    static_assert(L == 4 || L == 8 || L == 16 || L == 32 || L == 64, "Neighborhood size must be 4, 8, 16, 32, or 64.");

public:
    class Move;

    static constexpr const std::array<Move, L>& Moves();  ///< Return a reference to the array of neighborhood moves, computed at compile time.

    static constexpr Move XMove() { return Moves()[0]; }                          ///< Get the +X cardinal move.
    static constexpr Move YMove() { return Moves()[Move::TicksPer90Degrees()]; }  ///< Get the +Y cardinal move.

    static constexpr int Radius();                            ///< Get the maximum offset, in either the X or Y direction, among all neighborhood moves.
    static constexpr PathCost StandardCost(Offset2D offset);  ///< Compute the cost of a shortest possible grid path with endpoints separated by `offset`.

private:
    Neighborhood() = delete;

    struct MoveArray;
    static constexpr std::array<Move, L> moves = MoveArray::data;
};

/// A specialization allowing Neighborhood<128>::Move to be a friend of Neighborhood<64>::Move.
template <> class Neighborhood<128> { class Move; };

/// A data type representing one of the `L` neighborhood moves.
template <int L>
class Neighborhood<L>::Move
{
    friend class Neighborhood<L>;
    friend class Neighborhood<2*L>::Move;

public:
    static constexpr int TicksPer180Degrees() { return L/2; }  ///< Get the number of ticks per 180 degrees.
    static constexpr int TicksPer90Degrees() { return L/4; }   ///< Get the number of ticks per 90 degrees.
    static constexpr int TicksPer45Degrees() { return L/8; }   ///< Get the number of ticks per 45 degrees.

    constexpr int Index() const { return index_; }         ///< Get the index of the move.
    constexpr Offset2D Offset() const { return offset_; }  ///< Get the 2D offset represented by the move.
    constexpr PathCost Cost() const { return cost_; }      ///< Get the path cost associated with the move.

    constexpr bool IsCardinal() const { return offset_.X()*offset_.Y() == 0; }                        ///< Check whether the move is in a 90-degree cardinal direction.
    constexpr bool IsDiagonal() const { return offset_.X()*offset_.X() == offset_.Y()*offset_.Y(); }  ///< Check whether the move is in a 45-degree diagonal direction.
    constexpr bool IsOdd() const { return bool(index_ & 1); }                                         ///< Check whether the move is in an odd direction, meaning that the index is odd.

    constexpr const Move operator+() const { return *this; }                         ///< Return a copy of the move.
    constexpr const Move operator-() const { return *this + TicksPer180Degrees(); }  ///< Return the move in the exact opposite direction.

    constexpr const Move operator+(int ticks) const { return Neighborhood<L>::Moves()[(index_ + ticks) & (L - 1)]; }  ///< Return the move rotated by the specified number of `ticks` in the positive direction (from +X toward +Y).
    constexpr const Move operator-(int ticks) const { return Neighborhood<L>::Moves()[(index_ - ticks) & (L - 1)]; }  ///< Return the move rotated by the specified number of `ticks` in the negative direction (from +X toward -Y).

    constexpr const Move& operator++() { *this = *this + 1; return *this; }  ///< Rotate the move by one tick in the positive direction (from +X toward +Y).
    constexpr const Move& operator--() { *this = *this - 1; return *this; }  ///< Rotate the move by one tick in the negative direction (from +X toward -Y).

    constexpr const Move operator++(int) { auto old = *this; ++(*this); return *old; }  ///< Rotate the move by one tick in the positive direction (from +X toward +Y), and return the old value.
    constexpr const Move operator--(int) { auto old = *this; --(*this); return *old; }  ///< Rotate the move by one tick in the negative direction (from +X toward -Y), and return the old value.
 
    constexpr const Move& operator+=(int ticks) { *this = *this + ticks; return *this; }  ///< Rotate the move by the specified number of `ticks` in the positive direction (from +X toward +Y).
    constexpr const Move& operator-=(int ticks) { *this = *this - ticks; return *this; }  ///< Rotate the move by the specified number of `ticks` in the negative direction (from +X toward -Y).

    constexpr bool operator==(const Move& rhs) const { return index_ == rhs.index_; }  ///< Check that the two moves are the same.
    constexpr bool operator!=(const Move& rhs) const { return index_ != rhs.index_; }  ///< Check that the two moves are different.

private:
    // Create a move with the specified `index`.
    constexpr Move(int index);

    int index_;
    Offset2D offset_;
    PathCost cost_;
};

template <int L>
constexpr Neighborhood<L>::Move::Move(int index)
    : index_{ index & (L - 1) }
    , offset_{}
    , cost_{}
{
    if constexpr (L == 4) {
        // For the 4-neighborhood, select the approriate cardinal 2D offset and set the cost to 1 grid spacing.
        offset_ = (index_ == 0) ? Offset2D{ 1, 0 } :
                  (index_ == 1) ? Offset2D{ 0, 1 } :
                  (index_ == 2) ? Offset2D{ -1, 0 } :
                                  Offset2D{ 0, -1 };
        cost_ = PathCost{ 1.0 };
    }
    else {
        // Compute the 2D offset by finding the corresponding move from the previous neighborhood (even moves),
        // or by adding the 2D offsets of the two surrounding moves from the previous neighborhood (odd moves).
        offset_ = (index%2 == 0) ? typename Neighborhood<L/2>::Move(index_/2).Offset() :
                                   typename Neighborhood<L/2>::Move(index_/2).Offset() +
                                   typename Neighborhood<L/2>::Move(index_/2 + 1).Offset();
        // Select the Euclidean cost at compile time from a list of precomputed square root values.
        const int distanceSquared = offset_.X()*offset_.X() + offset_.Y()*offset_.Y();
        cost_ = PathCost{ (distanceSquared ==  1) ? 1.0 :
                          (distanceSquared ==  2) ? 1.4142135623730951 :
                          (distanceSquared ==  5) ? 2.23606797749979 :
                          (distanceSquared == 10) ? 3.1622776601683795 :
                          (distanceSquared == 13) ? 3.605551275463989 :
                          (distanceSquared == 17) ? 4.123105625617661 :
                          (distanceSquared == 25) ? 5.0 :
                          (distanceSquared == 29) ? 5.385164807134504 :
                                            /*34*/  5.830951894845301 };
    }
}

template <int L>
constexpr const std::array<typename Neighborhood<L>::Move, L>& Neighborhood<L>::Moves()
{
    return moves;
}

template <int L>
constexpr int Neighborhood<L>::Radius()
{
    // Return the neighborhood radius, a Fibonacci number.
    if constexpr (L == 4) return 1;
    if constexpr (L == 8) return 1;
    if constexpr (L == 16) return 2;
    if constexpr (L == 32) return 3;
    if constexpr (L == 64) return 5;
}

template <int L>
constexpr PathCost Neighborhood<L>::StandardCost(Offset2D offset)
{
    // Obtain the absolute values of the 2D offset coordinates.
    const int x = (offset.X() >= 0) ? offset.X() : -offset.X();
    const int y = (offset.Y() >= 0) ? offset.Y() : -offset.Y();
    // Sort the absolute values.
    const int a = std::min(x, y);
    const int b = std::max(x, y);
    // Derive the standard cost as a linear combination of at most two neighborhood move costs.
    const auto& moves = Neighborhood<L>::Moves();
    if constexpr (L ==  4) return               moves[0].Cost()*(a + b);
    if constexpr (L ==  8) return               moves[0].Cost()*(b - a)     + moves[1].Cost()*(a);
    if constexpr (L == 16) return (2*a < b)   ? moves[0].Cost()*(b - 2*a)   + moves[1].Cost()*(a) :
                                                moves[1].Cost()*(b - a)     + moves[2].Cost()*(2*a - b);
    if constexpr (L == 32) return (3*a < b)   ? moves[0].Cost()*(b - 3*a)   + moves[1].Cost()*(a) :
                                  (2*a < b)   ? moves[1].Cost()*(b - 2*a)   + moves[2].Cost()*(3*a - b) :
                                  (3*a < 2*b) ? moves[2].Cost()*(2*b - 3*a) + moves[3].Cost()*(2*a - b) :
                                                moves[3].Cost()*(b - a)     + moves[4].Cost()*(3*a - 2*b);
    if constexpr (L == 64) return (4*a < b)   ? moves[0].Cost()*(b - 4*a)   + moves[1].Cost()*(a) :
                                  (3*a < b)   ? moves[1].Cost()*(b - 3*a)   + moves[2].Cost()*(4*a - b) :
                                  (5*a < 2*b) ? moves[2].Cost()*(2*b - 5*a) + moves[3].Cost()*(3*a - b) :
                                  (2*a < b)   ? moves[3].Cost()*(b - 2*a)   + moves[4].Cost()*(5*a - 2*b) :
                                  (5*a < 3*b) ? moves[4].Cost()*(3*b - 5*a) + moves[5].Cost()*(2*a - b) :
                                  (3*a < 2*b) ? moves[5].Cost()*(2*b - 3*a) + moves[6].Cost()*(5*a - 3*b) :
                                  (4*a < 3*b) ? moves[6].Cost()*(3*b - 4*a) + moves[7].Cost()*(3*a - 2*b) :
                                                moves[7].Cost()*(b - a)     + moves[8].Cost()*(4*a - 3*b);
}

/// A traits class for initializing the set of moves for each neighborhood.
template <int L> struct Neighborhood<L>::MoveArray {};
template <> struct Neighborhood< 4>::MoveArray { static constexpr std::array<Neighborhood< 4>::Move,  4> data = { 0, 1, 2, 3 }; };
template <> struct Neighborhood< 8>::MoveArray { static constexpr std::array<Neighborhood< 8>::Move,  8> data = { 0, 1, 2, 3, 4, 5, 6, 7 }; };
template <> struct Neighborhood<16>::MoveArray { static constexpr std::array<Neighborhood<16>::Move, 16> data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }; };
template <> struct Neighborhood<32>::MoveArray { static constexpr std::array<Neighborhood<32>::Move, 32> data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 }; };
template <> struct Neighborhood<64>::MoveArray { static constexpr std::array<Neighborhood<64>::Move, 64> data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63 }; };

/// A type alias allowing `Neighborhood<L>::Move` to be abbreviated `Move<L>`.
template <int L>
using Move = typename Neighborhood<L>::Move;

}  // namespace

#endif
