set(SOURCE_FILES src/maputils.cpp)

add_library(maputils STATIC ${SOURCE_FILES})
target_link_libraries(maputils flecs::flecs_static sfml-graphics)

target_include_directories(maputils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../canvas2d/include)

install(TARGETS maputils)

