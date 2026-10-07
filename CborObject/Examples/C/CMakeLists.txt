cmake_minimum_required(VERSION 3.10)
project(CborExamples_C C)

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../cbor)

# CBOR核心源文件
set(CBOR_SOURCES
    ../cbor/cbor.c
    ../cbor/json.c
    ../cbor/pointer.c
)

# 禁用警告（Windows MSVC）
if(MSVC)
    add_definitions(/D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/utf-8)
endif()

# 编译所有示例
add_executable(01_basic 01_basic.c ${CBOR_SOURCES})
add_executable(02_json_pointer 02_json_pointer.c ${CBOR_SOURCES})
add_executable(03_container 03_container.c ${CBOR_SOURCES})
add_executable(04_cbor_binary 04_cbor_binary.c ${CBOR_SOURCES})

# 安装（可选）
install(TARGETS 01_basic 02_json_pointer 03_container 04_cbor_binary
        RUNTIME DESTINATION bin)

# 提示信息
message(STATUS "C Examples configured successfully")
message(STATUS "Build with: cmake --build .")
message(STATUS "Run examples:")
message(STATUS "  ./01_basic")
message(STATUS "  ./02_json_pointer")
message(STATUS "  ./03_container")
message(STATUS "  ./04_cbor_binary")
