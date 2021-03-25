#pragma once

#include "opengl-helper/CubeMap.h"
#include "opengl-helper/VertexArray.h"
#include "render/IRenderable.h"

namespace hyperbright {
namespace entity {
class SkyBox : public render::IRenderable
{
public:
	SkyBox();
	void render() const;

private:
	unsigned int indicesCount;
	std::unique_ptr<openGLHelper::VertexArray> vertexArray;

	openGLHelper::CubeMap cubeMap;
	void sendSharedShaderUniforms(const glm::mat4& projection, const glm::mat4& view, const glm::vec3&) const;
};
}   // namespace render
}   // namespace hyperbright