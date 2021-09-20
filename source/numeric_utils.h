// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "nanofmt/config.h"

#include <type_traits>

namespace NANOFMT_NS {
    namespace detail {
        template <bool Signed, std::size_t Size>
        struct long_compat_impl {
            using type = unsigned int;
        };

        template <std::size_t Size>
        struct long_compat_impl<true, Size> {
            using type = signed int;
        };

        template <>
        struct long_compat_impl<false, 8> {
            using type = unsigned long long;
        };

        template <>
        struct long_compat_impl<true, 8> {
            using type = signed long long;
        };

        template <typename IntegerT>
        using long_compat_t = typename long_compat_impl<std::is_signed_v<IntegerT>, sizeof(IntegerT)>::type;

        template <typename UnsignedIntT>
        constexpr int count_digits(UnsignedIntT value) noexcept;

        template <typename UnsignedIntT>
        constexpr UnsignedIntT rshift10(UnsignedIntT value, int shift) noexcept;

        constexpr int countl_zero(unsigned int value) noexcept;

        constexpr int countl_zero(unsigned long long value) noexcept;

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
            static_assert(std::is_unsigned_v<UnsignedIntT>, "Requires unsigned type");

            int result = 1;
            for (;;) {
                if (value < 10) {
                    return result;
                }
                if (value < 100) {
                    return result + 1;
                }
#if NANOFMT_CONSTEXPR_IF
                if constexpr (sizeof(UnsignedIntT) >= 2) {
                    if (value < 1'000) {
                        return result + 2;
                    }
                    if (value < 10'000) {
                        return result + 3;
                    }
                    value /= 10'000u;
                    result += 4;
                }
                else
#else
                {
                    value /= 100u;
                    result += 2;
                }
#endif
            }
        }

        // chop off any digits that cannot fit in the destination
        //
        template <typename UnsignedIntT>
        constexpr UnsignedIntT rshift10(UnsignedIntT value, int shift) noexcept {
#if NANOFMT_CONSTEXPR_IF
            if constexpr (sizeof(UnsignedIntT) >= 2) {
                while (shift >= 4) {
                    value /= 10'000u;
                    shift -= 4;
                }
                if (shift == 3) {
                    return value / 1'000u;
                }
            }
#endif
            if (shift == 2) {
                return value / 100u;
            }
            if (shift == 1) {
                return value / 10u;
            }
            return value;
        }

        constexpr int countl_zero(unsigned int value) noexcept {
#if NANOFMT_HAS_BUILTIN_CLZ || NANOFMT_CLANG_OR_GCC
            if (value == 0) {
                return sizeof(value) * 8;
            }
            return __builtin_clz(value);
#elif NANOFMT_HAS_BSR
            unsigned long index = 0;
            return _BitScanReverse(&index, value) ? 31 - index : 32;
#else
#    error "nanofmt::detail::countl_zero not implemented for this compiler/platform"
            return -1;
#endif
        }

        constexpr int countl_zero(unsigned long long value) noexcept {
#if NANOFMT_HAS_BUILTIN_CLZ || NANOFMT_CLANG_OR_GCC
            if (value == 0) {
                return sizeof(value) * 8;
            }
            return __builtin_clzll(value);
#elif NANOFMT_HAS_BSR
            unsigned long index = 0;
            return _BitScanReverse64(&index, value) ? 63 - index : 64;
#else
#    error "nanofmt::detail::countl_zero not implemented for this compiler/platform"
            return -1;
#endif
        }

        template <typename IntT>
        constexpr auto abs(IntT value) noexcept {
            using Unsigned = std::make_unsigned_t<IntT>;

#if NANOFMT_IF_CONSTEXPR
            if constexpr (std::is_signed_v<IntT>)
#endif
            {
                if (value < 0) {
                    return static_cast<Unsigned>(0 - static_cast<Unsigned>(value));
                }
            }
            return static_cast<Unsigned>(value);
        }

    } // namespace detail
} // namespace NANOFMT_NS
