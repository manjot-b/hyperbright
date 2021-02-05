#pragma once

#include <string>
#include <glm/glm.hpp>
#include <memory>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Model
{
	public:
		enum MoveType {
			STATIC,
			DYNAMIC
		};


		Model(const std::string &objPath, MoveType type, std::shared_ptr<Texture> texture);
		~Model();
		void draw(const Shader& shader) const;
		void update();
		void updateModelMatrix(glm::mat4& modelPose);
		void translate(const glm::vec3& translate);
		void rotate(const glm::vec3 &rotate);
		void scale(float scale);

		void setPosition(glm::vec3 position);
		bool isDynamic() { return dynamicObject; }

		std::vector<std::unique_ptr<Mesh>>& getMeshes() { return meshes; }

		glm::vec3 getPosition();

		bool shouldRender = false;

	private:
		std::vector<std::unique_ptr<Mesh>> meshes;

		BoundingBox boundingBox;
		glm::mat4 modelMatrix;
		glm::vec3 m_rotate;			// how much to rotate along each axis
		float m_scale;				// scale to apply to model
		glm::vec3 m_translation;	// translation vector
		glm::vec3 wPosition;

		int const dynamicObject;
		std::shared_ptr<Texture> m_texture;

		void extractDataFromNode(const aiScene* scene, const aiNode* node);
		void scaleToViewport();
};
