cmake_minimum_required(VERSION 3.16)
project(main)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Force shared libraries" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(LIB_TYPE "SHARED")

set(SDL_RENDER ON CACHE BOOL "" FORCE)
set(SDL_GPU ON CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory("${CMAKE_SOURCE_DIR}/SDL3-3.2.22" "${CMAKE_BINARY_DIR}/sdl3_build")

if(ANDROID)
    find_library(vulkan-lib vulkan)
endif()

add_library(main SHARED native-lib.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

if(ANDROID)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            SDL3::SDL3
            ${vulkan-lib}
            android
            log
    )
else()
    find_package(Vulkan REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE
            SDL3::SDL3
            Vulkan::Vulkan
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VK_USE_PLATFORM_ANDROID_KHR
)
