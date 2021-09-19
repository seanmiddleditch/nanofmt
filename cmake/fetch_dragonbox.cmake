include(FetchContent)

FetchContent_Declare(
    nanofmt_dragonbox
    GIT_REPOSITORY https://github.com/jk-jeon/dragonbox
    GIT_TAG        1.0.1
)

FetchContent_Populate(nanofmt_dragonbox)

add_library(nanofmt_dragonbox INTERFACE)
target_include_directories(nanofmt_dragonbox INTERFACE ${nanofmt_dragonbox_SOURCE_DIR}/include)
