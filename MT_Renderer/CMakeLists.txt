cmake_minimum_required(VERSION 4.0 FATAL_ERROR)
project(MT_Renderer)

set(CMAKE_CXX_STANDARD 20)

include (cmake/dependencies.cmake)
include(GNUInstallDirs)

set(ENGINE_PROJECT_NAME "MT_Renderer")

#Header includes
file(GLOB_RECURSE HEADER_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
        PREFIX "Include Files"
        FILES ${HEADER_FILES})

#Source
file(GLOB_RECURSE SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
        "main.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
        PREFIX "Source Files"
        FILES ${SRC_FILES})

set(
    ALL_FILES
    ${HEADER_FILES}
    ${SRC_FILES}
)

add_executable(${ENGINE_PROJECT_NAME} ${ALL_FILES})

target_include_directories(
        ${ENGINE_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC Vulkan::Vulkan
                                                    glfw
                                                    glm
                                                    vk-bootstrap::vk-bootstrap
                                                    STB-image::STB-image)