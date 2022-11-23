# examples: code usage examples of libgit2

# 设置输出目录为./bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
# Ensure that we do not use deprecated functions internally
add_definitions(-DGIT_DEPRECATE_HARD)

#file(GLOB SRC_GET_NEW_COMMITS *.c *.h)

# 遍历项目根目录下所有的 .c 文件
file (GLOB files *.c *.cpp)
foreach (file ${files})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" exe ${file})
    add_executable(${exe} ${file})
#    set_target_properties(${exe} PROPERTIES C_STANDARD 90)

    target_include_directories(${exe} PRIVATE ${LIBGIT2_INCLUDES} ${LIBGIT2_DEPENDENCY_INCLUDES})
    target_include_directories(${exe} SYSTEM PRIVATE ${LIBGIT2_SYSTEM_INCLUDES})
    if(WIN32 OR ANDROID)
        target_link_libraries(${exe} libgit2package)
    else()
        target_link_libraries(${exe} libgit2package pthread)
    endif()

endforeach ()
