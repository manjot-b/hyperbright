#include <glad/glad.h>
#include <iostream>

#include "Model.h"

Model::Model(const std::string &objPath, bool hasTexture, bool hasNormal, Shader shader) :
	shader(shader)
{
	ObjModel obj = ObjModel();
	obj.load(objPath);
	vertexCount = obj.triangleCount() * 3;

	std::vector<float> vertexBuffer;
	extractVertexData(vertexBuffer, obj, hasTexture, hasNormal);	

	std::vector<int> vertexComponents;
	vertexComponents.push_back(3);	// position components
	if (hasTexture)
	{
		vertexComponents.push_back(2);
	}
	if (hasNormal)
	{
		vertexComponents.push_back(3);
	}

//	float tmp[] = {
//		-0.5, -0.5, 0,
//		0.0, 0.5, 0,
//		0.5, -0.5, 0
//	};
//	int tmpComp = 3;
//	vertexArray = new VertexArray(&tmpComp, 1, tmp, sizeof(tmp)/sizeof(float));
	vertexArray = new VertexArray(vertexComponents.data(),
			vertexComponents.size(),
			vertexBuffer.data(),
			vertexBuffer.size());
}

Model::~Model() 
{
	delete vertexArray;
}

void Model::extractVertexData(std::vector<float> &buffer, ObjModel &obj, bool hasTexture, bool hasNormal)
{
	for (unsigned int tri = 0; tri < obj.triangleCount(); tri++)
	{
		const Triangle &triangle = obj[tri];

		for(unsigned int vert = 0; vert < 3; vert++)
		{
			buffer.push_back(triangle.vertex[vert].pos.x);
			buffer.push_back(triangle.vertex[vert].pos.y);
			buffer.push_back(triangle.vertex[vert].pos.z);
			
			if (hasTexture)
			{
				buffer.push_back(triangle.vertex[vert].tex.s);
				buffer.push_back(triangle.vertex[vert].tex.t);
			}

			if (hasNormal)
			{
				buffer.push_back(triangle.vertex[vert].norm.x);
				buffer.push_back(triangle.vertex[vert].norm.y);
				buffer.push_back(triangle.vertex[vert].norm.z);
			}
		}
	}
}

void Model::draw() const
{
	shader.use();
	vertexArray->bind();
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
//	glDrawArrays(GL_TRIANGLES, 0, 3);
}
