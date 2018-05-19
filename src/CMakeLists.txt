# find source files
file(GLOB_RECURSE CWAP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

execute_process(COMMAND llvm-config --cxxflags
        OUTPUT_VARIABLE LLVM_COMPILE_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --ldflags
        OUTPUT_VARIABLE LLVM_LINK_FLAGS
        OUTPUT_STRIP_TRAILING_WHITESPACE)

add_library(cwap STATIC ${CWAP_SOURCES})
target_compile_features(cwap PRIVATE cxx_range_for cxx_auto_type)
target_include_directories(cwap PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cwap PUBLIC globpp ${LLVM_LINK_FLAGS} clang)
target_compile_options(cwap PUBLIC ${LLVM_COMPILE_FLAGS})

