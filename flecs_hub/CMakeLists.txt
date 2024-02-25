cmake_minimum_required(VERSION 3.0)
project(flecs-components-graphics C)  # This is a C project

FetchContent_Declare(
	cglm
	GIT_REPOSITORY https://github.com/recp/cglm.git
	GIT_TAG master
)
FetchContent_MakeAvailable(cglm)


set(SOURCE_FILES
    deps/flecs_components_cglm.c
)

add_library(flecs-components-graphics ${SOURCE_FILES})
target_link_libraries(flecs-components-graphics cglm)

target_include_directories(flecs-components-graphics PUBLIC deps/${CMAKE_CURRENT_SOURCE_DIR})

set_property(TARGET flecs-components-graphics PROPERTY C_STANDARD 99)

