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
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "")
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "")
add_subdirectory(assimp)
target_link_libraries(${CMAKE_PROJECT_NAME} assimp)

# For PhysX setup see https://github.com/NVIDIAGameWorks/PhysX/issues/249#issuecomment-722013539
#### Build PhysX library ####
set( PHYSX_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/physx/physx" ) #This is needed for ${PHYSX_ROOT_DIR}/compiler/public/CMakeLists.txt
set( PHYSX_INCLUDE_DIRS ${PHYSX_ROOT_DIR}/include/ ${PHYSX_ROOT_DIR}/../pxshared/include/ )
set( PHYSX_LIBRARIES
	PhysXExtensions
	PhysX
	PhysXPvdSDK
	PhysXVehicle
	PhysXCharacterKinematic
	PhysXCooking
	PhysXCommon
	PhysXFoundation
	# SnippetUtils
)

# PhysX specific setting to build for the appropriate platform.
# Has to match the TARGET_BUILD_PLATFORM in ${PHYSX_ROOT_DIR}/physix/buildtools/cmake_generate_projects.py
if (MSVC)
	set(TARGET_BUILD_PLATFORM "windows")
else ()
	set(TARGET_BUILD_PLATFORM "linux")
endif ()

# If you want to build snippets you will have to manually copy glut32.dll and
# PhysXDevice64.dll from the physx submodule directory physx/bin/<VS VERSION>/<CONFIG>
# to where ever the snippet executable is built. It should be built in (from the root dir)
# build/dep/physx/bin/<VS VERSION>/<CONFIG>.
set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")

set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
# set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
set(PX_FLOAT_POINT_PRECISE_MATH OFF CACHE BOOL "Float point precise math")
set(NV_USE_STATIC_WINCRT ON CACHE BOOL "Use the statically linked windows CRT")
set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR "${CMAKE_BINARY_DIR}/dep/physx")
set(PX_OUTPUT_BIN_DIR "${CMAKE_BINARY_DIR}/dep/physx")
  
# Call into PhysX's CMake scripts
add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public)

# Add physx libraries to target
target_link_libraries(${CMAKE_PROJECT_NAME} ${PHYSX_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${PHYSX_INCLUDE_DIRS})

# Build Dear ImGUI as a library. It does not have its own CMakeLists.txt file
# so we will have to do this manually.
set(IMGUI_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/imgui")
set(IMGUI_BACKEND_DIR "${IMGUI_ROOT_DIR}/backends")
# add_library(imgui SHARED
# 	"${IMGUI_ROOT_DIR}/imconfig.h"
# 	"${IMGUI_ROOT_DIR}/imgui.cpp"
# 	"${IMGUI_ROOT_DIR}/imgui.h"
# 	"${IMGUI_ROOT_DIR}/imgui_demo.cpp"
# 	"${IMGUI_ROOT_DIR}/imgui_draw.cpp"
# 	"${IMGUI_ROOT_DIR}/imgui_internal.h"
# 	"${IMGUI_ROOT_DIR}/imgui_tables.cpp"
# 	"${IMGUI_ROOT_DIR}/imgui_widgets.cpp"
# 	"${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp"
# 	"${IMGUI_BACKEND_DIR}/imgui_impl_glfw.h"
# 	"${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp"
# 	"${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.h"
# 	"${CMAKE_SOURCE_DIR}/src/glad.cpp")
# target_include_directories(imgui PRIVATE "${IMGUI_ROOT_DIR}" "${IMGUI_BACKEND_DIR}" "${CMAKE_SOURCE_DIR/inc}")
# target_link_libraries(imgui glfw)

# target_link_libraries(${CMAKE_PROJECT_NAME} imgui)
# target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${IMGUI_ROOT_DIR}" "${IMGUI_BACKEND_DIR}")
set(IMGUI_SOURCES PRIVATE
	"${IMGUI_ROOT_DIR}/imconfig.h"
	"${IMGUI_ROOT_DIR}/imgui.cpp"
	"${IMGUI_ROOT_DIR}/imgui.h"
	"${IMGUI_ROOT_DIR}/imgui_demo.cpp"
	"${IMGUI_ROOT_DIR}/imgui_draw.cpp"
	"${IMGUI_ROOT_DIR}/imgui_internal.h"
	"${IMGUI_ROOT_DIR}/imgui_tables.cpp"
	"${IMGUI_ROOT_DIR}/imgui_widgets.cpp"
	"${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp"
	"${IMGUI_BACKEND_DIR}/imgui_impl_glfw.h"
	"${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp"
	"${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.h"
	"${CMAKE_SOURCE_DIR}/src/glad.cpp")
target_sources(${CMAKE_PROJECT_NAME} ${IMGUI_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${IMGUI_ROOT_DIR}" "${IMGUI_BACKEND_DIR}")