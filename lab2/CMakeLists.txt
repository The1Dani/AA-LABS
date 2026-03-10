set (
    EXEC_NAME
    lab2_main
)

set (SOURCE_FILES
    main.c
    sortings.c
    utils.c
    smoothsort.c
)

add_executable(${EXEC_NAME} ${SOURCE_FILES})
set_property(TARGET ${EXEC_NAME} PROPERTY C_STANDARD 23)

target_include_directories(${EXEC_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# TARGET_COMPILE_OPTIONS(${EXEC_NAME} PRIVATE -ggdb)
TARGET_COMPILE_OPTIONS(${EXEC_NAME} PRIVATE -O3)
target_compile_features(${EXEC_NAME} PRIVATE)

target_link_libraries(${EXEC_NAME} PRIVATE srt)