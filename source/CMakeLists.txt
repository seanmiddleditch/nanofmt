target_sources(nanofmt PRIVATE
    "charconv.cpp"
    "format.cpp"
    "numeric_utils.h"
    "parse_utils.h"
)
target_include_directories(nanofmt SYSTEM PRIVATE ${nanofmt_dragonbox_SOURCE_DIR}/include)
