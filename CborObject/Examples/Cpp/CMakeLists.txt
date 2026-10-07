cmake_minimum_required(VERSION 3.10)
project(CborExamples_Cpp CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../cbor)

# CBOR核心C源文件
set(CBOR_C_SOURCES
    ../cbor/cbor.c
    ../cbor/json.c
    ../cbor/pointer.c
)

# CBOR C++封装源文件
set(CBOR_CXX_SOURCES
    ../CborValue.cpp
    ../CborObject.cpp
    ../CborArray.cpp
    ../CborDocument.cpp
)

# 禁用警告（Windows MSVC）
if(MSVC)
    add_definitions(/D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/utf-8)
endif()

# 编译示例
add_executable(01_document_basic 01_document_basic.cpp ${CBOR_C_SOURCES} ${CBOR_CXX_SOURCES})

# 安装（可选）
install(TARGETS 01_document_basic
        RUNTIME DESTINATION bin)

message(STATUS "C++ Examples configured successfully")
message(STATUS "Build with: cmake --build .")
message(STATUS "Run example: ./01_document_basic")
