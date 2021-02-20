#pragma once

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <optional>

#include "Mesh.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

using InstanceColorsPtr = std::shared_ptr<std::vector<glm::vec4>>;

class Model : public Renderer::IRenderable
{
	public:
		Model(const std::string& objPath,
			const char* id,
			std::shared_ptr<Texture> texture,
			std::optional<glm::vec4> color,
			InstanceModelMatricesPtr instanceModelMatrices = nullptr,
			InstanceColorsPtr instanceColors = nullptr,
			bool fitToViewPort = false);
		Model(const Model& model);
		~Model();

		void render(const Shader& shader) const;
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
		std::optional<glm::vec4> getColor() const;
		const BoundingBox& getBoundingBox() const;
		const char* getId() const { return id; };
		void setInstanceModelMatrices(InstanceModelMatricesPtr instanceModelMatrices);
		void setInstanceColors(InstanceColorsPtr instanceColors);

		bool shouldRender = false;

	private:
		std::vector<std::unique_ptr<Mesh>> meshes;

		const char* id;
		InstanceModelMatricesPtr m_instanceModelMatrices;
		InstanceColorsPtr m_instanceColors;

		BoundingBox boundingBox;
		glm::mat4 modelMatrix;
		glm::vec3 m_rotate;			// how much to rotate along each axis
		glm::vec3 m_scale;			// scale to apply to model
		glm::vec3 m_translation;	// translation vector
		glm::vec3 m_position;
		std::optional<glm::vec4> m_color;	// Don't need to set if instancedColors is set.

		std::shared_ptr<Texture> m_texture;

		void extractDataFromNode(const aiScene* scene, const aiNode* node);
		void computeBoundingBox();
		void scaleToViewport();
		
};
