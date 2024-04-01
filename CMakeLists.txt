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

FetchContent_Declare(
	SFML 
	GIT_REPOSITORY https://github.com/SFML/SFML.git 
	GIT_TAG 2.6.x
)

FetchContent_MakeAvailable(SFML)

set (TGUI_BACKEND "SFML_GRAPHICS" CACHE STRING "Set TGUI Backend to SFML_GRAPHICS" FORCE)

FetchContent_Declare(
	TGUI 
	GIT_REPOSITORY https://github.com/texus/TGUI.git
	GIT_TAG v1.2.0
)	

FetchContent_MakeAvailable(TGUI)

# Set the option for static linking
set(SFML_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)



# Include sub-projects.
add_subdirectory ("app")
add_subdirectory ("modules")
add_subdirectory ("tests")
