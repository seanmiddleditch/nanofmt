include(fetch_doctest)

add_executable(nanofmt_test)
target_sources(nanofmt_test PRIVATE
    "fwd_only_type.h"
    "test_charconv.cpp"
    "test_format.cpp"
    "test_format_args.cpp"
    "test_utils.h"
)
target_link_libraries(nanofmt_test PRIVATE
    nanofmt
    doctest::doctest_with_main
)

add_test(NAME nanofmt_test COMMAND nanofmt_test)
