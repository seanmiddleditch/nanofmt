// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "nanofmt/config.h"

#include <type_traits>

namespace NANOFMT_NS::detail {
    template <typename UnsignedIntT>
    constexpr int count_digits(UnsignedIntT value) noexcept;

    template <typename UnsignedIntT>
    constexpr UnsignedIntT rshift10(UnsignedIntT value, int shift) noexcept;

    template <typename UnsignedIntT>
    constexpr int countl_zero(UnsignedIntT value) noexcept;

    template <typename IntT>
    constexpr auto abs(IntT value) noexcept;

    template <typename T>
    constexpr auto(min)(T lhs, T rhs) noexcept -> T {
        return lhs < rhs ? lhs : rhs;
    }

    template <typename T>
    constexpr auto(max)(T lhs, T rhs) noexcept -> T {
        return lhs < rhs ? rhs : lhs;
    }

    template <typename UnsignedIntT>
    constexpr int count_digits(UnsignedIntT value) noexcept {
        static_assert(std::is_unsigned_v<UnsignedIntT>);

        int result = 1;
        for (;;) {
            if (value < 10) {
                return result;
            }
            if (value < 100) {
                return result + 1;
            }
            if constexpr (sizeof(UnsignedIntT) > 1) {
                if (value < 1'000) {
                    return result + 2;
                }
                if (value < 10'000) {
                    return result + 3;
                }
                value /= 10'000u;
                result += 4;
            }
            else {
                return result + 2;
            }
        }
    }

    // chop off any digits that cannot fit in the destination
    //
    template <typename UnsignedIntT>
    constexpr UnsignedIntT rshift10(UnsignedIntT value, int shift) noexcept {
        if constexpr (sizeof(UnsignedIntT) > 1) {
            while (shift >= 4) {
                value /= 10'000u;
                shift -= 4;
            }
            if (shift == 3) {
                return value / 1'000u;
            }
        }
        if (shift == 2) {
            return value / 100u;
        }
        if (shift == 1) {
            return value / 10u;
        }
        return value;
    }

#if defined(__has_builtin)
#    if __has_builtin(__builtin_clz)
#        define NANOFMT_HAS_BUILTIN_CLZ 1
#    endif
#endif
#if defined(__clang__) || defined(__GNUC__)
#    define NANOFMT_CLANG_OR_GCC 1
#endif
#if defined(_WIN32)
#    define NANOFMT_HAS_BSR 1
#endif

    template <typename UnsignedIntT>
    constexpr int countl_zero(UnsignedIntT value) noexcept {
#if NANOFMT_HAS_BUILTIN_CLZ || NANOFMT_CLANG_OR_GCC
        if (value == 0) {
            return sizeof(value) * 8;
        }
        if constexpr (sizeof value <= 4) {
            return __builtin_clz(value);
        }
        else {
            return __builtin_clzll(value);
        }
#elif NANOFMT_HAS_BSR
        // __builtin_clz / __builtin_clzll
        if constexpr (sizeof value <= 4) {
            unsigned long index = 0;
            return _BitScanReverse(&index, value) ? 31 - index : 32;
        }
        else {
            unsigned long index = 0;
            return _BitScanReverse64(&index, value) ? 63 - index : 64;
        }
#else
#    error "nanofmt::detail::countl_zero not implemented for this compiler/platform"
        return -1;
#endif
    }

    template <typename IntT>
    constexpr auto abs(IntT value) noexcept {
        using Unsigned = std::make_unsigned_t<IntT>;

        if constexpr (std::is_signed_v<IntT>) {
            if (value < 0) {
                return static_cast<Unsigned>(0 - static_cast<Unsigned>(value));
            }
        }
        return static_cast<Unsigned>(value);
    }
} // namespace NANOFMT_NS::detail
