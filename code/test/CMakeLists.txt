add_library(Test STATIC)

find_package(OpenGL REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(stb_image REQUIRED)

target_include_directories(Test PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(Test PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/TestRenderCode.cpp
)

target_link_libraries(Test PRIVATE
	glad::glad
	glm::glm
	glfw
	stb::stb
	gl::Shader
	gl::Utils
	gl::test
	utils::EventBus
	utils::ModelLoader
)
