cmake_minimum_required(VERSION 3.5...3.28)

project(ShoutBlast)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type" FORCE)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
    if(MSVC)
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /DEBUG:NONE")
    else()
        set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g0")
    endif()
endif()

file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/Audio"
    "${PROJECT_SOURCE_DIR}/include/Core"
    "${PROJECT_SOURCE_DIR}/include/Embedded"
    "${PROJECT_SOURCE_DIR}/include/Graphics"
    "${PROJECT_SOURCE_DIR}/include/Graphics/Shaders"
    "${PROJECT_SOURCE_DIR}/include/Utilities"
)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")

add_subdirectory(libs/glfw)
add_subdirectory(libs/glad)
add_subdirectory(libs/imgui)
add_subdirectory(libs/miniaudio)
add_subdirectory(libs/stb)
add_subdirectory(libs/tinyfiledialogs)


add_executable(${PROJECT_NAME} ${SOURCES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} glad glfw imgui miniaudio stb tfd ws2_32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} glad glfw imgui miniaudio stb tfd pthread -ldl)
endif()