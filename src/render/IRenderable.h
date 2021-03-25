#pragma once

#include <memory>

#include "opengl-helper/Shader.h"

namespace hyperbright {
namespace render {
class IRenderable
{
public:
	IRenderable();
	IRenderable(const std::shared_ptr<openGLHelper::Shader>& shader);
	bool operator==(const IRenderable* other);
	virtual void render() const = 0;
	//virtual void renderShadow(const std::shared_ptr<openGLHelper::Shader>& shadowShader) const = 0;
	const std::shared_ptr<openGLHelper::Shader>& getShader() const;
	void setShader(const std::shared_ptr<openGLHelper::Shader>& shader);
	virtual void sendSharedShaderUniforms(const glm::mat4& projection, const glm::mat4& view, const glm::vec3& cameraPos) const;
protected:
	std::shared_ptr<openGLHelper::Shader> _shader;
};
}   // namespace render
}   // namespace hyperbright