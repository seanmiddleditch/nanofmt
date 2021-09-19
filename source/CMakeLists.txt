include(fetch_dragonbox)

target_sources(nanofmt PRIVATE
    "format.cpp"
    "numeric_utils.h"
    "parse_utils.h"
    "to_chars.cpp"
)

target_link_libraries(nanofmt PRIVATE dragonbox::dragonbox)
