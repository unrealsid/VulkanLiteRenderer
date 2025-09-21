include(FetchContent)

#1. Include OpenGL
find_package(OpenGL REQUIRED)

#2. Download GLFW3, if required
find_package(GLFW3 CONFIG QUIET)
if(GLFW3_FOUND)
    message(STATUS "Using GLFW via find_package")
endif()
if(NOT GLFW3_FOUND)
    FetchContent_Declare(
            GLFW3
            GIT_REPOSITORY "https://github.com/glfw/glfw.git"
            GIT_TAG        "master"
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    message(STATUS "Using GLFW via FetchContent")
    FetchContent_MakeAvailable(GLFW3)
endif()

#3. Download GLM, if required
find_package(GLM CONFIG QUIET)
if(GLM_FOUND)
    message(STATUS "Using GLM via find_package")
endif()
if(NOT GLM_FOUND)
    FetchContent_Declare(
            GLM
            GIT_REPOSITORY "https://github.com/g-truc/glm"
            GIT_TAG        "master"
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    message(STATUS "Using GLM via FetchContent")
    FetchContent_MakeAvailable(GLM)
endif()

#4. Setup glad libs
set(GLAD_LIBS glad)
set(GLAD_LIBS_DIR ${CMAKE_SOURCE_DIR}/libraries)

file(GLOB_RECURSE GLAD_LIBS_GLOBAL_DIR
        "${GLAD_LIBS_DIR}/*.h"
        "${GLAD_LIBS_DIR}/*.c"
)

message(STATUS "Current source files in imgui: ${GLAD_LIBS_GLOBAL_DIR}")

add_library(${GLAD_LIBS} STATIC ${GLAD_LIBS_GLOBAL_DIR})
target_include_directories(${GLAD_LIBS} PUBLIC ${GLAD_LIBS_GLOBAL_DIR})

target_link_libraries(${GLAD_LIBS} PUBLIC OpenGL::GL)



