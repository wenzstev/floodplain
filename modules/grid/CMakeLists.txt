set(SOURCE_FILES src/grid.cpp)

add_library(grid STATIC ${SOURCE_FILES})
target_link_libraries(grid flecs::flecs_static)

target_include_directories(grid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET factories PROPERTY CXX_STANDARD 20)
endif()

add_subdirectory(tests)

