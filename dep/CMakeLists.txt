# Make sure to add the submodules first, otherwise CMake will throw an error.

set(BUILD_SHARED_LIBS OFF CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(glfw)
target_link_libraries(${EXEC} glfw)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
add_subdirectory(assimp)
target_link_libraries(${EXEC} assimp)
