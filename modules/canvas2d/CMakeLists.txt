
set(SOURCE_FILES src/canvas2d.cpp)

add_library(canvas2d STATIC ${SOURCE_FILES})
target_link_libraries(canvas2d flecs::flecs_static sfml-graphics)

target_include_directories(canvas2d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET canvas2d PROPERTY CXX_STANDARD 20)
endif()


if(WIN32)
    add_custom_command(
        TARGET canvas2d
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:canvas2d>
        VERBATIM)
endif()

install(TARGETS canvas2d)

add_subdirectory(tests)

