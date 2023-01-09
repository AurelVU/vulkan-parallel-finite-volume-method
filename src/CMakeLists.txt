project(test_vulkan)

cmake_minimum_required(VERSION 3.17)

find_package(Vulkan REQUIRED)


find_program(GlslangValidator NAMES glslangValidator DOC "glsl to SPIR-V compiler")
if(NOT GlslangValidator)
    message(FATAL_ERROR "failed to find glslangValidator")
endif()

function(compile_shader)
    set(OneValueArgs SOURCE TARGET)
    cmake_parse_arguments(COMPILE_SHADER "" "${OneValueArgs}" "" ${ARGN})

    get_filename_component(TargetDir ${COMPILE_SHADER_TARGET} DIRECTORY)
    add_custom_command(
            COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${TargetDir}
            COMMAND ${GlslangValidator} ARGS -V ${COMPILE_SHADER_SOURCE} -o ${COMPILE_SHADER_TARGET}
            DEPENDS ${COMPILE_SHADER_SOURCE}
            OUTPUT ${COMPILE_SHADER_TARGET}
    )
    add_custom_target(${ARGV0} DEPENDS ${COMPILE_SHADER_TARGET})
endfunction()


compile_shader(saxpy_shader
   SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/shaders/saxpy.comp
   TARGET ${CMAKE_CURRENT_BINARY_DIR}/shaders/saxpy.spv
)

add_library(example_filter STATIC example_filter.cpp vulkan_helpers.cpp)
target_link_libraries(example_filter PUBLIC Vulkan::Vulkan)
target_include_directories(example_filter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
add_dependencies(example_filter saxpy_shader)

add_executable(vulkan_example main.cpp)
target_link_libraries(vulkan_example PRIVATE example_filter)
