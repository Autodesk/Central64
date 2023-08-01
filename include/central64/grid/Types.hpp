#pragma once
#ifndef CENTRAL64_TYPES
#define CENTRAL64_TYPES

#include <stdint.h>
#include <assert.h>
#include <limits>
#include <cmath>
#include <array>
#include <vector>
#include <string>
#include <charconv>
#include <memory>

namespace central64 {

/// An unsigned integer traits class for compact representation of grid connections. Used by the `Connections` class.
template <int L> struct Unsigned {};
template <> struct Unsigned< 4> { using type = uint8_t;  static constexpr type zero = 0; static constexpr type unit = 1; static constexpr type max =               0x0F; };  ///< The specialization of `Unsigned` for 4-bit unsigned integers.
template <> struct Unsigned< 8> { using type = uint8_t;  static constexpr type zero = 0; static constexpr type unit = 1; static constexpr type max =               0xFF; };  ///< The specialization of `Unsigned` for 8-bit unsigned integers.
template <> struct Unsigned<16> { using type = uint16_t; static constexpr type zero = 0; static constexpr type unit = 1; static constexpr type max =             0xFFFF; };  ///< The specialization of `Unsigned` for 16-bit unsigned integers.
template <> struct Unsigned<32> { using type = uint32_t; static constexpr type zero = 0; static constexpr type unit = 1; static constexpr type max =         0xFFFFFFFF; };  ///< The specialization of `Unsigned` for 32-bit unsigned integers.
template <> struct Unsigned<64> { using type = uint64_t; static constexpr type zero = 0; static constexpr type unit = 1; static constexpr type max = 0xFFFFFFFFFFFFFFFF; };  ///< The specialization of `Unsigned` for 64-bit unsigned integers.

} // namespace

#endif
