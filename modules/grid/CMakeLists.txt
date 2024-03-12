set(SOURCE_FILES src/grid.cpp)

add_library(grid STATIC ${SOURCE_FILES})
target_link_libraries(grid flecs::flecs_static sfml-graphics)

target_include_directories(grid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../transform/include)


