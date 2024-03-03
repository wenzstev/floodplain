
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
	transformtests 
	transformtests.cc
)


target_link_libraries(
	transformtests 
	transform
	gtest 
	gtest_main
	gmock
	gmock_main
	
)

target_link_libraries(
	transformtests 
	flecs::flecs_static
)

target_include_directories(transformtests PRIVATE ../include)


include(GoogleTest)
gtest_discover_tests(transformtests)