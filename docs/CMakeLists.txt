find_program(NANOFMT_SPHINX_PATH "sphinx-build" REQUIRED DOC "Path to sphinx-build")

add_custom_target(nanofmt_docs
    COMMAND ${NANOFMT_SPHINX_PATH} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    USES_TERMINAL
    SOURCES
        conf.py
        api.rst
        benchmarks.rst
        design.rst
        faq.rst
        index.rst
        license.rst
        requirements.txt
)
