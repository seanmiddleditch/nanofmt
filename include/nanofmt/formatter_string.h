// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#pragma once

namespace nanofmt::detail {
    struct string_formatter_base {
        format_spec spec;

        constexpr char const* parse(char const* in, char const* end) noexcept {
            return parse_spec(in, end, spec);
        }

        void do_format(buffer& buf, char const* const str, size_t length) const {
            if (spec.width < 0 || length >= static_cast<size_t>(spec.width)) {
                buf.append(str, length);
                return;
            }

            auto const padding = static_cast<size_t>(spec.width) - length;
            if (spec.align < 0) {
                buf.append(str, length);
                buf.fill_n(' ', padding);
            }
            else {
                buf.fill_n(' ', padding);
                buf.append(str, length);
            }
        }
    };

    template <typename StringT>
    struct string_formatter : string_formatter_base {
        void format(StringT const& value, buffer& buf) const {
            string_view const string = to_string_view(value);
            string_formatter_base::do_format(buf, string.data(), string.size());
        }
    };
} // namespace nanofmt::detail

namespace nanofmt {
    template <>
    struct formatter<string_view> : detail::string_formatter<string_view> {};
    template <>
    struct formatter<char const*> : detail::string_formatter<char const*> {};
    template <size_t N>
    struct formatter<char const (&)[N]> : detail::string_formatter<char const (&)[N]> {};

    template <>
    struct formatter<char> : formatter<void> {
        constexpr void format(char ch, buffer& buf) {
            buf.append(&ch, 1);
        }
    };

} // namespace nanofmt
