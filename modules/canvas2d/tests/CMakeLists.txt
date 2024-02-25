include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(
	canvas2dtests 
	canvas2dtests.cc
)

target_link_libraries(
	canvas2dtests 
	GTest::gtest_main
	canvas2d
)

target_link_libraries(
	canvas2dtests 
	flecs::flecs_static
)

target_include_directories(canvas2dtests PRIVATE ../include)

include(GoogleTest)
gtest_discover_tests(canvas2dtests)