
include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) 
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # Use MSVC runtime library dynamically


FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(
	floodplaintests 
	transformtests.cc
	canvas2dtests.cc 
	gridtests.cc
	guitests.cc
)


target_link_libraries(
	floodplaintests 
	grid 
	agents 
	canvas2d 
	transform
	sfml-main
	transform
	gtest 
	gtest_main
	gmock
	gmock_main
	
)

target_link_libraries(
	floodplaintests 
	flecs::flecs_static
)

target_include_directories(floodplaintests PRIVATE ../modules/grid/include ../modules/agents/include ../modules/transform/include ../modules/canvas2d/include)


include(GoogleTest)
gtest_discover_tests(floodplaintests)