// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "nanofmt/charconv.h"
#include "numeric_utils.h"
#if NANOFMT_FLOAT
#    include "nanofmt/dragonbox.h"
#endif
#include "nanofmt/format.h"

#include <cmath>

namespace NANOFMT_NS::detail {
    template <typename IntegerT>
    static char* to_chars_impl(char* dest, char const* end, IntegerT value, int_format fmt) noexcept;

    template <typename CarrierT, typename FloatT>
    static char* to_chars_impl(char* dest, char const* end, FloatT value, float_format fmt, int precision) noexcept;

    template <typename UnsignedIntT>
    static char* to_chars_impl_decimal(char* dest, char const* end, UnsignedIntT value) noexcept;

    template <char A = 'a', typename UnsignedIntT>
    static char* to_chars_impl_hex(char* dest, char const* end, UnsignedIntT value) noexcept;

    template <typename UnsignedIntT>
    static char* to_chars_impl_binary(char* dest, char const* end, UnsignedIntT value) noexcept;

    template <typename UnsignedIntT>
    static char* to_chars_n_round(char* dest, char const* end, UnsignedIntT value, std::size_t count) noexcept;

    template <char E = 'e', bool TrailingZeroes = true, typename CarrierT>
    char* to_chars_impl_scientific(
        char* dest,
        char const* end,
        CarrierT significand,
        int exponent,
        int precision) noexcept;

    template <bool TrailingZeroes = true, typename CarrierT>
    static char* to_chars_impl_fixed(
        char* dest,
        char const* end,
        CarrierT significand,
        int exponent,
        int precision) noexcept;

    template <char E = 'e', typename CarrierT>
    static char* to_chars_impl_general(
        char* dest,
        char const* end,
        CarrierT significand,
        int exponent,
        int precision) noexcept;

#if NANOFMT_FLOAT
    static char* to_chars_impl_nonfinite(
        char* dest,
        char const* end,
        bool negative,
        bool infinite,
        bool upper) noexcept;
#endif

    // maximum significand for double is 17 decimal digits
    static constexpr size_t significand_max_digits10 = 17;
} // namespace NANOFMT_NS::detail

namespace NANOFMT_NS {
    char* to_chars(char* dest, char const* end, signed char value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, unsigned char value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, signed short value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, unsigned short value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, signed int value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, unsigned int value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, signed long value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, unsigned long value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, signed long long value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

    char* to_chars(char* dest, char const* end, unsigned long long value, int_format fmt) noexcept {
        return detail::to_chars_impl(dest, end, value, fmt);
    }

#if NANOFMT_FLOAT
    char* to_chars(char* dest, char const* end, float value, float_format fmt) noexcept {
        return detail::to_chars_impl<std::uint32_t>(dest, end, value, fmt, -1);
    }

    char* to_chars(char* dest, char const* end, double value, float_format fmt) noexcept {
        return detail::to_chars_impl<std::uint64_t>(dest, end, value, fmt, -1);
    }

    char* to_chars(char* dest, char const* end, float value, float_format fmt, int precision) noexcept {
        return detail::to_chars_impl<std::uint32_t>(dest, end, value, fmt, precision);
    }

    char* to_chars(char* dest, char const* end, double value, float_format fmt, int precision) noexcept {
        return detail::to_chars_impl<std::uint64_t>(dest, end, value, fmt, precision);
    }
#endif

    template <typename IntegerT>
    char* detail::to_chars_impl(char* dest, char const* end, IntegerT value, int_format fmt) noexcept {
        if (value < 0 && dest != end) {
            *dest++ = '-';
        }

        if (value == 0) {
            *dest++ = '0';
            return dest;
        }

        auto const abs_value = detail::abs(value);

        switch (fmt) {
            case int_format::decimal:
                return detail::to_chars_impl_decimal(dest, end, abs_value);
            case int_format::hex:
                return detail::to_chars_impl_hex<'a'>(dest, end, abs_value);
            case int_format::hex_upper:
                return detail::to_chars_impl_hex<'A'>(dest, end, abs_value);
            case int_format::binary:
                return detail::to_chars_impl_binary(dest, end, abs_value);
            default:
                return dest;
        }
    }

    template <typename CarrierT, typename FloatT>
    char* detail::to_chars_impl(char* dest, char const* end, FloatT value, float_format fmt, int precision) noexcept {
        static_assert(sizeof(CarrierT) == sizeof(FloatT));

        if (!std::isfinite(value)) {
            switch (fmt) {
                case float_format::scientific_upper:
                case float_format::general_upper:
                    return to_chars_impl_nonfinite(dest, end, std::signbit(value), std::isinf(value), /*upper=*/true);
                default:
                    return to_chars_impl_nonfinite(
                        dest,
                        end,
                        std::signbit(value),
                        std::isinf(value),
                        /*upper=*/false);
            }
        }

        CarrierT significand = 0;
        int exponent = 0;

        if (value != 0) {
            auto const db_result = nanofmt::dragonbox::to_decimal(
                value,
                nanofmt::dragonbox::policy::sign::ignore,
                nanofmt::dragonbox::policy::cache::compact,
                nanofmt::dragonbox::policy::trailing_zero::remove,
                nanofmt::dragonbox::policy::binary_to_decimal_rounding::to_even);
            significand = db_result.significand;
            exponent = db_result.exponent;
        }

        // sign
        if (std::signbit(value)) {
            dest = put(dest, end, '-');
        }

        switch (fmt) {
            case float_format::scientific:
                return detail::to_chars_impl_scientific(dest, end, significand, exponent, precision);
            case float_format::scientific_upper:
                return detail::to_chars_impl_scientific<'E'>(dest, end, significand, exponent, precision);
            case float_format::fixed:
                return detail::to_chars_impl_fixed(dest, end, significand, exponent, precision);
            case float_format::general:
                return detail::to_chars_impl_general(dest, end, significand, exponent, precision);
            case float_format::general_upper:
                return detail::to_chars_impl_general<'E'>(dest, end, significand, exponent, precision);
            case float_format::hex: // FIXME: implement
            case float_format::hex_upper: // FIXME: implement
            default:
                return dest;
        }
    }

    template <typename UnsignedIntT>
    char* detail::to_chars_impl_decimal(char* dest, char const* end, UnsignedIntT value) noexcept {
        static_assert(std::is_unsigned_v<UnsignedIntT>);

        // pathological case of an empty buffer;
        // checking this here simplifies following code
        // by assuring we can write at least one character
        //
        if (dest == end) {
            return dest;
        }

        int const total_digits = count_digits(value);
        size_t const available = end - dest;
        int const digits = min<int>(total_digits, static_cast<int>(available));

        // trim any trailing digits if our value exceeds our buffer size
        //
        if (digits != total_digits) {
            value = rshift10(value, total_digits - digits);
        }

        // write digits; FIXME add table to do this in two-digit
        // batches to reduce divisions
        //
        char* ptr = dest + digits;
        do {
            *--ptr = '0' + (value % 10);
            value /= 10;
        } while (value != 0);

        return dest + digits;
    }

    template <char A, typename UnsignedIntT>
    char* detail::to_chars_impl_hex(char* dest, char const* end, UnsignedIntT value) noexcept {
        static_assert(std::is_unsigned_v<UnsignedIntT>);

        // constants to process nibbles (half-bytes, or 4 bits)
        //
        constexpr int bits_per_nibble = 4;
        constexpr int nibbles_per_byte = 2;
        constexpr int total_nibbles = sizeof(value) * nibbles_per_byte;
        constexpr UnsignedIntT nibble_mask = 0b1111;

        // find the first non-zero nibble for our initial position
        //
        int const zero_nibbles = countl_zero(value) >> nibbles_per_byte;
        int shift = (total_nibbles - zero_nibbles - 1) * bits_per_nibble;
        UnsignedIntT mask = nibble_mask << shift;

        // continously "chop" nibbles off the integer for generating
        // the appropriate hex digit
        //
        // works MSB to LSB so that we can early-terminate when the
        // output buffer is full
        //
        // we slide the mask as we chop nibbles and have finished
        // when the mask completely slides off to zero
        //
        while (mask != 0 && dest != end) {
            UnsignedIntT const nibble = (value & mask) >> shift;

            if (nibble < 10) {
                *dest++ = static_cast<char>('0' + nibble);
            }
            else {
                *dest++ = static_cast<char>(A + nibble - 10);
            }

            shift -= bits_per_nibble;
            mask >>= bits_per_nibble;
        }

        return dest;
    }

    template <typename UnsignedIntT>
    char* detail::to_chars_impl_binary(char* dest, char const* end, UnsignedIntT value) noexcept {
        static_assert(std::is_unsigned_v<UnsignedIntT>);

        // find the initial non-zero bit in the input
        //
        int const zeroes = countl_zero(value);
        UnsignedIntT mask = 1ull << ((sizeof(value) * 8 - zeroes) - 1);

        // iterate the bits from MSB to LSB, so that we
        // can terminate if the buffer becomes full
        //
        while (mask != 0 && dest != end) {
            *dest++ = '0' + ((value & mask) != 0);
            mask >>= 1;
        }

        return dest;
    }

    template <typename UnsignedIntT>
    char* detail::to_chars_n_round(char* dest, char const* end, UnsignedIntT value, std::size_t count) noexcept {
        static_assert(std::is_unsigned_v<UnsignedIntT>);

        if (count == 0) {
            return dest;
        }

        char tmp[significand_max_digits10] = {};

        // FIXME: we could use a much faster to_chars here; e.g. we could
        // keep this in reversed order, and we know it'll never truncate
        //
        size_t const tmp_len = to_chars(tmp, tmp + sizeof tmp, value) - tmp;

        // if we have as many or fewer digits as our round position, we're done (nothing
        // to round from)
        //
        if (tmp_len <= count) {
            return copy_to_n(dest, end, tmp, tmp_len);
        }

        char const remainder_digit = tmp[count];

        // if the remainder is less than 1/2 then we have no rounding to do
        if (remainder_digit < '5') {
            return copy_to_n(dest, end, tmp, count);
        }

        // if our remainder is exactly 1/2 we round to nearest even;
        // the following loop handles rounding _up_, so we only need
        // to handle the special case of rounding _down_ here
        //
        // we identify "exactly 1/2" as having exactly one remaining
        // digit equal to 5; this only works because we have dragonbox
        // strip trialing zeroes!
        //
        if (remainder_digit == '5' && tmp_len - 1 == count) {
            char const last_digit = tmp[count - 1];
            bool const is_even = ((last_digit - '0') & 1) == 0;
            if (is_even) {
                return copy_to_n(dest, end, tmp, count);
            }
        }

        // we're rounding up; walk backwards and round up characters so long
        // as we have a 9 to round up, and stop otherwise
        //
        char* ptr = tmp + count;
        while (ptr-- > tmp) {
            if (*ptr == '9') {
                *ptr = '0';
                continue;
            }
            ++*ptr;
            break;
        }

        return copy_to_n(dest, end, tmp, count);
    }

    template <char E, bool TrailingZeroes, typename CarrierT>
    char* detail::to_chars_impl_scientific(
        char* dest,
        char const* end,
        CarrierT significand,
        int exponent,
        int precision) noexcept {
        static_assert(std::is_unsigned_v<CarrierT>);

        // generate significand digits, with truncation/rounding when appropriate
        //
        char digits[significand_max_digits10] = {};
        char const* const digits_end = precision < 0
            ? to_chars(digits, digits + sizeof digits, significand)
            : to_chars_n_round(digits, digits + sizeof digits, significand, precision + 1);
        int const digits_count = static_cast<int>(digits_end - digits);

        // calculate our adjusted exponent, which shifts the decimal point to be just after
        // the most significant digit (based on the actual significand, not the truncated/
        // rounded version)
        //
        int const adjusted_exp = exponent + count_digits(significand) - 1;
        auto const absolute_exp = abs(adjusted_exp);

        // calculate the length of our fractional portion after the decimal point.
        // if a precision is provided, that specifies the precise number of digits
        // in the fractional portion; otherwise, it's just whatever is "leftover"
        // from the significand
        //
        int const fract_digits = precision < 0 ? digits_count - 1 : precision;

        // write most significant digit
        dest = put(dest, end, digits[0]);

        // write fractional part, if any
        if (fract_digits > 0) {
            dest = put(dest, end, '.');

            int const fract_available = digits_count - 1;
            int const fract_buffer_size = min<int>(fract_available, fract_digits);
            dest = copy_to_n(dest, end, digits + 1, fract_buffer_size);

            // fill in any trailing zeroes only if our mode allows it; this is effectively
            // always going to be the precision minus the number of fractional digits pulled
            // from the significand
            //
            if constexpr (TrailingZeroes) {
                dest = fill_n(dest, end, '0', static_cast<size_t>(fract_digits) - fract_buffer_size);
            }
        }

        // write exponent, format [eE][+-][d]dd (zero-padded if exponent if less than two digits)
        //
        dest = put(dest, end, E);
        dest = put(dest, end, adjusted_exp < 0 ? '-' : '+');
        if (absolute_exp < 10) {
            dest = put(dest, end, '0');
        }
        return to_chars(dest, end, absolute_exp);
    }

    template <bool TrailingZeroes, typename CarrierT>
    char* detail::to_chars_impl_fixed(
        char* dest,
        char const* end,
        CarrierT significand,
        int exponent,
        int precision) noexcept {
        static_assert(std::is_unsigned_v<CarrierT>);

        int const sig_digits = count_digits(significand);

        // find the location of the decimmal point
        //
        int const decimal_pos = sig_digits + exponent;

        // in the case that the decimal position is outside our
        // precision, we're just going to print zeroes and don't
        // even need to turn our significand into decimals
        //
        if (precision >= 0 && -decimal_pos > precision) {
            // we will only display zeroes due to truncation
            if constexpr (TrailingZeroes) {
                if (precision > 0) {
                    dest = copy_to_n(dest, end, "0.", 2);
                    return fill_n(dest, end, '0', precision);
                }
            }
            return put(dest, end, '0');
        }

        // calculate how many of the significant digits _might_ be visible,
        // and how many _are_ visible
        int const max_visible_digits = precision < 0 ? sig_digits : decimal_pos + precision;
        int const visible_digits = min<int>(sig_digits, max_visible_digits);

        // calculate the length of the integer (pre-decimal) part
        //
        int const integer_length = max<int>(decimal_pos, 0);

        // calculate the visible integer (pre-decimal) and fractional (post-decimal) digits
        // of the significand
        //
        int const integer_digits = min<int>(visible_digits, integer_length);
        int const fract_digits = visible_digits - integer_digits;

        // generate significand digits; we know the full buffer will always be used since
        // max_visible_digits is always _at least_ the actual digits we will actually display
        //
        char digits[significand_max_digits10];
        to_chars_n_round(digits, digits + sizeof digits, significand, visible_digits);

        // write the integer portion of the significand and enough zeroes to reach
        // the decimal point; if there is no integer portion of the significand,
        // just write a zero
        if (decimal_pos > 0) {
            dest = copy_to_n(dest, end, digits, integer_digits);
            if (integer_length > integer_digits) {
                dest = fill_n(dest, end, '0', static_cast<size_t>(integer_length) - integer_digits);
            }
        }
        else {
            dest = put(dest, end, '0');
        }

        // write the fractional portion of the signifand and any appropriate trailing zeroes
        //
        if (fract_digits > 0 || (precision > 0 && TrailingZeroes)) {
            int const fract_offset = max<int>(-decimal_pos, 0);

            dest = put(dest, end, '.');
            dest = fill_n(dest, end, '0', fract_offset);
            dest = copy_to_n(dest, end, digits + integer_digits, fract_digits);

            if constexpr (TrailingZeroes) {
                int const fract_length_unused = fract_digits + fract_offset;
                int const fract_tail_zeroes = max<int>(precision - fract_length_unused, 0);
                dest = fill_n(dest, end, '0', fract_tail_zeroes);
            }
        }

        return dest;
    }

    template <char E, typename CarrierT>
    char* detail::to_chars_impl_general(
        char* dest,
        char const* end,
        CarrierT significand,
        int exponent,
        int precision) noexcept {
        int const sig_digits = count_digits(significand);

        // C11 spec (see also https://en.cppreference.com/w/c/io/fprintf)
        //
        // For the g conversion style conversion with style e or f will be performed.
        // For the G conversion style conversion with style E or F will be performed.
        //
        // Let P equal the precision if nonzero, 6 if the precision is not specified,
        // or 1 if the precision is 0.
        //
        // Then, if a conversion with style E would have an exponent of X:
        //
        //   if P > X >= -4, the conversion is with style f or F and precision P - 1 - X.
        //   otherwise, the conversion is with style e or E and precision P - 1.
        //

        int const P = (precision < 0) ? 6 : (precision == 0) ? 1 : precision;
        int const X = exponent + sig_digits - 1;

        if (P > X && X >= -4) {
            return to_chars_impl_fixed</*TrailingZeroes=*/false>(dest, end, significand, exponent, P - 1 - exponent);
        }
        return to_chars_impl_scientific<E, /*TrailingZeroes=*/false>(dest, end, significand, exponent, P - 1);
    }

#if NANOFMT_FLOAT
    char* detail::to_chars_impl_nonfinite(
        char* dest,
        char const* end,
        bool negative,
        bool infinite,
        bool upper) noexcept {
        if (negative) {
            dest = put(dest, end, '-');
        }
        if (infinite) {
            return copy_to(dest, end, upper ? "INF" : "inf");
        }
        return copy_to(dest, end, upper ? "NAN" : "nan");
    }
#endif
} // namespace NANOFMT_NS
