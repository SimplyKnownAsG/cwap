# get globpp
foreach(file_name CMakeLists.txt Glob.cpp Glob.hpp Iterator.cpp Iterator.hpp Search.cpp Search.hpp)
        file(DOWNLOAD
                https://raw.githubusercontent.com/SimplyKnownAsG/globpp/master/src/${file_name}
                ${CMAKE_CURRENT_SOURCE_DIR}/globpp/${file_name})
endforeach(file_name)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/globpp)

