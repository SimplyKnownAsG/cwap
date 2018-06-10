# get globpp

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/globpp)
        execute_process(
                COMMAND git clone https://github.com/SimplyKnownAsG/globpp.git
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/globpp/src ${CMAKE_CURRENT_BINARY_DIR}/globpp/build)


if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp)
        execute_process(
                COMMAND git clone https://github.com/jbeder/yaml-cpp.git
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif()
set(YAML_CPP_BUILD_TESTS CACHE BOOL OFF)
set(YAML_CPP_BUILD_TOOLS CACHE BOOL OFF)
set(YAML_CPP_BUILD_CONTRIB CACHE BOOL OFF)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp ${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp/build)

