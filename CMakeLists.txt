# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
add_definitions(-DPROJECT_ROOT_DIR="${CMAKE_SOURCE_DIR}")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Floodplain")

include(FetchContent)
FetchContent_Declare(
	flecs
	GIT_REPOSITORY https://github.com/SanderMertens/flecs.git
	GIT_TAG master
)
FetchContent_MakeAvailable(flecs)

FetchContent_Declare(SFML 
	GIT_REPOSITORY https://github.com/SFML/SFML.git 
	GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
	flecs-components-graphics
	GIT_REPOSITORY https://github.com/flecs-hub/flecs-components-graphics.git
	GIT_TAG master
)
FetchContent_MakeAvailable(flecs-components-graphics)


# Include sub-projects.
add_subdirectory ("app")
add_subdirectory ("modules")
add_subdirectory ("flecs_hub")
