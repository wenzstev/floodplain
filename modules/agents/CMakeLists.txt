
set(SOURCE_FILES src/agents.cpp)

add_library(agents STATIC ${SOURCE_FILES})
target_link_libraries(agents flecs::flecs_static transform)

target_include_directories(agents PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ../transform/include)

install(TARGETS agents)

