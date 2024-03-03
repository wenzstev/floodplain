
set(SOURCE_FILES src/transform.cpp)

add_library(transform STATIC ${SOURCE_FILES})
target_link_libraries(transform flecs::flecs_static)

target_include_directories(transform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


add_subdirectory(tests)

