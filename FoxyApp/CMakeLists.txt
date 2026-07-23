file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.h
    src/*.cpp
)
# if you wanna change name of executable from app <-> foxyapp or something... change the name in all the below instances of that name
add_executable(FoxyApp ${SOURCES})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

target_link_libraries(FoxyApp PRIVATE Foxy)

set_target_properties(FoxyApp PROPERTIES
    #VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/FoxyApp"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

#if (WIN32)
#    target_compile_definitions(FoxyApp PRIVATE FOXY_PLATFORM_WINDOWS)
#endif()
#
## WithConsole onDebug || WithoutConsole onRelease
#set_target_properties(FoxyApp PROPERTIES
#    WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>
#)

############################################################################

# Scans shaders/*.slang, auto-detects which of vertMain/fragMain/compMain
# each file defines (by simple text search), and compiles each to its own .spv.
# Drop a new .slang file in shaders/ and it's picked up on the next CMake configure  
# no manual registration needed.

function(add_slang_shaders)
    file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.slang")
    set(SHADER_OUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")

    foreach(SHADER_SRC_PATH ${SHADER_SOURCES})
        get_filename_component(SHADER_NAME ${SHADER_SRC_PATH} NAME_WE)
        set(SHADER_OUT_PATH "${SHADER_OUT_DIR}/${SHADER_NAME}.spv")

        file(READ ${SHADER_SRC_PATH} SHADER_CONTENTS)
        set(ENTRY_POINT_ARGS "")
        foreach(ENTRY vertMain fragMain compMain)
            string(FIND "${SHADER_CONTENTS}" "${ENTRY}" FOUND_IDX)
            if (NOT FOUND_IDX EQUAL -1)
                list(APPEND ENTRY_POINT_ARGS -entry ${ENTRY})
            endif()
        endforeach()

        if (NOT ENTRY_POINT_ARGS)
            message(FATAL_ERROR "No recognized entry points (vertMain/fragMain/compMain) found in ${SHADER_SRC_PATH}")
        endif()

        target_sources(FoxyApp PRIVATE ${SHADER_SRC_PATH})

        set_source_files_properties(${SHADER_SRC_PATH} PROPERTIES
            VS_TOOL_OVERRIDE "None"
            HEADER_FILE_ONLY TRUE
        )

        add_custom_command(
            OUTPUT ${SHADER_OUT_PATH}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUT_DIR}
            COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SRC_PATH}
                    -target spirv -profile spirv_1_4 -emit-spirv-directly
                    -fvk-use-entrypoint-name
                    ${ENTRY_POINT_ARGS}
                    -o ${SHADER_OUT_PATH}
            DEPENDS ${SHADER_SRC_PATH}
            COMMENT "Compiling Slang shader: ${SHADER_NAME}.slang -> shaders/${SHADER_NAME}.spv"
            VERBATIM
        )
        add_custom_target(${SHADER_NAME}_shader DEPENDS ${SHADER_OUT_PATH})
        add_dependencies(FoxyApp ${SHADER_NAME}_shader)
    endforeach()
endfunction()

add_slang_shaders()
