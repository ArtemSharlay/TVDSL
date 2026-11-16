# CMakeList.txt : CMake project for dz2, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.14)   

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("dz2")


include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)


set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


add_executable (dz2 "dz2.cpp" "dz2.h" "src/parser/parser.cpp" "src/dfa/dfa.cpp")

target_include_directories(dz2 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_compile_definitions(dz2 PRIVATE
    CONFIG_PATH="${CMAKE_SOURCE_DIR}"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dz2 PROPERTY CXX_STANDARD 20)
endif()

# ---- Тесты ----
enable_testing()

add_executable(parser_tests "test/parserTest.cpp" "src/parser/parser.cpp" "src/dfa/dfa.cpp")

target_include_directories(parser_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(parser_tests PRIVATE
    CONFIG_PATH="${CMAKE_SOURCE_DIR}"
)

set_property(TARGET parser_tests PROPERTY CXX_STANDARD 20)


target_link_libraries(parser_tests PRIVATE GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(parser_tests)

add_test(NAME ParserTests COMMAND parser_tests)