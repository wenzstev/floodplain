include(FetchContent)
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(
	gridtests 
	gridtests.cc
)

target_link_libraries(
	gridtests 
	GTest::gtest_main
	grid
)

target_link_libraries(
	gridtests 
	flecs::flecs_static
)

target_include_directories(gridtests PRIVATE ../include)

include(GoogleTest)
gtest_discover_tests(gridtests)