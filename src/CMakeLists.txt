add_compile_options(-Wall -Wextra -Wpedantic
  -fdiagnostics-color=always
)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
  message(STATUS "Setting optimization to level 3")
  add_compile_options(-O3)
else()
  message(STATUS "Setting optimization to level 0")
  message(STATUS "Enabling debugging symbols")
  add_compile_options(-O0 -g)
endif()

add_library(gnu-lexer lexer.cpp)
target_include_directories(gnu-lexer PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
add_subdirectory(test)

install(TARGETS gnu-lexer DESTINATION lib)
