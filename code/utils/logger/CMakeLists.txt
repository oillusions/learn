add_library(Logger STATIC)

target_include_directories(Logger PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(Logger PRIVATE
#	${CMAKE_CURRENT_SOURCE_DIR}/ModelParser.cpp
)

target_link_libraries(Logger PRIVATE
	gl::Utils
)


add_library(utils::Logger ALIAS Logger)