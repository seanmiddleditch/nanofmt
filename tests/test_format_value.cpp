// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "nanofmt/format.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>

TEST_CASE("nanofmt.format_value", "[nanofmt][format_value]") {
    using namespace NANOFMT_NS;

    SECTION("format_value_to") {
        char buf[32];
        char* const end = format_value_to(buf, 9001, "09d");

        CHECK((end - buf) == 9);
        CHECK(std::strcmp(buf, "000009001") == 0);
    }

    SECTION("format_value_to_n") {
        char buf[32];

        char* const end = format_value_to_n(buf, sizeof buf, 9001, "09d");

        CHECK(std::strncmp(buf, "000009001", end - buf) == 0);
    }

    SECTION("format_value_to overflow") {
        char buf[4];
        std::memset(buf, 0xfe, sizeof buf);

        char* const end = format_value_to(buf, 9001, "09d");
        REQUIRE(*end == '\0');
        CHECK(std::strcmp(buf, "000") == 0);
    }

    SECTION("format_value_to_n overflow") {
        char buf[4];
        char* const end = format_value_to_n(buf, sizeof buf, 9001, "09d");

        CHECK((end - buf) == 4);
        CHECK(std::strncmp(buf, "0000", sizeof buf) == 0);
    }

    SECTION("format_value_size") {
        std::size_t const length = format_value_length(9001, "+7d");

        CHECK(length == 7);
    }
}
