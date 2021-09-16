// Copyright (c) Sean Middleditch and contributors. See accompanying LICENSE.md for copyright details.

#include "nanofmt/format.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("nanofmt.format_value", "[nanofmt][format_value]") {
    using namespace nanofmt;

    SECTION("format_value_to") {
        char buf[32];
        char* const end = format_value_to(buf, 9001, "09d");

        CHECK((end - buf) == 9);
        CHECK(std::strcmp(buf, "000009001") == 0);
    }

    SECTION("format_value_to_n") {
        char buf[4];
        char* const end = format_value_to_n(buf, sizeof buf, 9001, "09d");

        CHECK((end - buf) == 3);
        CHECK(std::strcmp(buf, "000") == 0);
    }

    SECTION("format_value_size") {
        std::size_t const size = format_value_size(9001, "+7d");

        CHECK(size == 7);
    }
}
