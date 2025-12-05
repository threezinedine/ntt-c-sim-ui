cmake_minimum_required(VERSION 3.20)
project(SimUI LANGUAGES C)

option(
    SIMUI_BUILD_EXAMPLES
    "Build SimUI example applications"
    OFF
)

option(
    SIMUI_USE_DEFAULT_RENDERER
    "Use the default rendering backend"
    ON
)

option(
    CMAKE_BUILD_TYPE 
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    Debug
)

if (MSVC)
    set_property(GLOBAL PROPERTIES USE_FOLDERS ON)
endif()

file(
    GLOB 
    SIMUI_SOURCES
    "src/*.c"
)

file(
    GLOB 
    SIMUI_HEADERS
    "include/simui/*.h"
)

set(SIMUI_ALL_SOURCES ${SIMUI_SOURCES} ${SIMUI_HEADERS})
set(SIMUI_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(
    ${PROJECT_NAME} 
    STATIC
    ${SIMUI_ALL_SOURCES}
)

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
    ${SIMUI_INCLUDE_DIRS}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
        SIMUI_DEBUG
        SOURCE_PATH=${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_compile_options(
        ${PROJECT_NAME}
        PUBLIC 
        -g
    )

    message(STATUS "SimUI: Debug build enabled.")
else()
    target_compile_options(
        ${PROJECT_NAME}
        PUBLIC 
        -O3
    )
    message(STATUS "SimUI: Release build enabled.")
endif()

if (SIMUI_USE_DEFAULT_RENDERER)
    target_compile_definitions(
        ${PROJECT_NAME}
        PUBLIC
        SIMUI_USE_DEFAULT_RENDERER
    )

    message(STATUS "SimUI: Using default renderer backend.")

    include(FetchContent)

    if (NOT TARGET glfw)
        FetchContent_Declare(
            glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
        )
        FetchContent_MakeAvailable(glfw)
    endif()

    if (NOT TARGET GLAD)
        FetchContent_Declare(
            GLAD
            GIT_REPOSITORY https://github.com/threezinedine/glad.git
        )
        FetchContent_MakeAvailable(glad)
    endif()

    find_package(OpenGL REQUIRED)

    target_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        glfw
        GLAD
        OpenGL::GL
    )
endif()

if (MSVC)
    set(CMAKE_FOLDER "Example")
endif()

if (SIMUI_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (MSVC)
    unset(CMAKE_FOLDER)
endif()