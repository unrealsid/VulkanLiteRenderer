set(ENGINE_PROJECT_NAME "MT_Renderer")

# Automatically get all .cpp and .h files from the src directory
file(GLOB_RECURSE SRC_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.glsl"
)

add_executable(${ENGINE_PROJECT_NAME} ${SRC_FILES})

target_include_directories(
        ${ENGINE_PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${ENGINE_PROJECT_NAME} PUBLIC OpenGL::GL
                                                    glad
                                                    glfw
                                                    glm)