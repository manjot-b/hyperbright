cmake_minimum_required(VERSION 3.16.3)
project(myapp)

set(CMAKE_CXX_STANDARD 17)

add_executable(${CMAKE_PROJECT_NAME})


include_directories(inc)

# Make sure OpenGL is on the system.
find_package(OpenGL REQUIRED)

# Include OpenGL headers and link library.
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_gl_LIBRARIES} ${CMAKE_DL_LIBS})

if( MSVC )
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
	
	# Output dll to the executable directory.
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
	
	# Assimp outputs its dlls to its own directory so they must be copied over manually.
	set(ASSIMP_DLL "${CMAKE_BINARY_DIR}/dep/assimp/bin/$<CONFIG>")
	set(EXEC_DIR "${CMAKE_BINARY_DIR}/$<CONFIG>")
	add_custom_command (
		TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSIMP_DLL} ${EXEC_DIR}
		DEPENDS ${ASSIMP_DLL}
		COMMENT "Copying ${ASSIMP_DLL} => ${EXEC_DIR}"
	)
endif()

add_subdirectory(src)
add_subdirectory(dep)

# Copy resource folders to build directory.
# We don't want to copy if we're building in the source dir.
# Note that symbolic linking is not availible on windows.
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
	set (MODELS "${CMAKE_SOURCE_DIR}/rsc/models")
	set (MODELS_DEST "${CMAKE_BINARY_DIR}/models")
    add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${MODELS} ${MODELS_DEST}
		DEPENDS ${MODELS_DEST}
		COMMENT "Copying ${MODELS} => ${MODELS_DEST}"
    )

	set (IMAGES "${CMAKE_SOURCE_DIR}/rsc/images")
	set (IMAGES_DEST "${CMAKE_BINARY_DIR}/images")
    add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${IMAGES} ${IMAGES_DEST}
		DEPENDS ${IMAGES_DEST}
		COMMENT "Copying ${IMAGES} => ${IMAGES_DEST}\n"
    )

	set (SHADERS "${CMAKE_SOURCE_DIR}/rsc/shaders")
	set (SHADERS_DEST "${CMAKE_BINARY_DIR}/shaders")
    add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${SHADERS} ${SHADERS_DEST}
		DEPENDS ${SHADERS_DEST}
		COMMENT "Copying ${SHADERS} => ${SHADERS_DEST}"
    )
endif()
