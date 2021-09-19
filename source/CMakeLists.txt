include(fetch_dragonbox)

target_sources(nanofmt PRIVATE
    "format.cpp"
    "numeric_utils.h"
    "parse_utils.h"
    "to_chars.cpp"
)
target_include_directories(nanofmt PRIVATE ${nanofmt_dragonbox_SOURCE_DIR}/include)
