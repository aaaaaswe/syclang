# Runtime library CMakeLists.txt

# Standard library for SysLang
add_library(syclang_rt STATIC
    stdlib.c
)

target_include_directories(syclang_rt PUBLIC ${CMAKE_SOURCE_DIR}/lib/include)
