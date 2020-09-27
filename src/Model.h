#pragma once

#include <string>
#include <glm/glm.hpp>

#include "Shader.h"
#include "VertexArray.h"
#include "ObjModel.h"

class Model
{
	public:
		Model(const std::string &objPath, bool hasTexture, bool hasNormal, Shader shader);
		~Model();
		void draw() const;
		void update();
		void rotate(const glm::vec3 &rotate);
		void scale(float scale);

	private:
		Shader shader;
		VertexArray *vertexArray;
		unsigned int vertexCount;
		glm::mat4 modelMatrix;
		glm::vec3 m_rotate;			// how much to rotate along each axis
		float m_scale;				// scale to apply to model
		glm::vec3 m_translation;	// translation vector

		struct BoundingBox
		{
			float x, y, z, width, height, depth;
		} boundingBox;

		void extractVertexData(std::vector<float> &buffer, ObjModel &obj, bool hasTexture, bool hasNormal);
		void calcBoundingBox(ObjModel &obj);

};
