// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "fwd_only_type.h"
#include "test_utils.h"

#include "nanofmt/format.h"
#include "nanofmt/std_string.h"

#include <doctest/doctest.h>

#include <cstdint>
#include <limits>

enum class standard_enum { one, two };
enum class custom_enum { foo, bar };

struct custom_type {
    int value = 0;
};

struct unknown {};

namespace NANOFMT_NS {
    template <>
    struct formatter<custom_enum> : formatter<char const*> {
        void format(custom_enum value, format_context& ctx) {
            switch (value) {
                case custom_enum::foo:
                    formatter<char const*>::format("foo", ctx);
                    break;
                case custom_enum::bar:
                    formatter<char const*>::format("bar", ctx);
                    break;
            }
        }
    };

    template <>
    struct formatter<custom_type> {
        constexpr char const* parse(format_parse_context& ctx) noexcept {
            return ctx.begin();
        }

        void format(custom_type custom, format_context& ctx) {
            ctx.format("custom{{{}}", custom.value);
        }
    };
} // namespace NANOFMT_NS

static_assert(NANOFMT_NS::detail::has_formatter<custom_type>::value, "has_formatter failed");
static_assert(NANOFMT_NS::detail::has_formatter<std::string>::value, "has_formatter failed");

TEST_CASE("nanofmt.format.core") {
    using namespace NANOFMT_NS;

    SUBCASE("format_to overflow") {
        char buffer[12];
        std::memset(buffer, 0xfe, sizeof buffer);

        char const* const end = format_to(buffer, "Hello, {}! {:09d}", "World", 9001);
        REQUIRE(*end == '\0');

        CHECK(std::strcmp(buffer, "Hello, Worl") == 0);
    }

    SUBCASE("format_to_n overflow") {
        char buffer[12];
        char* const end = format_to_n(buffer, sizeof buffer, "Hello, {}! {:09d}", "World", 9001);

        CHECK((end - buffer) == 12);
        CHECK(std::strncmp(buffer, "Hello, World", sizeof buffer) == 0);
    }
}

TEST_CASE("nanofmt.format.append") {
    using namespace NANOFMT_NS;

    char buffer[12] = {};
    format_to(buffer, "Hello");
    format_append_to(buffer, "{} ", ',');
    char* const end = format_append_to(buffer, "World! {:09d}", 9001);

    CHECK((end - buffer) == 11);
    CHECK(std::strcmp(buffer, "Hello, Worl") == 0);
}

TEST_CASE("nanofmt.vformat.append") {
    using namespace NANOFMT_NS;

    char buffer[12] = {};
    vformat_to(buffer, "{} + {}", make_format_args(1, 2));
    char const* const end = vformat_append_to(buffer, " = {}", make_format_args(3));

    CHECK((end - buffer) == 9);
    CHECK(std::strcmp(buffer, "1 + 2 = 3") == 0);
}

TEST_CASE("nanofmt.format.integers") {
    using namespace NANOFMT_NS::test;

    SUBCASE("width and fill") {
        CHECK(sformat("{:6d}", 1234) == "  1234");
        CHECK(sformat("{:6d}", -1234) == " -1234");
    }

    SUBCASE("zero pad") {
        CHECK(sformat("{:06}", 1234) == "001234");
        CHECK(sformat("{:06}", -1234) == "-01234");
    }

    SUBCASE("precision") {
        // BUG? -- fmtlib/std::format doesn't support precision for integral types, should we nuke this?
        CHECK(sformat("{:.4}", 123456) == "1234");
        CHECK(sformat("{:.4b}", 0b1001'0110) == "1001");
    }

    SUBCASE("char") {
        CHECK(sformat("{:d}", ' ') == "32");
    }

    SUBCASE("hex") {
        CHECK(sformat("{:x}", ~16u) == "ffffffef");
        CHECK(sformat("{:08x}", 0xDEAD) == "0000dead");
        CHECK(sformat("{:08X}", 0xDEAD) == "0000DEAD");
    }

    SUBCASE("binary") {
        CHECK(sformat("{:b}", 0) == "0");
        CHECK(sformat("{:b}", 0b11011) == "11011");
        CHECK(sformat("{:b}", 5) == "101");
        CHECK(sformat("{:b}", -256) == "-100000000");
    }
}

TEST_CASE("nanofmt.format.floating") {
    using namespace NANOFMT_NS::test;

    SUBCASE("precision") {
        CHECK(sformat("{:.1f}", 1.55) == "1.6");
        CHECK(sformat("{:.1e}", 1.45) == "1.4e+00");
        CHECK(sformat("{}", std::numeric_limits<float>::max()) == "3.40282e+38");
    }

    SUBCASE("signs") {
        CHECK(sformat("{:+.3}", 1.0) == "+1");
        CHECK(sformat("{:+.3}", -1.0) == "-1");
        CHECK(sformat("{:-.3}", 1.0) == "1");
        CHECK(sformat("{:-.3}", -1.0) == "-1");
        CHECK(sformat("{: .3}", 1.0) == " 1");
        CHECK(sformat("{: .3}", -1.0) == "-1");
    }

    SUBCASE("nonfinite") {
        CHECK(sformat("{:f}", std::numeric_limits<float>::infinity()) == "inf");
        CHECK(sformat("{:f}", -std::numeric_limits<float>::infinity()) == "-inf");

        CHECK(sformat("{:f}", std::numeric_limits<float>::quiet_NaN()) == "nan");
        CHECK(sformat("{:f}", -std::numeric_limits<float>::quiet_NaN()) == "-nan");
    }

    SUBCASE("casing") {
        CHECK(sformat("{:.2E}", -12.99) == "-1.30E+01");
        CHECK(sformat("{:E}", -std::numeric_limits<float>::infinity()) == "-INF");

        CHECK(sformat("{:G}", -.000'123) == "-0.000123");
        CHECK(sformat("{:G}", std::numeric_limits<float>::quiet_NaN()) == "NAN");
    }
}

TEST_CASE("nanofmt.format.strings") {
    using namespace NANOFMT_NS::test;

    SUBCASE("char arrays") {
        char const s[] = "array";

        CHECK(sformat("{}", "test") == "test");
        CHECK(sformat("{}", s) == "array");
    }

    SUBCASE("stdlib") {
        CHECK(sformat("{}", std::string("test")) == "test");
        CHECK(sformat("{}", std::string_view("test")) == "test");

        CHECK(sformat("{}{}{}", std::string_view("ab"), std::string("cd"), "ef") == "abcdef");

        CHECK(sformat(nanofmt::format_string(std::string("a{}c")), "b") == "abc");
    }

    SUBCASE("width and fill") {
        CHECK(sformat("{:<8}{:05}", "value", 42) == "value   00042");
    }
}

TEST_CASE("nanofmt.format.bools") {
    using namespace NANOFMT_NS::test;

    SUBCASE("default") {
        CHECK(sformat("{}", true) == "true");
        CHECK(sformat("{}", false) == "false");
    }

    SUBCASE("integer") {
        CHECK(sformat("{:d}", true) == "1");
        CHECK(sformat("{:d}", false) == "0");
    }
}

TEST_CASE("nanofmt.format.pointers") {
    using namespace NANOFMT_NS::test;

    SUBCASE("nullptr") {
        CHECK(sformat("{}", nullptr) == "0x0");
    }

    SUBCASE("raw") {
        void const* ptr = reinterpret_cast<void const*>(static_cast<std::uintptr_t>(0xDEADC0DE));
        int const* iptr = reinterpret_cast<int const*>(static_cast<std::uintptr_t>(0xFEFEFEFE));

        CHECK(sformat("{}", ptr) == "0xdeadc0de");
        CHECK(sformat("{}", iptr) == "0xfefefefe");
    }
}

TEST_CASE("nanofmt.format.enums") {
    using namespace NANOFMT_NS::test;

    SUBCASE("standard") {
        CHECK(sformat("{0}", standard_enum::two) == "1");
    }

    SUBCASE("custom") {
        CHECK(sformat("{}", custom_enum::bar) == "bar");
    }
}

TEST_CASE("nanofmt.format.custom") {
    using namespace NANOFMT_NS::test;

    custom_type local{7};
    custom_type& ref = local;

    custom_type const clocal{7};
    custom_type const& cref = clocal;

    CHECK(sformat("{}", custom_type{1}) == "custom{1}");
    CHECK(sformat("{}", local) == "custom{7}");
    CHECK(sformat("{}", ref) == "custom{7}");
    CHECK(sformat("{}", clocal) == "custom{7}");
    CHECK(sformat("{}", cref) == "custom{7}");

    CHECK(sformat("{}", fwd_only_type{}) == "fwd_only_type");
}

TEST_CASE("nanofmt.format.length") {
    using namespace NANOFMT_NS;

    CHECK(format_length("{}") == 0);
    CHECK(format_length("{}", 777) == 3);

    // https://github.com/seanmiddleditch/nanofmt/issues/49
    CHECK(format_length("{0} = 0x{0:X} = 0b{0:b}", 28) == 19);
}

// TEST_CASE("nanofmt.format.compile_error") {
//    using namespace NANOFMT_NS::test;
//
//    CHECK(sformat("{}", unknown{}) == "");
//}
