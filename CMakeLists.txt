cmake_minimum_required(VERSION 3.16.3)
project(myapp)

set(CMAKE_CXX_STANDARD 17)

set(EXEC "myapp")
add_executable(${EXEC})


include_directories(inc)

# Make sure OpenGL is on the system.
find_package(OpenGL REQUIRED)

# Include OpenGL headers and link library.
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(${EXEC} ${OPENGL_gl_LIBRARIES})

add_subdirectory(src)
add_subdirectory(dep)

#Copy resource folder to build directory
# we don't want to copy if we're building in the source dir
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
	set (MODELS "${CMAKE_SOURCE_DIR}/rsc/models")
	set (MODELS_DEST "${CMAKE_CURRENT_BINARY_DIR}/models")
    add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink ${MODELS} ${MODELS_DEST}
		DEPENDS ${MODELS_DEST}
		COMMENT "Linking ${MODELS} => ${MODELS_DEST}"
    )

	set (IMAGES "${CMAKE_SOURCE_DIR}/rsc/images")
	set (IMAGES_DEST "${CMAKE_CURRENT_BINARY_DIR}/images")
    add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink ${IMAGES} ${IMAGES_DEST}
		DEPENDS ${IMAGES_DEST}
		COMMENT "Linking ${IMAGES} => ${IMAGES_DEST}"
    )

	set (SHADERS "${CMAKE_SOURCE_DIR}/rsc/shaders")
	set (SHADERS_DEST "${CMAKE_CURRENT_BINARY_DIR}/shaders")
    add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink ${SHADERS} ${SHADERS_DEST}
		DEPENDS ${SHADERS_DEST}
		COMMENT "Linking ${SHADERS} => ${SHADERS_DEST}"
    )
endif()
