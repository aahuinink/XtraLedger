set(XLCORE_SOURCES
    memory/arena.c
    errors.c
)

add_library(${XLCORE_LIB_NAME}
    STATIC
    ${XLCORE_SOURCES}
)

target_include_directories(
    ${XLCORE_LIB_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(${XLCORE_LIB_NAME} PUBLIC c_std_11)

target_link_libraries(
    ${XLCORE_LIB_NAME}
    PRIVATE
    ${STORAGE_LIB_NAME}
)
