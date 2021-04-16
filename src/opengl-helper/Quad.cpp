#include "Quad.h"

#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "model/MeshUtils.h"

namespace hyperbright {
namespace openGLHelper {

Quad::Quad(const std::shared_ptr<Shader>& shader, std::shared_ptr<Texture> texture) : IRenderable(shader),
	texture(texture), width(1.f), height(1.f), modelMat(1.f)
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
void Quad::sendSharedShaderUniforms(const glm::mat4&, const glm::mat4&, const glm::vec3&, const glm::mat4&) const
{
}

void Quad::setModelMatrix(glm::mat4& mat) { modelMat = mat; }
const glm::mat4& Quad::getModelMatrix() const { return modelMat; }

/*
 Normalizes the length and width of the quad based on the viewport dimensions
 so that a square is rendered.
*/
void Quad::normalizeToViewport(unsigned int width, unsigned int height)
{
	float yScale = ((float)width) / height;
	modelMat = glm::scale(glm::mat4(1.f), glm::vec3(1.f, yScale, 1.f));
	this->width = 1.f;
	this->height = yScale;
}

void Quad::scale(float scale)
{
	this->scale(glm::vec2(scale));
}

void Quad::scale(const glm::vec2 scale)
{
	modelMat = glm::scale(modelMat, glm::vec3(scale, 1.f));
	width *= scale.x;
	height *= scale.y;
}

void Quad::translate(glm::vec2 trans)
{
	modelMat = glm::translate(modelMat, glm::vec3(trans, 0.f));
}

float Quad::getWidth() const { return width; }
float Quad::getHeight() const { return height; }
void Quad::setTexture(std::shared_ptr<Texture> tex) { texture = tex; }

}	// namespace openGLHelper
}	// namespace hyperbright