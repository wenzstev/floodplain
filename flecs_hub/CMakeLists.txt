cmake_minimum_required(VERSION 3.0)
project(flecs-components-graphics C)  # This is a C project

FetchContent_Declare(
	cglm
	GIT_REPOSITORY https://github.com/recp/cglm.git
	GIT_TAG master
)
FetchContent_MakeAvailable(cglm)

FetchContent_Declare(
	flecs
	GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
	GIT_TAG master
)
FetchContent_MakeAvailable(flecs)


set(SOURCE_FILES
	"flecs_components_transform.c" 
    "flecs_components_cglm.c"
	"flecs_components_graphics.c"
	"flecs_components_geometry.c"
	"flecs_components_gui.c" 
	"flecs_components_input.c" 
	"flecs_components_physics.c"
	"flecs_game.c" 
	"flecs_systems_physics.c" 
	"flecs_systems_transform.c" 
)


add_library(flecs-components-graphics STATIC ${SOURCE_FILES})
target_link_libraries(flecs-components-graphics cglm flecs::flecs_static)

target_include_directories(flecs-components-graphics PUBLIC include)

set_property(TARGET flecs-components-graphics PROPERTY C_STANDARD 99)

