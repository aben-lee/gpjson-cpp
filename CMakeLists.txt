cmake_minimum_required(VERSION 3.10)
project(gpjson_cpp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    # /utf-8: sources and execution charset are UTF-8; keep warnings non-fatal.
    add_compile_options(/utf-8 /W1 /D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall)
endif()

enable_testing()

add_subdirectory(CborObject)
add_subdirectory(GeoJson)

# ---------------------------------------------------------------------------
# Unit / smoke tests. None of these depend on Qt or on the unpublished v5
# object layer; they exercise the CBOR/JSON wrapper, the C core and geometry.
# ---------------------------------------------------------------------------
set(GPJSON_TEST_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/CborObject
    ${CMAKE_CURRENT_SOURCE_DIR}/CborObject/cbor
    ${CMAKE_CURRENT_SOURCE_DIR}/GeoJson)

add_executable(test_cborobject tests/test_cborobject.cpp)
target_include_directories(test_cborobject PRIVATE ${GPJSON_TEST_INCLUDE_DIRS})
target_link_libraries(test_cborobject PRIVATE CborObject)
add_test(NAME cborobject COMMAND $<TARGET_FILE:test_cborobject>)

add_executable(test_cbor_api_verify tests/test_cbor_api_verify.cpp)
target_include_directories(test_cbor_api_verify PRIVATE ${GPJSON_TEST_INCLUDE_DIRS})
target_link_libraries(test_cbor_api_verify PRIVATE CborObject)
add_test(NAME cbor_api_verify COMMAND $<TARGET_FILE:test_cbor_api_verify>)

add_executable(test_geojson tests/test_geojson.cpp)
target_include_directories(test_geojson PRIVATE ${GPJSON_TEST_INCLUDE_DIRS})
target_link_libraries(test_geojson PRIVATE GeoJson CborObject)
add_test(NAME geojson COMMAND $<TARGET_FILE:test_geojson>)

# The snippets under CborObject/Examples are reference code and are not part
# of the default build/test (the public C core is already compiled and tested
# via test_cbor_api_verify). The pure-C examples can be configured standalone:
#   cmake -S CborObject/Examples/C -B build-c
