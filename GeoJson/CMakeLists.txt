cmake_minimum_required(VERSION 3.10)
project(GeoJson VERSION 1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../CborObject
    ${CMAKE_CURRENT_SOURCE_DIR}/../CborObject/cbor
)

# 头文件列表
set(GEOJSON_HEADERS
    GeometryBase.h
    Point.h
    LineString.h
    Polygon.h
    MultiGeometries.h
    Geometry.h
)

# 源文件列表
set(GEOJSON_SOURCES
    Point.cpp
    LineString.cpp
    Polygon.cpp
    MultiGeometries.cpp
    Geometry.cpp
)

# 创建静态库
add_library(GeoJson STATIC ${GEOJSON_SOURCES} ${GEOJSON_HEADERS})

# 设置目标属性
target_include_directories(GeoJson PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/GeoJson>
)

# 链接CborObject库（如果已构建）
# target_link_libraries(GeoJson PUBLIC CborObject)

# 编译选项
if(MSVC)
    target_compile_options(GeoJson PRIVATE /W3)
else()
    target_compile_options(GeoJson PRIVATE -Wall -Wextra)
endif()

# 安装规则
install(TARGETS GeoJson
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${GEOJSON_HEADERS}
    DESTINATION include/GeoJson
)

# 打印配置信息
message(STATUS "GeoJson Library Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Source Directory: ${CMAKE_CURRENT_SOURCE_DIR}")
