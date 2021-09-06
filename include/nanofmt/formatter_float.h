// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

#include "to_chars.h"

#include <cmath>

namespace nanofmt::detail {
    template <typename FloatT>
    struct float_formatter {
        format_spec spec;

        constexpr char const* parse(char const* in, char const* end) noexcept {
            return parse_spec(in, end, spec, "aAeEfFgG");
        }

        void format(FloatT value, buffer& buf) const noexcept {
            if (!std::signbit(value)) {
                if (spec.sign == '+') {
                    buf.append('+');
                }
                else if (spec.sign == ' ') {
                    buf.append(' ');
                }
            }

            switch (spec.type) {
                default:
                case 'g':
                case 'G':
                    buf.advance_to(to_chars(
                        buf.pos,
                        buf.end,
                        value,
                        spec.type == 'G' ? float_format::general_upper : float_format::general,
                        spec.precision));
                    break;
                case 'e':
                case 'E':
                    buf.advance_to(to_chars(
                        buf.pos,
                        buf.end,
                        value,
                        spec.type == 'E' ? float_format::scientific_upper : float_format::scientific,
                        spec.precision < 0 ? 6 : spec.precision));
                    break;
                case 'f':
                case 'F':
                    buf.advance_to(to_chars(
                        buf.pos,
                        buf.end,
                        value,
                        float_format::fixed,
                        spec.precision < 0 ? 6 : spec.precision));
                    break;
            }
        }
    };
} // namespace nanofmt::detail

namespace nanofmt {
    template <>
    struct formatter<double> : detail::float_formatter<double> {};

    template <>
    struct formatter<float> : detail::float_formatter<float> {};
} // namespace nanofmt
