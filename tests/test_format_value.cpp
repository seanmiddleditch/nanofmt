// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "nanofmt/format.h"

#include <catch2/catch_test_macros.hpp>
#include <cstring>

TEST_CASE("nanofmt.format_value", "[nanofmt][format_value]") {
    using namespace NANOFMT_NS;

    SECTION("format_value_to") {
        char buffer[32];
        char* const end = format_value_to(buffer, 9001, "09d");

        CHECK((end - buffer) == 9);
        CHECK(std::strcmp(buffer, "000009001") == 0);
    }

    SECTION("format_value_to_n") {
        char buffer[32];

        char* const end = format_value_to_n(buffer, sizeof buffer, 9001, "09d");

        CHECK(std::strncmp(buffer, "000009001", end - buffer) == 0);
    }

    SECTION("format_value_to overflow") {
        char buffer[4];
        std::memset(buffer, 0xfe, sizeof buffer);

        char* const end = format_value_to(buffer, 9001, "09d");
        REQUIRE(*end == '\0');
        CHECK(std::strcmp(buffer, "000") == 0);
    }

    SECTION("format_value_to_n overflow") {
        char buffer[4];
        char* const end = format_value_to_n(buffer, sizeof buffer, 9001, "09d");

        CHECK((end - buffer) == 4);
        CHECK(std::strncmp(buffer, "0000", sizeof buffer) == 0);
    }

    SECTION("format_value_size") {
        std::size_t const length = format_value_length(9001, "+7d");

        CHECK(length == 7);
    }
}
