include(fetch_catch2)

add_executable(nanofmt_test)
target_sources(nanofmt_test PRIVATE
    "test_charconv.cpp"
    "test_format.cpp"
    "test_format_args.cpp"
    "test_utils.h"
 "fwd_only_type.h")
target_link_libraries(nanofmt_test PRIVATE
    nanofmt
    Catch2::Catch2WithMain
)

add_test(NAME nanofmt_test COMMAND nanofmt_test)
