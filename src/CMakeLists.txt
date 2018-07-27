# find source files
file(GLOB_RECURSE CWAP_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(LLVM)
if (NOT DEFINED LLVM_INCLUDE_DIRS)
        message(WARNING "Missing LLVM_INCLUDE_DIRS, will likely fail")
endif()
if (NOT DEFINED LLVM_LIBRARY_DIRS)
        message(WARNING "Missing LLVM_LIBRARY_DIRS, will likely fail")
endif()

add_library(cwap STATIC ${CWAP_SOURCES})

target_compile_features(cwap
        PRIVATE cxx_range_for cxx_auto_type)

target_include_directories(cwap
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
               ${LLVM_INCLUDE_DIRS})

target_link_libraries(cwap
        PUBLIC globpp
               yaml-cpp
               "-L${LLVM_LIBRARY_DIRS}" clang)

file(COPY ${CWAP_RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

message("Found these resources ${CWAP_RESOURCES}")
