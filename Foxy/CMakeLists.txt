file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/Foxy/*.h
    src/Foxy/*.cpp
)

add_library(Foxy STATIC ${SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_include_directories(Foxy PUBLIC src)
target_link_libraries(Foxy PUBLIC glfw glm Vulkan::Vulkan nvrhi)

target_compile_definitions(Foxy PUBLIC
    $<$<CONFIG:Debug>:FOXY_DEBUG>
    $<$<CONFIG:Release>:FOXY_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:FOXY_RELEASE>
)

if (WIN32)
    target_compile_definitions(Foxy PUBLIC FOXY_PLATFORM_WINDOWS)
endif()