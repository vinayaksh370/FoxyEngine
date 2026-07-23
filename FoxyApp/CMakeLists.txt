file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.h
    src/*.cpp
)
# if you wanna change name of executable from app <-> foxyapp or something... change the name in all the below instances of that name
add_executable(FoxyApp ${SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_link_libraries(FoxyApp PRIVATE Foxy)

set_target_properties(FoxyApp PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/FoxyApp"
)

#if (WIN32)
#    target_compile_definitions(FoxyApp PRIVATE FOXY_PLATFORM_WINDOWS)
#endif()
#
## WithConsole onDebug || WithoutConsole onRelease
#set_target_properties(FoxyApp PROPERTIES
#    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
#)