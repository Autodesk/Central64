#pragma once
#ifndef CENTRAL64_CONNECTIONS
#define CENTRAL64_CONNECTIONS

#include <central64/grid/Neighborhood.hpp>

namespace central64 {

/// A data type indicating whether each of the `L` neighborhood moves is connected or disconnected.
template <int L>
class Connections
{
public:
    constexpr Connections() = default;  ///< Create a set of connections for which all moves are connected.

    constexpr bool IsConnected(const Move<L>& move) const { return bool(flags_ & (Unsigned<L>::unit << move.Index())); }  ///< Check whether the specified `move` is connected.
    constexpr bool IsAllConnected() const { return bool(flags_ == Unsigned<L>::max); }                                    ///< Check whether all moves are connected.
    constexpr bool IsAllDisconnected() const { return bool(flags_ == Unsigned<L>::zero); }                                ///< Check whether all moves are disconnected.

    constexpr void ConnectAll() { flags_ = Unsigned<L>::max; }                                                              ///< Connect all moves.
    constexpr void DisconnectAll() { flags_ = Unsigned<L>::zero; }                                                          ///< Disconnect all moves.
    constexpr void Connect(const Move<L>& move) { flags_ |= (Unsigned<L>::unit << move.Index()); }                          ///< Connect the specified `move`.
    constexpr void Disconnect(const Move<L>& move) { flags_ &= (Unsigned<L>::max ^ (Unsigned<L>::unit << move.Index())); }  ///< Disconnect the specified `move`.

    constexpr const Connections operator&(Connections rhs) const { return Connections{ typename Unsigned<L>::type(flags_ & rhs.flags_) }; }  ///< Return the intersection of two sets of connections.
    constexpr const Connections operator|(Connections rhs) const { return Connections{ typename Unsigned<L>::type(flags_ | rhs.flags_) }; }  ///< Return the union of two sets of connections.

    constexpr const Connections& operator&=(Offset2D rhs) { *this = *this & rhs; return *this; }  ///< Intersect with the sets of connections on the right-hand side.
    constexpr const Connections& operator|=(Offset2D rhs) { *this = *this | rhs; return *this; }  ///< Union with the sets of connections on the right-hand side.

    constexpr bool operator==(Connections rhs) const { return flags_ == rhs.flags_; }  ///< Check whether two sets of connections are equal.
    constexpr bool operator!=(Connections rhs) const { return flags_ != rhs.flags_; }  ///< Check whether two sets of connections are different.

private:
    // Create a set of connections directly from an unsigned integer representing a set of flags.
    Connections(typename Unsigned<L>::type flags) : flags_{ flags } {}

    typename Unsigned<L>::type flags_ = Unsigned<L>::zero;
};

}  // namespace

#endif
