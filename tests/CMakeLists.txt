enable_testing()
include(CTest)

set(CTEST_OUTPUT_ON_FAILURE ON)
add_executable(TestInternalsAcctArena
    internal/arena_tests.c)

target_link_libraries( TestInternalsAcctArena PRIVATE ${XLCORE_LIB_NAME})
target_include_directories(TestInternalsAcctArena PRIVATE ${PROJECT_SOURCE_DIR}/src/xlcore/memory)

add_test(NAME test_acct_arena COMMAND TestInternalsAcctArena)
