add_library(test INTERFACE)

target_include_directories(test INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(test INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/Model.cpp
)

target_link_libraries(test INTERFACE
	glad::glad
	glm::glm
)


add_library(gl::test ALIAS test)
