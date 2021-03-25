#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VertexArray.h"
#include "opengl-helper/Texture.h"
#include "render/IRenderable.h"

namespace hyperbright {
namespace openGLHelper {

class Quad : public render::IRenderable
{
public:
	Quad(const std::shared_ptr<Shader>& shader, std::shared_ptr<Texture> texture = nullptr);
	virtual void render() const;
	virtual void sendSharedShaderUniforms(const glm::mat4&, const glm::mat4&, const glm::vec3&) const;

	void setModelMatrix(glm::mat4& mat);
	const glm::mat4& getModelMatrix() const;
	void normalizeToViewport(unsigned int width, unsigned int height);

private:
	unsigned int indicesCount;
	glm::mat4 modelMat;
	std::unique_ptr<VertexArray> vertexArray;
	std::shared_ptr<Texture> texture;
};
}	// namespace openGLHelper
}	// namespace hyperbright