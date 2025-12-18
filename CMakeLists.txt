cmake_minimum_required(VERSION 3.12)

project("learn" VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
list(APPEND CMAKE_PREFIX_PATH "C:/libs/cmake/glfw")
list(APPEND CMAKE_PREFIX_PATH "C:/libs/cmake/glm")

add_executable(${CMAKE_PROJECT_NAME} ./code/main.cpp)

find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(stb_image REQUIRED)

set(GLM_BUILD_LIBRARY OFF)

add_subdirectory(code/opengl/shader)
add_subdirectory(code/opengl/scene_node)
add_subdirectory(code/opengl/utils)
add_subdirectory(code/utils/model_loader)
add_subdirectory(code/test)

#target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
#	-static-libgcc
#	-static-libstdc++
#	-static
#)

target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC
	OpenGL::GL
	glad::glad
	glm::glm
    glfw
	stb::stb
	gl::Shader
	gl::SceneNode
	gl::Utils
	utils::ModelLoader
	Test
)


