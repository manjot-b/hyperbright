#include "SkyBox.h"

namespace hyperbright {
namespace entity {
	SkyBox::SkyBox() :
		IRenderable(std::make_shared<openGLHelper::Shader>("rsc/shaders/skybox_vertex.glsl", "rsc/shaders/skybox_fragment.glsl")),
		cubeMap("rsc/images/night_skybox.png")
	{
		_shader->link();
		const float scale = 1000.f;	// make sure this is within the perspective far plane.
		std::vector<float> vertices = {
		-0.5, -0.5, 0.5,
		0.5, -0.5, 0.5,
		-0.5, 0.5, 0.5,
		0.5, 0.5, 0.5,
		-0.5, 0.5, -0.5,
		0.5, 0.5, -0.5,
		-0.5, -0.5, -0.5,
		0.5, -0.5, -0.5
		};

		for (auto& v : vertices)
			v *= scale;

		std::vector<unsigned int> indices = {
			0, 1, 2,
			2, 1, 3,
			2, 3, 4,
			4, 3, 5,
			4, 5, 6,
			6, 5, 7,
			6, 7, 0,
			0, 7, 1,
			1, 7, 3,
			3, 7, 5,
			6, 0, 4,
			4, 0, 2
		};

		indicesCount = indices.size();

		vertexArray = std::make_unique<openGLHelper::VertexArray>(vertices, indices);
	}

	void SkyBox::render() const
	{
		cubeMap.bind(GL_TEXTURE0);
		vertexArray->bind();
		glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	/*
	 Override IRenderable function because we don't need the camera position for anything and we need to alter the view matrix.
	*/
	void SkyBox::sendSharedShaderUniforms(const glm::mat4& projection, const glm::mat4& view, const glm::vec3&, const glm::mat4&) const
	{
		glm::mat4 noTransView = glm::mat4(glm::mat3(view));

		_shader->setUniformMatrix4fv("perspective", projection);
		_shader->setUniformMatrix4fv("view", noTransView);
	}
}   // namespace render
}   // namespace hyperbright