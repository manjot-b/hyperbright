#pragma once

#include <string>

#include "Shader.h"
#include "VertexArray.h"
#include "ObjModel.h"

class Model
{
	public:
		Model(const std::string &objPath, bool hasTexture, bool hasNormal, Shader shader);
		~Model();
		void draw() const;

	private:
		Shader shader;
		VertexArray *vertexArray;
		unsigned int vertexCount;

		void extractVertexData(std::vector<float> &buffer, ObjModel &obj, bool hasTexture, bool hasNormal);
};
