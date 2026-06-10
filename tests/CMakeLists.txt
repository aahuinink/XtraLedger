enable_testing()
include(CTest)

set(INTERNAL_TEST_SOURCES
    internal/lookup-table.c
    internal/arena.c
)

set(CTEST_OUTPUT_ON_FAILURE ON)
add_executable(TestInternals
    internal/main.c)

add_library(InternalTests
    STATIC
    ${INTERNAL_TEST_SOURCES}
)

target_link_libraries( TestInternals PRIVATE ${XLCORE_LIB_NAME} InternalTests)

target_include_directories(InternalTests PRIVATE ${PROJECT_SOURCE_DIR}/src/xlcore/memory)
target_link_libraries(InternalTests PRIVATE ${XLCORE_LIB_NAME})

add_test(NAME test_internals COMMAND TestInternals)
