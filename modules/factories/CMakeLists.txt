# CMakeList.txt : CMake project for FlecsTest, include source and define
# project specific logic here.
#

set(SOURCE_FILES src/factories.cpp src/CollectResourceSystem.cpp)

add_library(factories STATIC ${SOURCE_FILES})
target_link_libraries(factories flecs::flecs_static)

target_include_directories(factories PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET factories PROPERTY CXX_STANDARD 20)
endif()


add_subdirectory(tests)