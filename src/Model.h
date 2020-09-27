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
		void rotate(const glm::vec3 &rotate);

	private:
		Shader shader;
		VertexArray *vertexArray;
		unsigned int vertexCount;
		glm::mat4 modelMatrix;

		void extractVertexData(std::vector<float> &buffer, ObjModel &obj, bool hasTexture, bool hasNormal);
};
