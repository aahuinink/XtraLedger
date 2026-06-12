enable_testing()
include(CTest)


set(CTEST_OUTPUT_ON_FAILURE ON)

add_executable(TestInternalsAcctSoA
    internal/arena.c)

add_executable(TestInternalsLookupTable
    internal/lookup-table.c)

target_link_libraries( TestInternalsAcctSoA PRIVATE ${XLCORE_LIB_NAME})
target_link_libraries( TestInternalsLookupTable PRIVATE ${XLCORE_LIB_NAME})

target_include_directories(TestInternalsAcctSoA PRIVATE ${PROJECT_SOURCE_DIR}/src/xlcore/memory)
target_include_directories(TestInternalsLookupTable PRIVATE ${PROJECT_SOURCE_DIR}/src/xlcore/memory)

add_test(NAME test_internals_acct_soa COMMAND TestInternalsAcctSoA)
add_test(NAME test_internals_lookup_table COMMAND TestInternalsLookupTable)
