# Make sure to add the submodules first, otherwise CMake will throw an error.

set(BUILD_SHARED_LIBS ON CACHE BOOL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")
add_subdirectory(glfw)
target_link_libraries(${CMAKE_PROJECT_NAME} glfw)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
add_subdirectory(assimp)
target_link_libraries(${CMAKE_PROJECT_NAME} assimp)