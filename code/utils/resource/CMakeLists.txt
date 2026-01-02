add_library(Resource INTERFACE)

target_include_directories(Resource INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_library(utils::Resource ALIAS Resource)

