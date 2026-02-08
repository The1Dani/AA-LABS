set (
    EXEC_NAME
    lab1_main
)

set (SOURCE_FILES
    main.c
    methods.c
)

add_executable(${EXEC_NAME} ${SOURCE_FILES})
set_property(TARGET ${EXEC_NAME} PROPERTY C_STANDARD 23)

target_include_directories(${EXEC_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

TARGET_COMPILE_OPTIONS(${EXEC_NAME} PRIVATE -ggdb)
target_compile_features(${EXEC_NAME} PRIVATE)

target_link_libraries(${EXEC_NAME} PRIVATE srt)
target_link_libraries(${EXEC_NAME} PRIVATE gmp)
target_link_libraries(${EXEC_NAME} PRIVATE m)