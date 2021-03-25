#include "Quad.h"

#include <vector>

#include "model/MeshUtils.h"

namespace hyperbright {
namespace openGLHelper {

Quad::Quad(const std::shared_ptr<Shader>& shader, std::shared_ptr<Texture> texture) : IRenderable(shader),
	texture(texture), modelMat(1.f)
{
	// A quad of length 1 for each side.
	std::vector<model::Vertex> vertices = {
		// position					 normal			  texture
		{glm::vec3(-.5f, .5f, 0.f),  glm::vec3(0.f), glm::vec2(0.f, 1.f)},
		{glm::vec3(-.5f, -.5f, 0.f), glm::vec3(0.f), glm::vec2(0.f, 0.f)},
		{glm::vec3(.5f, -.5f, 0.f),  glm::vec3(0.f), glm::vec2(1.f, 0.f)},
		{glm::vec3(.5f, .5f, 0.f),   glm::vec3(0.f), glm::vec2(1.f, 1.f)},
	};

	std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 0, 3
	};

	indicesCount = indices.size();

	vertexArray = std::make_unique<VertexArray>(vertices, indices);
}

void Quad::render() const
{
	vertexArray->bind();
	if (texture)
		texture->bind(GL_TEXTURE0);

	_shader->setUniformMatrix4fv("model", modelMat);

	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


/*
 A 2d quad does not need any of these uniforms to be set.
*/
void Quad::sendSharedShaderUniforms(const glm::mat4&, const glm::mat4&, const glm::vec3&) const
{
}

void Quad::setModelMatrix(glm::mat4& mat) { modelMat = mat; }
const glm::mat4& Quad::getModelMatrix() const { return modelMat; }

}	// namespace openGLHelper
}	// namespace hyperbright