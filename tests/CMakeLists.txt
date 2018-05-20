# get the test engine (Catch)
file(DOWNLOAD
        https://github.com/catchorg/Catch2/releases/download/v1.12.2/catch.hpp
        ${CMAKE_BINARY_DIR}/3rd-party/catch/catch.hpp)

file(GLOB TEST_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cpp)

# Make test executable
add_executable(run_tests ${TEST_SOURCES})
target_link_libraries(run_tests cwap globpp)
target_compile_features(run_tests PRIVATE cxx_range_for cxx_auto_type)

target_include_directories(run_tests
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/cpp
                ${CMAKE_BINARY_DIR}/3rd-party/catch)


# the ctest thing is far too complicated...
add_custom_command(
        TARGET run_tests
        POST_BUILD
        COMMAND $<TARGET_FILE:run_tests>)

