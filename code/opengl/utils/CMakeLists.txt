add_library(Utils INTERFACE)

find_package(glad REQUIRED)


target_include_directories(Utils INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Utils INTERFACE
	glad::glad
)


add_library(gl::Utils ALIAS Utils)
