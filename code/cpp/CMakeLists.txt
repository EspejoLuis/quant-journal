# Specify the minimum version of CMake required to process this file
cmake_minimum_required(VERSION 3.10)

# Set the name of the project
project(quantJournal)
include(CTest)

# Set the C++ standard to use (C++17 here)
set(CMAKE_CXX_STANDARD 23)

find_package(Catch2 REQUIRED)

# List the .cpp files to compile. Each becomes a .o object file; all .o files
# are archived together into the static library libquantJournal.a below.
set(SOURCES
    src/common/mathFunctions.hpp
    src/engine/monteCarloEngine.cpp
    src/engine/blackScholesCloseForm.cpp
    src/product/vanillaEuropeanOption.cpp
    src/product/digitalEuropeanOption.cpp
)

# Compile SOURCES into a static library: libquantJournal.a
# All production code lives here — test executables link against this archive.
add_library(quantJournal ${SOURCES})

# Tell CMake which folders to search when resolving #include directives for
# quantJournal and any target that links it (PUBLIC propagates to dependents).
# Without this, #include "blackScholesEngine.h" would fail to resolve.
target_include_directories(quantJournal PUBLIC src/engine src/product src/common)

# --- Unit test executables ---
# Each add_executable compiles one test .cpp into a standalone binary (.out /
# no extension on macOS). Catch2WithMain supplies the main() entry point so the
# test file only needs TEST_CASE blocks.
add_executable(monteCarloEngineUnit    tests/unit/engine/monteCarloEngine.cpp)
add_executable(blackScholesCloseFormUnit  tests/unit/engine/blackScholesCloseForm.cpp)
add_executable(vanillaEuropeanOptionUnit tests/unit/product/vanillaEuropeanOption.cpp)
add_executable(digitalEuropeanOptionUnit tests/unit/product/digitalEuropeanOption.cpp)
add_executable(mathFunctionsUnit tests/unit/common/mathFunctions.cpp)

# Add tests/unit to each executable's include path (PRIVATE = only this target
# sees it). Needed for any shared test helpers living under tests/unit/.
# The production headers (src/engine, src/product, src/common) are inherited
# automatically from the quantJournal library via PUBLIC above.
target_include_directories(monteCarloEngineUnit    PRIVATE tests/unit)
target_include_directories(blackScholesCloseFormUnit  PRIVATE tests/unit)
target_include_directories(vanillaEuropeanOptionUnit PRIVATE tests/unit)
target_include_directories(digitalEuropeanOptionUnit PRIVATE tests/unit)
target_include_directories(mathFunctionsUnit PRIVATE tests/unit)


# Link each test binary against:
#   quantJournal         — the static lib (.a) with all production .o files
#   Catch2::Catch2WithMain — Catch2's static lib that provides main()
# The linker resolves every symbol used in the test .o against these archives.
target_link_libraries(monteCarloEngineUnit    quantJournal Catch2::Catch2WithMain)
target_link_libraries(blackScholesCloseFormUnit  quantJournal Catch2::Catch2WithMain)
target_link_libraries(vanillaEuropeanOptionUnit quantJournal Catch2::Catch2WithMain)
target_link_libraries(digitalEuropeanOptionUnit quantJournal Catch2::Catch2WithMain)
target_link_libraries(mathFunctionsUnit quantJournal Catch2::Catch2WithMain)


# Register each binary with CTest so `ctest` (or the VS Code TestMate runner)
# knows what command to run. NAME is the label shown in test output; COMMAND is
# the binary to execute.
add_test(NAME monteCarloEngineUnit    COMMAND monteCarloEngineUnit)
add_test(NAME blackScholesCloseFormUnit  COMMAND blackScholesCloseFormUnit)
add_test(NAME vanillaEuropeanOptionUnit COMMAND vanillaEuropeanOptionUnit)
add_test(NAME digitalEuropeanOptionUnit COMMAND digitalEuropeanOptionUnit)
add_test(NAME mathFunctionsUnit COMMAND mathFunctionsUnit)



# ------------------------------------------------------------------------------------
# Only apply this configuration if the build type is explicitly set to "Coverage"
# Usage:
#   cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Coverage
# ------------------------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Coverage")
    message(STATUS "Building with LLVM coverage instrumentation")

    # Apply to everything in this build (belt-and-braces)
    add_compile_options(-fprofile-instr-generate -fcoverage-mapping -O0 -g)
    add_link_options(-fprofile-instr-generate)

    # --- 1) Add compiler flags for coverage instrumentation ---
    # -fprofile-instr-generate : instruments code to generate profiling counters (.profraw file)
    # -fcoverage-mapping       : generates mapping info so llvm-cov can map counters to source lines
    # -O0                      : no optimisation — every line compiles exactly as written so LLVM
    #                            can map counters to exact source lines. Higher levels (-O1/-O2)
    #                            eliminate dead branches (e.g. unreachable switch defaults) which
    #                            improves branch coverage numbers but corrupts line-level mapping.
    # -g                       : include debug symbols for better coverage reports
    target_compile_options(quantJournal           PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)
    target_compile_options(monteCarloEngineUnit        PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)
    target_compile_options(vanillaEuropeanOptionUnit        PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)
    target_compile_options(digitalEuropeanOptionUnit        PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)
    target_compile_options(blackScholesCloseFormUnit        PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)
    target_compile_options(mathFunctionsUnit        PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)

    # --- 2) Add linker flags for coverage ---
    target_link_options(quantJournal           PRIVATE -fprofile-instr-generate)
    target_link_options(monteCarloEngineUnit PRIVATE -fprofile-instr-generate)
    target_link_options(vanillaEuropeanOptionUnit PRIVATE -fprofile-instr-generate)
    target_link_options(digitalEuropeanOptionUnit PRIVATE -fprofile-instr-generate)
    target_link_options(blackScholesCloseFormUnit PRIVATE -fprofile-instr-generate)
    target_link_options(mathFunctionsUnit PRIVATE -fprofile-instr-generate)

    # Paths/patterns to ignore from reports (tests + Catch header)
    set(ignore_regex_tests ".*/tests/")
    set(ignore_regex_catch "catch\\.hpp$")

    # --- 3) Define a custom target "coverage" to:
    #   a) Build the tests (unit + integration)
    #   b) Run them with LLVM_PROFILE_FILE set so output goes to separate .profraw files
    #   c) Merge the raw profiling data into a single .profdata
    #   d) Generate a terminal coverage report (for both binaries)
    #   e) Generate an HTML coverage report in coverage_html/ (for both binaries)
    add_custom_target(
        coverage
        # Start clean: remove previous coverage artifacts so paths map cleanly
        COMMAND ${CMAKE_COMMAND} -E remove -f 
        monteCarloEngine.profraw 
        vanillaEuropeanOption.profraw
        digitalEuropeanOption.profraw 
        blackScholesCloseForm.profraw
        mathFunctions.profraw
        merged.profdata
        COMMAND ${CMAKE_COMMAND} -E remove_directory coverage_html

        # Run UNIT tests -> unit.profraw
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=monteCarloEngine.profraw
                $<TARGET_FILE:monteCarloEngineUnit>
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=vanillaEuropeanOption.profraw
                $<TARGET_FILE:vanillaEuropeanOptionUnit>
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=digitalEuropeanOption.profraw
                $<TARGET_FILE:digitalEuropeanOptionUnit>
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=blackScholesCloseForm.profraw
                $<TARGET_FILE:blackScholesCloseFormUnit>
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=mathFunctions.profraw
                $<TARGET_FILE:mathFunctionsUnit>

        # Merge profiles
        COMMAND xcrun llvm-profdata merge -sparse 
        monteCarloEngine.profraw 
        vanillaEuropeanOption.profraw
        digitalEuropeanOption.profraw
        blackScholesCloseForm.profraw
        mathFunctions.profraw
        -o merged.profdata

        # Generate a terminal coverage summary report for BOTH binaries
        COMMAND xcrun llvm-cov report
                $<TARGET_FILE:monteCarloEngineUnit>
                -object=$<TARGET_FILE:vanillaEuropeanOptionUnit>
                -object=$<TARGET_FILE:digitalEuropeanOptionUnit>
                -object=$<TARGET_FILE:blackScholesCloseFormUnit>
                -object=$<TARGET_FILE:mathFunctionsUnit>
                -instr-profile=merged.profdata
                -ignore-filename-regex=${ignore_regex_tests}
                -ignore-filename-regex=${ignore_regex_catch}
                -path-equivalence=${CMAKE_SOURCE_DIR},.

        # Create the HTML output directory
        COMMAND ${CMAKE_COMMAND} -E make_directory coverage_html

        # Generate a detailed HTML coverage report for BOTH executables
        # NOTE: we chdir to SOURCE dir so 'src/…' paths resolve and files render.
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_SOURCE_DIR} xcrun llvm-cov show
                $<TARGET_FILE:monteCarloEngineUnit>
                -object=$<TARGET_FILE:vanillaEuropeanOptionUnit>
                -object=$<TARGET_FILE:digitalEuropeanOptionUnit>
                -object=$<TARGET_FILE:blackScholesCloseFormUnit>
                -object=$<TARGET_FILE:mathFunctionsUnit>
                -instr-profile=${CMAKE_BINARY_DIR}/merged.profdata
                -format=html
                -output-dir=${CMAKE_BINARY_DIR}/coverage_html
                -ignore-filename-regex=${ignore_regex_tests}
                -ignore-filename-regex=${ignore_regex_catch}
                --show-line-counts-or-regions
                --show-branches=count
                --show-expansions
                -path-equivalence=${CMAKE_SOURCE_DIR},.
                ${CMAKE_SOURCE_DIR}/src

        # Optional (macOS): auto-open the report
        COMMAND open coverage_html/index.html

        # Run everything from the build directory so output paths are clean
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

        # Ensure BOTH tests are built before running coverage
        DEPENDS monteCarloEngineUnit 
                vanillaEuropeanOptionUnit 
                digitalEuropeanOptionUnit 
                blackScholesCloseFormUnit 
                mathFunctionsUnit

        COMMENT "Running unit + integration with LLVM coverage and generating merged terminal + HTML reports (with branch details)"
    )
endif()
