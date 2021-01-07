# It is recommended to add each .cpp and .h file manually so that the
# generated build system knows when to ask CMake to regenerate the build system.
set(SOURCES
    "Camera.h"
    "Camera.cpp"
    "glad.cpp"
    "main.cpp"
    "Mesh.h"
    "Mesh.cpp"
    "Model.h"
    "Model.cpp"
    "Renderer.h"
    "Renderer.cpp"
    "Shader.h"
    "Shader.cpp"
    "Texture.h"
    "Texture.cpp"
    "Vertex.h"
    "VertexArray.h"
    "VertexArray.cpp"
)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${SOURCES})
