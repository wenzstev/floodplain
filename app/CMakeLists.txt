
add_executable(flecs_console src/main.cpp   )

target_link_libraries(flecs_console PRIVATE agents grid canvas2d transform flecs::flecs_static sfml-graphics)
target_compile_features(flecs_console PRIVATE cxx_std_17)


target_include_directories(flecs_console PRIVATE 
    ../modules/grid/include 
    ../modules/canvas2d/include
    ../modules/transform/include
    ../modules/agents/include
)

if(WIN32)
    add_custom_command(
        TARGET flecs_console
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:flecs_console>
        VERBATIM)
endif()

install(TARGETS flecs_console)