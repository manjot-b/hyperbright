#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <optional>

#include "Mesh.h"
#include "opengl-helper/Shader.h"
#include "opengl-helper/Texture.h"
#include "render/Renderer.h"

namespace hyperbright {
namespace model {
using InstanceColorsPtr = std::shared_ptr<std::vector<glm::vec4>>;

class Model : public render::Renderer::IRenderable
{
	public:
		Model(const std::string& objPath,
			const std::string& id,
			const std::shared_ptr<openGLHelper::Shader>& shader,
			std::shared_ptr<openGLHelper::Texture> texture,
			InstanceModelMatricesPtr instanceModelMatrices = nullptr,
			InstanceColorsPtr instanceColors = nullptr,
			bool fitToViewPort = false);
		Model(const Model& model) = delete;		// Multiple models may end up with same VAO.
		~Model();

		void render() const;
		void update();
		void translate(const glm::vec3& translate);
		void rotate(const glm::vec3 &rotate);
		void scale(float scale);
		void scale(const glm::vec3& scale);


		const std::vector<std::unique_ptr<Mesh>>& getMeshes() const;
		void setModelMatrix(const glm::mat4& modelPose);
		const glm::mat4& getModelMatrix() const;
		void setPosition(const glm::vec3& position);
		const glm::vec3& getPosition() const;
		void setColor(const glm::vec4& color);
		const BoundingBox& getBoundingBox() const;
		const std::string& getId() const { return id; };
		void setInstanceModelMatrices(InstanceModelMatricesPtr instanceModelMatrices);
		void setInstanceColors(InstanceColorsPtr instanceColors);

		bool shouldRender = false;

	private:
		std::vector<std::unique_ptr<Mesh>> meshes;

		std::string id;
		InstanceModelMatricesPtr m_instanceModelMatrices;
		InstanceColorsPtr m_instanceColors;

		BoundingBox boundingBox;
		glm::mat4 modelMatrix;
		glm::vec3 m_rotate;			// how much to rotate along each axis
		glm::vec3 m_scale;			// scale to apply to model
		glm::vec3 m_translation;	// translation vector
		glm::vec3 m_position;

		std::shared_ptr<openGLHelper::Texture> m_texture;

		void extractDataFromNode(const aiScene* scene, const aiNode* node);
		void computeBoundingBox();
		void scaleToViewport();
		
};
}	// namespace model
}	// namespace hyperbright