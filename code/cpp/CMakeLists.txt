# Specify the minimum version of CMake required to process this file
cmake_minimum_required(VERSION 3.10)

# Set the name of the project
project(quantJournal)
include(CTest)

# Set the C++ standard to use (C++17 here)
set(CMAKE_CXX_STANDARD 20)

find_package(Catch2 REQUIRED)
# Define a library target (.a) named 'quantJournal' made from cpp file
# This will compile src/monteCarloEngine.cpp into a reusable static library
set(SOURCES
    src/engine/monteCarloEngine.cpp
    src/product/vanillaEuropeanOption.cpp
)
add_library(quantJournal ${SOURCES})
# Specify the include directory for the quantJournal library
# Any target that links to quantJournal will be able to find headers here.
target_include_directories(quantJournal PUBLIC src/engine src/product src/common)

##### Unit Test EXECUTABLE #####
# Define an executable target named 'monteCarloEngineUnit'
# This is your Catch2 test runner
add_executable(monteCarloEngineUnit tests/unit/engine/monteCarloEngine.cpp)
add_executable(vanillaEuropeanOptionUnit tests/unit/product/vanillaEuropeanOption.cpp)

# Include directories needed to compile 'monteCarloEngineUnit'
target_include_directories(monteCarloEngineUnit PRIVATE tests/unit)
target_include_directories(vanillaEuropeanOptionUnit PRIVATE tests/unit)

# Link the quantJournal library into the 'monteCarloEngineUnit' executable
target_link_libraries(monteCarloEngineUnit quantJournal Catch2::Catch2WithMain)
target_link_libraries(vanillaEuropeanOptionUnit quantJournal Catch2::Catch2WithMain)

add_test(NAME monteCarloEngineUnit COMMAND monteCarloEngineUnit)
add_test(NAME vanillaEuropeanOptionUnit COMMAND vanillaEuropeanOptionUnit)



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
    # -O0                      : disable optimisations (ensures accurate mapping)
    # -g                       : include debug symbols for better coverage reports
    target_compile_options(quantJournal           PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)
    target_compile_options(monteCarloEngineUnit        PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)
    target_compile_options(vanillaEuropeanOptionUnit        PRIVATE -fprofile-instr-generate -fcoverage-mapping -O0 -g)

    # --- 2) Add linker flags for coverage ---
    target_link_options(quantJournal           PRIVATE -fprofile-instr-generate)
    target_link_options(monteCarloEngineUnit PRIVATE -fprofile-instr-generate)
    target_link_options(vanillaEuropeanOptionUnit PRIVATE -fprofile-instr-generate)

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
        COMMAND ${CMAKE_COMMAND} -E remove -f monteCarloEngine.profraw vanillaEuropeanOption.profraw merged.profdata
        COMMAND ${CMAKE_COMMAND} -E remove_directory coverage_html

        # Run UNIT tests -> unit.profraw
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=monteCarloEngine.profraw
                $<TARGET_FILE:monteCarloEngineUnit>
        COMMAND ${CMAKE_COMMAND} -E env LLVM_PROFILE_FILE=vanillaEuropeanOption.profraw
                $<TARGET_FILE:vanillaEuropeanOptionUnit>

        # Merge profiles
        COMMAND xcrun llvm-profdata merge -sparse monteCarloEngine.profraw vanillaEuropeanOption.profraw -o merged.profdata

        # Generate a terminal coverage summary report for BOTH binaries
        COMMAND xcrun llvm-cov report
                $<TARGET_FILE:monteCarloEngineUnit>
                -object=$<TARGET_FILE:vanillaEuropeanOptionUnit>
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
        DEPENDS monteCarloEngineUnit vanillaEuropeanOptionUnit

        COMMENT "Running unit + integration with LLVM coverage and generating merged terminal + HTML reports (with branch details)"
    )
endif()
