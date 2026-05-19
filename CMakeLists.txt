cmake_minimum_required(VERSION 3.14)
project(bestreviewer CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    TLS_VERIFY OFF
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)
include_directories(test/support test/fake)

enable_testing()

set(GOLDEN_APPROVED_DIR ${CMAKE_SOURCE_DIR}/test/golden/approved)
set(GOLDEN_RECEIVED_DIR ${CMAKE_SOURCE_DIR}/test/golden/received)
file(MAKE_DIRECTORY ${GOLDEN_APPROVED_DIR} ${GOLDEN_RECEIVED_DIR})

add_executable(TunerTest test/TunerTest.cpp)
target_link_libraries(TunerTest GTest::gtest_main GTest::gmock)

add_executable(TVControllerTest test/TVControllerTest.cpp)
target_link_libraries(TVControllerTest GTest::gtest_main GTest::gmock)

add_executable(TVControllerGoldenTest test/TVControllerGoldenTest.cpp)
target_compile_definitions(TVControllerGoldenTest PRIVATE
    GOLDEN_FIXTURE_DIR="${GOLDEN_APPROVED_DIR}")
target_link_libraries(TVControllerGoldenTest GTest::gtest_main)

add_custom_target(update-golden
    COMMAND ${CMAKE_COMMAND} -E env UPDATE_GOLDEN=1
        $<TARGET_FILE:TVControllerGoldenTest>
    DEPENDS TVControllerGoldenTest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Regenerate test/golden/approved/*.approved.txt from current output")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(TunerTest PRIVATE --coverage)
    target_link_options(TunerTest PRIVATE --coverage)
    target_compile_options(TVControllerTest PRIVATE --coverage)
    target_link_options(TVControllerTest PRIVATE --coverage)
    target_compile_options(TVControllerGoldenTest PRIVATE --coverage)
    target_link_options(TVControllerGoldenTest PRIVATE --coverage)
endif()

include(GoogleTest)
gtest_discover_tests(TunerTest)
gtest_discover_tests(TVControllerTest)
gtest_discover_tests(TVControllerGoldenTest
    PROPERTIES LABELS "golden")
