include(FetchContent)

#1. Include OpenGL
find_package(Vulkan REQUIRED)

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

#4. Add Vk Bootstrap
find_package(vk-bootstrap QUIET)
if(vk-bootstrap_FOUND)
    message(STATUS "Using vk-bootstrap via find_package")
endif()

if(NOT vk-bootstrap_FOUND)
    FetchContent_Declare(
            vk_bootstrap
            GIT_REPOSITORY "https://github.com/charles-lunarg/vk-bootstrap"
            GIT_TAG        v1.4.321
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    message(STATUS "Using vk-bootstrap via FetchContent")
    FetchContent_MakeAvailable(vk_bootstrap)
endif()

#5STB
find_package(STB QUIET)
if(STB_FOUND)
    message(STATUS "Using STB via find_package")
endif()
if(NOT STB_FOUND)
    FetchContent_Declare(
            STB
            GIT_REPOSITORY "https://github.com/unrealsid/stb-cmake.git"
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
    )
    message(STATUS "Using STB via FetchContent")
    FetchContent_MakeAvailable(STB)
endif()


