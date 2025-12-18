add_library(SceneNode STATIC)

find_package(glm REQUIRED)

target_include_directories(SceneNode PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(SceneNode PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/TransForm.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/SceneNode.cpp
)

target_link_libraries(SceneNode PRIVATE
	glm::glm
)

add_library(gl::SceneNode ALIAS SceneNode)