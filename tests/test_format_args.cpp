// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "nanofmt/format.h"

#include <catch2/catch_test_macros.hpp>

namespace {
    template <typename T>
    constexpr auto to_arg(T const& value) noexcept {
        return nanofmt::make_format_args(value).values[0];
    }
} // namespace

TEST_CASE("nanofmt.format_arg.misc", "[nanofmt][format_arg]") {
    using namespace nanofmt;

    CHECK(to_arg(true).type == detail::value_type::t_bool);
    CHECK(to_arg('c').type == detail::value_type::t_char);
}

TEST_CASE("nanofmt.format_arg.integers", "[nanofmt][format_arg][integers]") {
    using namespace nanofmt;

    SECTION("misc") {
        CHECK(to_arg(true).type == detail::value_type::t_bool);
        CHECK(to_arg('c').type == detail::value_type::t_char);
    }

    SECTION("signed") {
        using signed_char = signed char;

        CHECK(to_arg(signed_char{}).type == detail::value_type::t_int);
        CHECK(to_arg(short{}).type == detail::value_type::t_int);
        CHECK(to_arg(0).type == detail::value_type::t_int);
        CHECK(to_arg(0l).type == detail::value_type::t_long);
        CHECK(to_arg(0ll).type == detail::value_type::t_longlong);
    }

    SECTION("unsigned") {
        using unsigned_char = unsigned char;
        using unsigned_short = unsigned short;

        CHECK(to_arg(unsigned_char{}).type == detail::value_type::t_uint);
        CHECK(to_arg(unsigned_short{}).type == detail::value_type::t_uint);
        CHECK(to_arg(0u).type == detail::value_type::t_uint);
        CHECK(to_arg(0ul).type == detail::value_type::t_ulong);
        CHECK(to_arg(0ull).type == detail::value_type::t_ulonglong);
    }
}

TEST_CASE("nanofmt.format_arg.floats", "[nanofmt][format_arg][floats]") {
    using namespace nanofmt;

    CHECK(to_arg(0.f).type == detail::value_type::t_float);
    CHECK(to_arg(0.0).type == detail::value_type::t_double);
}

TEST_CASE("nanofmt.format_arg.pointers", "[nanofmt][format_arg][pointers][strings]") {
    using namespace nanofmt;

    SECTION("pointers") {
        void const* cptr;
        void* ptr;

        CHECK(to_arg(cptr).type == detail::value_type::t_voidptr);
        CHECK(to_arg(ptr).type == detail::value_type::t_voidptr);
        CHECK(to_arg(nullptr).type == detail::value_type::t_voidptr);
    }

    SECTION("C strings") {
        char chars[16] = {};
        char const* cstr = nullptr;
        char* str = nullptr;

        CHECK(to_arg(chars).type == detail::value_type::t_cstring);
        CHECK(to_arg("literal").type == detail::value_type::t_cstring);
        CHECK(to_arg(cstr).type == detail::value_type::t_cstring);
        CHECK(to_arg(str).type == detail::value_type::t_cstring);
    }
}

TEST_CASE("nanofmt.format_arg.enums", "[nanofmt][format_arg][pointers][enums]") {
    using namespace nanofmt;

    enum
#if __has_cpp_attribute(gsl::suppress)
        [[gsl::suppress(Enum .3)]]
#endif
        cenum{cenum_value};
    enum class enum_class { value };
    enum class chonky_enum_class : long long { value };

    CHECK(to_arg(cenum_value).type == detail::value_type::t_int);
    CHECK(to_arg(enum_class::value).type == detail::value_type::t_int);
    CHECK(to_arg(chonky_enum_class::value).type == detail::value_type::t_longlong);
}

namespace custom {
    enum class enum_class { value };
    struct struct_type {};
    class class_type {};
} // namespace custom

namespace nanofmt {
    template <>
    struct formatter<custom::enum_class> : formatter<void> {};
    template <>
    struct formatter<custom::struct_type> : formatter<void> {};
    template <>
    struct formatter<custom::class_type> : formatter<void> {};
} // namespace nanofmt

TEST_CASE("nanofmt.format_arg.custom", "[nanofmt][format_arg][pointers][custom]") {
    using namespace nanofmt;

    SECTION("enums") {
        CHECK(to_arg(custom::enum_class::value).type == detail::value_type::t_custom);
    }

    SECTION("objects") {
        custom::struct_type st;
        const custom::struct_type cst;
        custom::class_type ct;
        const custom::class_type cct;

        CHECK(to_arg(st).type == detail::value_type::t_custom);
        CHECK(to_arg(cst).type == detail::value_type::t_custom);
        CHECK(to_arg(ct).type == detail::value_type::t_custom);
        CHECK(to_arg(cct).type == detail::value_type::t_custom);
    }
}
