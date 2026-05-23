add_library(
    ${STORAGE_LIB_NAME}
    STATIC
    plugins/sqlite.c
)

target_include_directories(
    ${STORAGE_LIB_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
