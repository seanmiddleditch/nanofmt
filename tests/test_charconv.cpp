// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "test_utils.h"
#include "nanofmt/charconv.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <limits>

TEST_CASE("nanofmt.to_chars.integers", "[nanofmt][to_chars][integers]") {
    using namespace NANOFMT_NS::test;
    using namespace NANOFMT_NS;

    SECTION("decimal") {
        CHECK(to_string(0) == "0");
        CHECK(to_string(1) == "1");
        CHECK(to_string(10) == "10");
        CHECK(to_string(100) == "100");
        CHECK(to_string(1'000) == "1000");
    }

    SECTION("hex") {
        CHECK(to_string(0x0, int_format::hex) == "0");
        CHECK(to_string(0x1, int_format::hex) == "1");

        CHECK(to_string(0xdeadc0de, int_format::hex) == "deadc0de");
        CHECK(to_string(0xdeadc0de, int_format::hex_upper) == "DEADC0DE");
    }

    SECTION("binary") {
        CHECK(to_string(0b0, int_format::binary) == "0");
        CHECK(to_string(0b1, int_format::binary) == "1");

        CHECK(to_string(0b10011001, int_format::binary) == "10011001");

        CHECK(to_string(-0b01011010, int_format::binary) == "-1011010");
    }

    SECTION("negatives") {
        CHECK(to_string(-0) == "0");
        CHECK(to_string(-1) == "-1");
        CHECK(to_string(-10) == "-10");
        CHECK(to_string(-100) == "-100");
        CHECK(to_string(-1'000) == "-1000");
    }

    SECTION("bounds") {
        CHECK(to_string(std::numeric_limits<std::int8_t>::max()) == "127");
        CHECK(to_string(std::numeric_limits<std::int16_t>::max()) == "32767");
        CHECK(to_string(std::numeric_limits<std::int32_t>::max()) == "2147483647");
        CHECK(to_string(std::numeric_limits<std::int64_t>::max()) == "9223372036854775807");

        CHECK(to_string(std::numeric_limits<std::int8_t>::min()) == "-128");
        CHECK(to_string(std::numeric_limits<std::int16_t>::min()) == "-32768");
        CHECK(to_string(std::numeric_limits<std::int32_t>::min()) == "-2147483648");
        CHECK(to_string(std::numeric_limits<std::int64_t>::min()) == "-9223372036854775808");

        CHECK(to_string(std::numeric_limits<std::uint8_t>::max()) == "255");
        CHECK(to_string(std::numeric_limits<std::uint16_t>::max()) == "65535");
        CHECK(to_string(std::numeric_limits<std::uint32_t>::max()) == "4294967295");
        CHECK(to_string(std::numeric_limits<std::uint64_t>::max()) == "18446744073709551615");
    }
}

TEST_CASE("nanofmt.to_chars.fixed", "[nanofmt][to_chars][float][fixed]") {
    using namespace NANOFMT_NS::test;
    using namespace NANOFMT_NS;

    SECTION("whole numbers") {
        CHECK(to_string(0e0f, float_format::fixed) == "0");
        CHECK(to_string(1e0f, float_format::fixed) == "1");
        CHECK(to_string(1e1f, float_format::fixed) == "10");
        CHECK(to_string(1e2f, float_format::fixed) == "100");
        CHECK(to_string(1e3f, float_format::fixed) == "1000");
        CHECK(to_string(1e4f, float_format::fixed) == "10000");
    }

    SECTION("fractional numbers") {
        CHECK(to_string(1e-1f, float_format::fixed) == "0.1");
        CHECK(to_string(1e-2f, float_format::fixed) == "0.01");
        CHECK(to_string(1e-3f, float_format::fixed) == "0.001");
        CHECK(to_string(1e-4f, float_format::fixed) == "0.0001");
    }

    SECTION("bounds") {
        CHECK(
            to_string(std::numeric_limits<float>::max(), float_format::fixed) ==
            "340282350000000000000000000000000000000");
        CHECK(
            to_string(std::numeric_limits<double>::max(), float_format::fixed) ==
            "179769313486231570000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000");

        CHECK(
            to_string(std::numeric_limits<float>::min(), float_format::fixed) ==
            "0.000000000000000000000000000000000000011754944");
        CHECK(
            to_string(std::numeric_limits<double>::min(), float_format::fixed) ==
            "0.0000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00022250738585072014");
    }

    SECTION("rounding") {
        CHECK(to_string(.001, float_format::fixed, 0) == "0");
        CHECK(to_string(1.55, float_format::fixed, 0) == "2");
        CHECK(to_string(1.55, float_format::fixed, 1) == "1.6");
        CHECK(to_string(1.35, float_format::fixed, 1) == "1.4");
        CHECK(to_string(1.25, float_format::fixed, 1) == "1.2");
        CHECK(to_string(1.351, float_format::fixed, 1) == "1.4");
        CHECK(to_string(1.251, float_format::fixed, 1) == "1.3");
    }

    SECTION("nonfinite") {
        CHECK(to_string(std::numeric_limits<float>::infinity(), float_format::fixed) == "inf");
        CHECK(to_string(-std::numeric_limits<float>::infinity(), float_format::fixed) == "-inf");
        CHECK(to_string(std::numeric_limits<float>::quiet_NaN(), float_format::fixed) == "nan");
        CHECK(to_string(-std::numeric_limits<float>::quiet_NaN(), float_format::fixed) == "-nan");
    }
}

TEST_CASE("nanofmt.to_chars.scientifix", "[nanofmt][to_chars][float][scientific]") {
    using namespace NANOFMT_NS::test;
    using namespace NANOFMT_NS;

    SECTION("whole numbers") {
        CHECK(to_string(0e0f, float_format::scientific) == "0e+00");
        CHECK(to_string(1e0f, float_format::scientific) == "1e+00");
        CHECK(to_string(1e1f, float_format::scientific) == "1e+01");
        CHECK(to_string(1e2f, float_format::scientific) == "1e+02");
        CHECK(to_string(1e3f, float_format::scientific) == "1e+03");
        CHECK(to_string(1e4f, float_format::scientific) == "1e+04");
    }

    SECTION("fractional numbers") {
        CHECK(to_string(1e-1f, float_format::scientific) == "1e-01");
        CHECK(to_string(1e-2f, float_format::scientific) == "1e-02");
        CHECK(to_string(1e-3f, float_format::scientific) == "1e-03");
        CHECK(to_string(1e-4f, float_format::scientific) == "1e-04");
    }

    SECTION("mixed numbers") {
        CHECK(to_string(1.55e-1f, float_format::scientific) == "1.55e-01");
        CHECK(to_string(12.55e-2f, float_format::scientific) == "1.255e-01");
        CHECK(to_string(12'000.55'001f, float_format::scientific) == "1.200055e+04");
        CHECK(to_string(1.55e-4f, float_format::scientific) == "1.55e-04");
    }

    SECTION("bounds") {
        CHECK(to_string(std::numeric_limits<float>::max(), float_format::scientific) == "3.4028235e+38");
        CHECK(to_string(std::numeric_limits<double>::max(), float_format::scientific) == "1.7976931348623157e+308");

        CHECK(to_string(std::numeric_limits<float>::min(), float_format::scientific) == "1.1754944e-38");
        CHECK(to_string(std::numeric_limits<double>::min(), float_format::scientific) == "2.2250738585072014e-308");
    }

    SECTION("rounding") {
        CHECK(to_string(1.55e-4f, float_format::scientific, 0) == "2e-04");
        CHECK(to_string(1.35e-4f, float_format::scientific, 1) == "1.4e-04");
        CHECK(to_string(1.25e-4f, float_format::scientific, 1) == "1.2e-04");
        CHECK(to_string(1.351e-4f, float_format::scientific, 1) == "1.4e-04");
        CHECK(to_string(1.251e-4f, float_format::scientific, 1) == "1.3e-04");
    }

    SECTION("nonfinite") {
        CHECK(to_string(std::numeric_limits<float>::infinity(), float_format::scientific) == "inf");
        CHECK(to_string(-std::numeric_limits<float>::infinity(), float_format::scientific) == "-inf");
        CHECK(to_string(std::numeric_limits<float>::quiet_NaN(), float_format::scientific) == "nan");
        CHECK(to_string(-std::numeric_limits<float>::quiet_NaN(), float_format::scientific) == "-nan");
    }
}

TEST_CASE("nanofmt.to_chars.general", "[nanofmt][to_chars][float][general]") {
    using namespace NANOFMT_NS::test;
    using namespace NANOFMT_NS;

    SECTION("bounds") {
        CHECK(to_string(std::numeric_limits<float>::max(), float_format::general) == "3.40282e+38");
        CHECK(to_string(std::numeric_limits<double>::max(), float_format::general) == "1.79769e+308");

        CHECK(to_string(std::numeric_limits<float>::min(), float_format::general) == "1.17549e-38");
        CHECK(to_string(std::numeric_limits<double>::min(), float_format::general) == "2.22507e-308");
    }

    SECTION("nonfinite") {
        CHECK(to_string(std::numeric_limits<float>::infinity(), float_format::general) == "inf");
        CHECK(to_string(-std::numeric_limits<float>::infinity(), float_format::general) == "-inf");
        CHECK(to_string(std::numeric_limits<float>::quiet_NaN(), float_format::general) == "nan");
        CHECK(to_string(-std::numeric_limits<float>::quiet_NaN(), float_format::general) == "-nan");
    }
}
