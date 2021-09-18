// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "test_utils.h"
#include "nanofmt/format.h"
#include "nanofmt/std_string.h"

#include <catch2/catch_test_macros.hpp>
#include <limits>

enum class standard_enum { one, two };
enum class custom_enum { foo, bar };

class custom_type {};

namespace NANOFMT_NS {
    template <>
    struct formatter<custom_enum> : formatter<char const*> {
        void format(custom_enum value, buffer& buf) {
            switch (value) {
                case custom_enum::foo:
                    formatter<char const*>::format("foo", buf);
                    break;
                case custom_enum::bar:
                    formatter<char const*>::format("bar", buf);
                    break;
            }
        }
    };

    template <>
    struct formatter<custom_type> : formatter<char const*> {
        void format(custom_type, buffer& buf) {
            formatter<char const*>::format("custom", buf);
        }
    };
} // namespace NANOFMT_NS

TEST_CASE("nanofmt.format.integers", "[nanofmt][format][integers]") {
    using namespace NANOFMT_NS::test;

    SECTION("width and fill") {
        CHECK(sformat("{:6d}", 1234) == "  1234");
        CHECK(sformat("{:6d}", -1234) == " -1234");
    }

    SECTION("zero pad") {
        CHECK(sformat("{:06}", 1234) == "001234");
        CHECK(sformat("{:06}", -1234) == "-01234");
    }

    SECTION("precision") {
        // BUG? -- fmtlib/std::format doesn't support precision for integral types, should we nuke this?
        CHECK(sformat("{:.4}", 123456) == "1234");
        CHECK(sformat("{:.4b}", 0b1001'0110) == "1001");
    }

    SECTION("hex") {
        CHECK(sformat("{:x}", ~16u) == "ffffffef");
        CHECK(sformat("{:08x}", 0xDEAD) == "0000dead");
        CHECK(sformat("{:08X}", 0xDEAD) == "0000DEAD");
    }

    SECTION("binary") {
        CHECK(sformat("{:b}", 0) == "0");
        CHECK(sformat("{:b}", 0b11011) == "11011");
        CHECK(sformat("{:b}", 5) == "101");
        CHECK(sformat("{:b}", -256) == "-100000000");
    }
}

TEST_CASE("nanofmt.format.floating", "[nanofmt][format][floating]") {
    using namespace NANOFMT_NS::test;

    SECTION("precision") {
        CHECK(sformat("{:.1f}", 1.55) == "1.6");
        CHECK(sformat("{:.1e}", 1.45) == "1.4e+00");
        CHECK(sformat("{}", std::numeric_limits<float>::max()) == "3.40282e+38");
    }

    SECTION("signs") {
        CHECK(sformat("{:+.3}", 1.0) == "+1");
        CHECK(sformat("{:+.3}", -1.0) == "-1");
        CHECK(sformat("{:-.3}", 1.0) == "1");
        CHECK(sformat("{:-.3}", -1.0) == "-1");
        CHECK(sformat("{: .3}", 1.0) == " 1");
        CHECK(sformat("{: .3}", -1.0) == "-1");
    }

    SECTION("nonfinite") {
        CHECK(sformat("{:f}", std::numeric_limits<float>::infinity()) == "inf");
        CHECK(sformat("{:f}", -std::numeric_limits<float>::infinity()) == "-inf");

        CHECK(sformat("{:f}", std::numeric_limits<float>::quiet_NaN()) == "nan");
        CHECK(sformat("{:f}", -std::numeric_limits<float>::quiet_NaN()) == "-nan");
    }

    SECTION("casing") {
        CHECK(sformat("{:.2E}", -12.99) == "-1.30E+01");
        CHECK(sformat("{:E}", -std::numeric_limits<float>::infinity()) == "-INF");

        CHECK(sformat("{:G}", -.000'123) == "-0.000123");
        CHECK(sformat("{:G}", std::numeric_limits<float>::quiet_NaN()) == "NAN");
    }
}

TEST_CASE("nanofmt.format.strings", "[nanofmt][format][strings]") {
    using namespace NANOFMT_NS::test;

    SECTION("char arrays") {
        char const s[] = "array";

        CHECK(sformat("{}", "test") == "test");
        CHECK(sformat("{}", s) == "array");
    }

    SECTION("stdlib") {
        CHECK(sformat("{}", std::string("test")) == "test");
        CHECK(sformat("{}", std::string_view("test")) == "test");

        CHECK(sformat("{}{}{}", std::string_view("ab"), std::string("cd"), "ef") == "abcdef");

        CHECK(sformat(std::string("a{}c"), "b") == "abc");
    }

    SECTION("width and fill") {
        CHECK(sformat("{:<8}{:05}", "value", 42) == "value   00042");
    }
}

TEST_CASE("nanofmt.format.bools", "[nanofmt][format][bools]") {
    using namespace NANOFMT_NS::test;

    SECTION("default") {
        CHECK(sformat("{}", true) == "true");
        CHECK(sformat("{}", false) == "false");
    }

    SECTION("integer") {
        CHECK(sformat("{:d}", true) == "1");
        CHECK(sformat("{:d}", false) == "0");
    }
}

TEST_CASE("nanofmt.format.pointers", "[nanofmt][format][pointers]") {
    using namespace NANOFMT_NS::test;

    SECTION("nullptr") {
        if constexpr (sizeof(void*) == 8) {
            CHECK(sformat("{}", nullptr) == "0x0000000000000000");
        }
        else {
            CHECK(sformat("{}", nullptr) == "0x00000000");
        }
    }

    SECTION("raw") {
        void const* ptr = reinterpret_cast<void const*>(static_cast<std::uintptr_t>(0xDEADC0DE));
        int const* iptr = reinterpret_cast<int const*>(static_cast<std::uintptr_t>(0xFEFEFEFE));

        if constexpr (sizeof(void*) == 8) {
            CHECK(sformat("{}", ptr) == "0x00000000deadc0de");
            CHECK(sformat("{:X}", iptr) == "0X00000000FEFEFEFE");
        }
        else {
            CHECK(sformat("{}", ptr) == "0xdeadc0de");
            CHECK(sformat("{:X}", iptr) == "0XFEFEFEFE");
        }
    }
}

TEST_CASE("nanofmt.format.enums", "[nanofmt][format][enums]") {
    using namespace NANOFMT_NS::test;

    SECTION("standard") {
        CHECK(sformat("{0}", standard_enum::two) == "1");
    }

    SECTION("custom") {
        CHECK(sformat("{}", custom_enum::bar) == "bar");
    }
}

TEST_CASE("nanofmt.format.custom", "[nanofmt][format][custom]") {
    using namespace NANOFMT_NS::test;

    custom_type local;
    custom_type& ref = local;

    CHECK(sformat("{}", custom_type{}) == "custom");
    CHECK(sformat("{}", local) == "custom");
    CHECK(sformat("{}", ref) == "custom");
}

// SECTION("errors") {
//    char buffer[256];

//    CHECK(sformat_to(buffer, "{} {:4d} {:3.5f}", "abc", 9, 12.57) == NANOFMT_NS::format_result::success);
//    CHECK(sformat_to(buffer, "{} {:4d", "abc", 9) == NANOFMT_NS::format_result::malformed_input);
//    CHECK(sformat_to(buffer, "{0} {1}", "abc", 9) == NANOFMT_NS::format_result::success);
//    CHECK(sformat_to(buffer, "{0} {1} {5}", "abc", 9, 12.57) == NANOFMT_NS::format_result::out_of_range);
//}
