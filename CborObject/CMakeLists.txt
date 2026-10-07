cmake_minimum_required(VERSION 3.10)
project(CborObject VERSION 1.0 LANGUAGES C CXX)

# C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cbor
)

# 源文件
set(CBOROBJECT_SOURCES
    CborValue.cpp
    CborArray.cpp
    CborObject.cpp
    CborDocument.cpp
    cbor/cbor.c
    cbor/json.c
    cbor/pointer.c
)

# 头文件
set(CBOROBJECT_HEADERS
    CborValue.h
    CborArray.h
    CborObject.h
    CborDocument.h
    cbor/cbor.h
    cbor/define.h
    cbor/list.h
    cbor/fastsearch.h
)

# 创建静态库
add_library(CborObject STATIC ${CBOROBJECT_SOURCES} ${CBOROBJECT_HEADERS})

# 编译选项
target_compile_definitions(CborObject PRIVATE _CRT_SECURE_NO_WARNINGS)

# 设置输出目录
set_target_properties(CborObject PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# 安装规则
install(TARGETS CborObject
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${CBOROBJECT_HEADERS}
    DESTINATION include/CborObject
)

# 导出编译命令（用于IDE支持）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 打印配置信息
message(STATUS "CborObject Library Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
