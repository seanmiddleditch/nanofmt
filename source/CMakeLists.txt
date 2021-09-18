include(fetch_dragonbox)

target_sources(nanofmt PRIVATE
    "formatters.cpp"
    "numeric_utils.h"
    "parse_utils.h"
    "to_chars.cpp"
    "vformat.cpp"
)

target_link_libraries(nanofmt PRIVATE dragonbox::dragonbox_to_chars)
