#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Shader.h"
#include "Mesh.h"

class Model
{
	public:
		Model(const std::string &objPath);
		~Model();
		void draw(const Shader& shader) const;
		void update();
		void updateModelMatrix(glm::mat4& modelPose);
		void translate(const glm::vec3& translate);
		void rotate(const glm::vec3 &rotate);
		void scale(float scale);
		bool shouldRender = false;

	private:
		std::vector<std::unique_ptr<Mesh>> meshes;

		BoundingBox boundingBox;
		glm::mat4 modelMatrix;
		glm::vec3 m_rotate;			// how much to rotate along each axis
		float m_scale;				// scale to apply to model
		glm::vec3 m_translation;	// translation vector

		void extractDataFromNode(const aiScene* scene, const aiNode* node);
		void scaleToViewport();
};
