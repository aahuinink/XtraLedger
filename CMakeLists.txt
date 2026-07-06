cmake_minimum_required(VERSION 3.16)
project(XtraLedger)

if(NOT CMAKE_BUILD_TYPE)
    set(default_build_type "Debug")
    message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
    set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Choose the build type." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "XtraLedger Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for xtraLedger
set(xtraLedger_Version_Major 0)
set(xtraLedger_Version_Minor 1)
set(xtraLedger_Version_Patch 0)
set(xtraLedger_Version_Status "-dev")
set(PROJECT_VERSION
    "${xtraLedger_Version_Major}.${xtraLedger_Version_Minor}.${xtraLedger_Version_Patch}${xtraLedger_Version_Status}"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wconversion)

set(XLCORE_LIB_NAME xlcore)

add_executable(
    XtraLedger
)

target_sources(XtraLedger
    PRIVATE
    ${CMAKE_SOURCE_DIR}/app/repl/main.c
)

target_link_libraries(XtraLedger PRIVATE ${XLCORE_LIB_NAME})

add_subdirectory(src)

add_subdirectory(tests)
