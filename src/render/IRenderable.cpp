#include "IRenderable.h"

namespace hyperbright {
namespace render {

IRenderable::IRenderable() {}

IRenderable::IRenderable(const std::shared_ptr<openGLHelper::Shader>& shader) : _shader(shader) {}

bool IRenderable::operator==(const IRenderable* other)
{
	if (this == other) return true;
	return false;
}

void IRenderable::renderMiniMap() const
{
	render();
}

const std::shared_ptr<openGLHelper::Shader>& IRenderable::getShader() const
{
	return _shader;
};
void IRenderable::setShader(const std::shared_ptr<openGLHelper::Shader>& shader)
{
	_shader = shader;
}

/*
 The default shader accepts these. This function can be overriden if an IRenderable uses a custom shader.
*/
void IRenderable::sendSharedShaderUniforms(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& cameraPos, const glm::mat4& lightSpace) const
{
	_shader->setUniformMatrix4fv("perspective", projection);
	_shader->setUniformMatrix4fv("view", view);
	_shader->setUniform3fv("cameraPos", cameraPos);
	_shader->setUniformMatrix4fv("lightSpace", lightSpace);
}

}   // namespace render
}   // namespace hyperbright