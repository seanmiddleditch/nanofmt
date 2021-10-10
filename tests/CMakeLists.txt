include(fetch_catch2)

add_executable(nanofmt_test)
target_sources(nanofmt_test PRIVATE
    "test_charconv.cpp"
    "test_format.cpp"
    "test_format_args.cpp"
    "test_utils.h"
)
target_link_libraries(nanofmt_test PRIVATE
    nanofmt
    Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(nanofmt_test)
