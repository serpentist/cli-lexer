include_directories(${CMAKE_SOURCE_DIR}/include)
add_executable(nodectrl nodectrl.cpp)
target_link_libraries(nodectrl PRIVATE gnu-lexer)
