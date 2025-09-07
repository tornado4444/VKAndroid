cmake_minimum_required(VERSION 3.20)
project(main LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE PROJECT_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

if(ANDROID)
    add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCES})

    find_library(log-lib log)
    find_library(android-lib android)
    find_library(EGL-lib EGL)
    find_library(GLESv2-lib GLESv2)

    target_link_libraries(${PROJECT_NAME}
            ${log-lib}
            ${android-lib}
            ${EGL-lib}
            ${GLESv2-lib}
    )

    find_path(VULKAN_INCLUDE_DIR vulkan/vulkan.h
            PATHS ${ANDROID_NDK}/sysroot/usr/include
    )

    if(VULKAN_INCLUDE_DIR)
        target_include_directories(${PROJECT_NAME} PRIVATE ${VULKAN_INCLUDE_DIR})
    endif()

else()
    add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

    find_package(Vulkan REQUIRED)
    if(Vulkan_FOUND)
        target_link_libraries(${PROJECT_NAME} ${Vulkan_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIRS})
    endif()
endif()

set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

if(EXISTS "${SHADER_DIR}")
    file(GLOB SHADER_FILES
            "${SHADER_DIR}/*.vert"
            "${SHADER_DIR}/*.frag"
            "${SHADER_DIR}/*.comp"
            "${SHADER_DIR}/*.geom"
            "${SHADER_DIR}/*.tesc"
            "${SHADER_DIR}/*.tese"
    )

    if(SHADER_FILES AND ANDROID)
        get_filename_component(CURRENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

        set(POSSIBLE_ASSETS_PATHS
                "${CURRENT_DIR}/../../../src/main/assets"
                "${CURRENT_DIR}/../../src/main/assets"
                "${CURRENT_DIR}/../src/main/assets"
                "${CURRENT_DIR}/src/main/assets"
        )

        set(ANDROID_ASSETS_DIR "")
        foreach(ASSETS_PATH ${POSSIBLE_ASSETS_PATHS})
            get_filename_component(NORMALIZED_PATH "${ASSETS_PATH}" ABSOLUTE)
            if(EXISTS "${NORMALIZED_PATH}")
                set(ANDROID_ASSETS_DIR "${NORMALIZED_PATH}")
                break()
            endif()
        endforeach()

        if(NOT ANDROID_ASSETS_DIR)
            set(ANDROID_ASSETS_DIR "${CURRENT_DIR}/assets")
        endif()

        set(ASSETS_SHADER_DIR "${ANDROID_ASSETS_DIR}/shaders")
        file(MAKE_DIRECTORY "${ANDROID_ASSETS_DIR}")
        file(MAKE_DIRECTORY "${ASSETS_SHADER_DIR}")

        find_program(GLSLC_EXECUTABLE glslc
                PATHS
                ${ANDROID_NDK}/shader-tools/windows-x86_64/
                ${ANDROID_NDK}/shader-tools/linux-x86_64/
                ${ANDROID_NDK}/shader-tools/darwin-x86_64/
                ENV PATH
        )

        if(GLSLC_EXECUTABLE)
            set(SPIRV_BINARY_FILES "")
            foreach(SHADER ${SHADER_FILES})
                get_filename_component(FILE_NAME ${SHADER} NAME)
                set(SPIRV "${ASSETS_SHADER_DIR}/${FILE_NAME}.spv")

                add_custom_command(
                        OUTPUT ${SPIRV}
                        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_SHADER_DIR}"
                        COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
                        DEPENDS ${SHADER}
                        VERBATIM
                )
                list(APPEND SPIRV_BINARY_FILES ${SPIRV})
            endforeach()

            add_custom_target(Shaders ALL
                    DEPENDS ${SPIRV_BINARY_FILES}
            )
            add_dependencies(${PROJECT_NAME} Shaders)

        else()
            foreach(SHADER ${SHADER_FILES})
                get_filename_component(FILE_NAME ${SHADER} NAME)
                set(DEST_FILE "${ASSETS_SHADER_DIR}/${FILE_NAME}")

                add_custom_command(
                        OUTPUT ${DEST_FILE}
                        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSETS_SHADER_DIR}"
                        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} ${DEST_FILE}
                        DEPENDS ${SHADER}
                        VERBATIM
                )
                list(APPEND SHADER_COPY_FILES ${DEST_FILE})
            endforeach()

            add_custom_target(ShaderCopy ALL
                    DEPENDS ${SHADER_COPY_FILES}
            )
            add_dependencies(${PROJECT_NAME} ShaderCopy)
        endif()

    elseif(SHADER_FILES AND NOT ANDROID)
        set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
        file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

        find_program(GLSLC_EXECUTABLE glslc)
        if(GLSLC_EXECUTABLE)
            set(SPIRV_BINARY_FILES "")
            foreach(SHADER ${SHADER_FILES})
                get_filename_component(FILE_NAME ${SHADER} NAME)
                set(SPIRV "${SHADER_OUTPUT_DIR}/${FILE_NAME}.spv")

                add_custom_command(
                        OUTPUT ${SPIRV}
                        COMMAND ${GLSLC_EXECUTABLE} ${SHADER} -o ${SPIRV}
                        DEPENDS ${SHADER}
                )
                list(APPEND SPIRV_BINARY_FILES ${SPIRV})
            endforeach()

            add_custom_target(Shaders ALL DEPENDS ${SPIRV_BINARY_FILES})
            add_dependencies(${PROJECT_NAME} Shaders)
        endif()
    endif()
endif()

if(NOT PROJECT_SOURCES)
    message(FATAL_ERROR "No source files found!")
endif()