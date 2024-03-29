cmake_minimum_required(VERSION 3.18)
project(HyperBright)

set(CMAKE_CXX_STANDARD 17)

add_executable(${CMAKE_PROJECT_NAME})

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	# Turn off restrict and class-memaccess error so physx can compile on linux/ubuntu distros.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=restrict")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=class-memaccess")
	# Turn off some errors so physx can compile on arch linux.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=array-bounds")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=nonnull")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=stringop-overread")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=mismatched-new-delete")

	# Physx requires exactly one of NDEBUG or _DEBUG to be defined, but _DEBUG is only
	# used by Visual Studio.
	string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
	if (${BUILD_TYPE_LOWER} STREQUAL "debug")
	  target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _DEBUG)
	endif()
endif()


# Make sure OpenGL is on the system.
find_package(OpenGL REQUIRED)

# Include OpenGL headers and link library.
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_gl_LIBRARIES} ${CMAKE_DL_LIBS})

# Store all .cpp file in SOURCES
file(GLOB_RECURSE SOURCES
	"${CMAKE_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/ai/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/audio/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/engine/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/entity/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/model/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/opengl-helper/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/physics/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/render/*.cpp"
	"${CMAKE_SOURCE_DIR}/src/ui/*.cpp"
)
file(GLOB_RECURSE HEADERS
	"${CMAKE_SOURCE_DIR}/src/*.h"
	"${CMAKE_SOURCE_DIR}/src/ai/*.h"
	"${CMAKE_SOURCE_DIR}/src/audio/*.h"
	"${CMAKE_SOURCE_DIR}/src/engine/*.h"
	"${CMAKE_SOURCE_DIR}/src/entity/*.h"
	"${CMAKE_SOURCE_DIR}/src/model/*.h"
	"${CMAKE_SOURCE_DIR}/src/opengl-helper/*.h"
	"${CMAKE_SOURCE_DIR}/src/physics/*.h"
	"${CMAKE_SOURCE_DIR}/src/render/*.h"
	"${CMAKE_SOURCE_DIR}/src/ui/*.h"
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${SOURCES} ${HEADERS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/src")
# Create a filter/folder in Visual Studio for the source and header files.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" PREFIX "Header Files" FILES ${HEADERS})

file(GLOB_RECURSE RESOURCES
	"${CMAKE_SOURCE_DIR}/rsc/*.glsl"
	"${CMAKE_SOURCE_DIR}/rsc/*.jpeg"
	"${CMAKE_SOURCE_DIR}/rsc/*.png"
	"${CMAKE_SOURCE_DIR}/rsc/*.wav"
	"${CMAKE_SOURCE_DIR}/rsc/*.otf"
)

# Don't include Wavefront obj files as resource files because visual studio thinks they
# are obj files meant for the linker. Setting their VS metadata ExclududedFromProperty does
# not work on .obj files. This bug has been reported.
# https://discourse.cmake.org/t/possible-bug-setting-vs-settings-excludedfrombuild-true-for-wavefornt-obj-files/2494
file(GLOB_RECURSE RESOURCES_OBJ
	"${CMAKE_SOURCE_DIR}/rsc/*.obj"
)
set_property(SOURCE ${RESOURCES_OBJ} PROPERTY HEADER_FILE_ONLY True)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${RESOURCES} ${RESOURCES_OBJ})
# Create a filter/folder in Visual Studio for the resource files.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/rsc" PREFIX "Resource Files" FILES ${RESOURCES} ${RESOURCES_OBJ})

# Dear ImGui does not have its own CMakeLists.txt files so the simplest soution is to just
# add the required files to the main executable.
set(IMGUI_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dep/imgui")
set(IMGUI_BACKEND_DIR "${IMGUI_ROOT_DIR}/backends")

set(IMGUI_SOURCES
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
	"${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.h")
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${IMGUI_SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${IMGUI_ROOT_DIR}" "${IMGUI_BACKEND_DIR}")

# Force the use of GLAD for ImGUI. If not definied then ImGUI will default to GLEW if
# the header files are include. See dep/imgui/backends/imgui_impl_opengl3.h.
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)

# The source_group command does not work if placed in dep/CMakeLists.txt but does if it is placed here.
source_group("ImGui" FILES ${IMGUI_SOURCES})


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

	# Copy PhysX dlls to the executable directory. Hardcode directory for VS 2019.
	set(PHYSX_DLL "${CMAKE_BINARY_DIR}/dep/physx/bin/win.x86_64.vc142.mt/$<CONFIG>")
	add_custom_command (
		TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${PHYSX_DLL} ${EXEC_DIR}
		DEPENDS ${PHYSX_DLL}
		COMMENT "Copying ${PHYSX_DLL} => ${EXEC_DIR}"
	)

	# Copy freetype.dll for ftgl.
	set(FREETYPE_DLL "${CMAKE_SOURCE_DIR}/dep/freetype-windows-binaries/release\ dll/win64")
	add_custom_command (
		TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${FREETYPE_DLL} ${EXEC_DIR}
		DEPENDS ${FREETYPE_DLL}
		COMMENT "Copying ${FREETYPE_DLL} => ${EXEC_DIR}"
	)
endif()

add_subdirectory(dep)

# Copy resource folders to build directory.
# We don't want to copy if we're building in the source dir.
# Note that symbolic linking is not availible on windows.
if (NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
	set (RSC_DIR "${CMAKE_SOURCE_DIR}/rsc")
	set (RSC_DEST "${CMAKE_BINARY_DIR}/rsc")
    add_custom_command (
        TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${RSC_DIR} ${RSC_DEST}
		DEPENDS ${RSC_DEST}
		COMMENT "Copying ${RSC_DIR} => ${RSC_DEST}"
    )
endif()
