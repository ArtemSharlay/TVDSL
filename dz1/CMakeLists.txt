# CMakeList.txt : CMake project for dz1, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("dz1")

# Add source to this project's executable.
add_executable (dz1 "dz1.cpp"  "src/regex/regexAST.cpp" "src/regex/regexParser.cpp" "src/nfa/nfa.cpp" "src/dfa/dfa.cpp" "src/json/jsonParser.cpp" )
target_compile_definitions(dz1 PRIVATE
    CONFIG_PATH="${CMAKE_SOURCE_DIR}"
)
target_include_directories(dz1 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_property(TARGET dz1 PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dz1 PROPERTY CXX_STANDARD 20)
endif()

enable_testing()

add_executable(lexer_tests "tests/test.cpp" "src/regex/regexAST.cpp" "src/regex/regexParser.cpp" "src/nfa/nfa.cpp" "src/dfa/dfa.cpp" "src/json/jsonParser.cpp")

target_include_directories(lexer_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(lexer_tests PRIVATE
    CONFIG_PATH="${CMAKE_SOURCE_DIR}"
)

set_property(TARGET lexer_tests PROPERTY CXX_STANDARD 20)

add_test(NAME LexerTests COMMAND lexer_tests)

