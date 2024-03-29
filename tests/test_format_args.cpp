// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "test_utils.h"

#include "nanofmt/format.h"

#include <doctest/doctest.h>

namespace {
    template <typename T>
    constexpr auto to_arg(T const& value) noexcept {
        return ::NANOFMT_NS::make_format_args(value).values[0];
    }
} // namespace

TEST_CASE("nanofmt.format_arg.misc") {
    using namespace NANOFMT_NS;

    CHECK(to_arg(true).tag == format_arg::type::t_bool);
    CHECK(to_arg('c').tag == format_arg::type::t_char);
}

TEST_CASE("nanofmt.format_arg.integers") {
    using namespace NANOFMT_NS;

    SUBCASE("misc") {
        CHECK(to_arg(true).tag == format_arg::type::t_bool);
        CHECK(to_arg('c').tag == format_arg::type::t_char);
    }

    SUBCASE("signed") {
        using signed_char = signed char;

        CHECK(to_arg(signed_char{}).tag == format_arg::type::t_int);
        CHECK(to_arg(short{}).tag == format_arg::type::t_int);
        CHECK(to_arg(0).tag == format_arg::type::t_int);
        CHECK(to_arg(0l).tag == format_arg::type::t_long);
        CHECK(to_arg(0ll).tag == format_arg::type::t_longlong);
    }

    SUBCASE("unsigned") {
        using unsigned_char = unsigned char;
        using unsigned_short = unsigned short;

        CHECK(to_arg(unsigned_char{}).tag == format_arg::type::t_uint);
        CHECK(to_arg(unsigned_short{}).tag == format_arg::type::t_uint);
        CHECK(to_arg(0u).tag == format_arg::type::t_uint);
        CHECK(to_arg(0ul).tag == format_arg::type::t_ulong);
        CHECK(to_arg(0ull).tag == format_arg::type::t_ulonglong);
    }
}

TEST_CASE("nanofmt.format_arg.floats") {
    using namespace NANOFMT_NS;

    CHECK(to_arg(0.f).tag == format_arg::type::t_float);
    CHECK(to_arg(0.0).tag == format_arg::type::t_double);
}

TEST_CASE("nanofmt.format_arg.pointers") {
    using namespace NANOFMT_NS;

    SUBCASE("pointers") {
        void const* cptr = nullptr;
        void* ptr = nullptr;

        CHECK(to_arg(cptr).tag == format_arg::type::t_voidptr);
        CHECK(to_arg(ptr).tag == format_arg::type::t_voidptr);
        CHECK(to_arg(nullptr).tag == format_arg::type::t_voidptr);
    }

    SUBCASE("C strings") {
        char chars[16] = {};
        char const* cstr = nullptr;
        char* str = nullptr;

        CHECK(to_arg(chars).tag == format_arg::type::t_cstring);
        CHECK(to_arg("literal").tag == format_arg::type::t_cstring);
        CHECK(to_arg(cstr).tag == format_arg::type::t_cstring);
        CHECK(to_arg(str).tag == format_arg::type::t_cstring);
    }
}

namespace {
    // clang-format off
    enum NANOFMT_GSL_SUPPRESS(enum.3) cenum { cenum_value };
    // clang-format on
    enum class enum_class { value };
    enum class chonky_enum_class : long long { value };
} // namespace

TEST_CASE("nanofmt.format_arg.enums") {
    using namespace NANOFMT_NS;

    CHECK(to_arg(cenum_value).tag == format_arg::type::t_int);
    CHECK(to_arg(enum_class::value).tag == format_arg::type::t_int);
    CHECK(to_arg(chonky_enum_class::value).tag == format_arg::type::t_longlong);
}

namespace custom {
    enum class enum_class { value };
    struct struct_type {};
    class class_type {};
} // namespace custom

namespace NANOFMT_NS {
    struct custom_formatter_base {
        constexpr char const* parse(char const* in, char const*) noexcept {
            return in;
        }
        template <typename T>
        void format(T const&, format_output&) {}
    };

    template <>
    struct formatter<custom::enum_class> : custom_formatter_base {};
    template <>
    struct formatter<custom::struct_type> : custom_formatter_base {};
    template <>
    struct formatter<custom::class_type> : custom_formatter_base {};
} // namespace NANOFMT_NS

TEST_CASE("nanofmt.format_arg.custom") {
    using namespace NANOFMT_NS;

    SUBCASE("enums") {
        CHECK(to_arg(custom::enum_class::value).tag == format_arg::type::t_custom);
    }

    SUBCASE("objects") {
        custom::struct_type st;
        const custom::struct_type cst;
        custom::class_type ct;
        const custom::class_type cct;

        CHECK(to_arg(st).tag == format_arg::type::t_custom);
        CHECK(to_arg(cst).tag == format_arg::type::t_custom);
        CHECK(to_arg(ct).tag == format_arg::type::t_custom);
        CHECK(to_arg(cct).tag == format_arg::type::t_custom);
    }
}
