include(FetchContent)

FetchContent_Declare(
    doctest
    SYSTEM
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.4.11
)

FetchContent_MakeAvailable(doctest)
