#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "Model.h"

Model::Model(const std::string &objPath, bool hasTexture, bool hasNormal, Shader shader) :
	shader(shader), modelMatrix(1.0f), m_rotate(0), m_scale(1), m_translation(0)
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

	vertexArray = new VertexArray(vertexComponents.data(),
			vertexComponents.size(),
			vertexBuffer.data(),
			vertexBuffer.size());


	calcBoundingBox(obj);
	// Scale model so that the longest side of its BoundingBox
	// has a length of 1.
	m_scale = 1 / glm::max(boundingBox.width, glm::max(boundingBox.height, boundingBox.depth));
	update();
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

/**
 * Draws the model. Remember to update() the model first.
 */
void Model::draw() const
{
//	std::cout << glm::to_string(modelMatrix) << std::endl;
	shader.use();
	shader.setUniformMatrix4fv("model", modelMatrix);
	vertexArray->bind();
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

/**
 * Updates the model matrix. Should be called before draw().
 */
void Model::update()
{
	// Apply transformations
	modelMatrix = glm::translate(modelMatrix, m_translation);
	modelMatrix = modelMatrix * glm::eulerAngleXYZ(m_rotate.x, m_rotate.y, m_rotate.z);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(m_scale, m_scale, m_scale));

	// Reset transformation values
	m_translation = glm::vec3(0);
	m_rotate = glm::vec3(0);
	m_scale = 1;
}

/**
 * Rotates the model along each x,y, and z axis at the specified angles.
 * Input parameters are to be in radians. Remember to use the right-hand rule.
 */
void Model::rotate(const glm::vec3 &rotate)
{
	m_rotate = rotate;
}

void Model::scale(const float scale)
{
	m_scale = scale;
}

void Model::calcBoundingBox(ObjModel &obj)
{
	float minX = obj[0].vertex[0].pos.x;
	float maxX = minX;
	float minY = obj[0].vertex[0].pos.y;
	float maxY = minY;
	float minZ = obj[0].vertex[0].pos.z;
	float maxZ = minZ;

	for (unsigned int tri = 0; tri < obj.triangleCount(); tri++)
	{
		const Triangle &triangle = obj[tri];

		for(unsigned int vert = 0; vert < 3; vert++)
		{
			minX = glm::min(minX, triangle.vertex[vert].pos.x);
			maxX = glm::max(maxX, triangle.vertex[vert].pos.x);
			
			minY = glm::min(minY, triangle.vertex[vert].pos.y);
			maxY = glm::max(maxY, triangle.vertex[vert].pos.y);
			
			minZ = glm::min(minZ, triangle.vertex[vert].pos.z);
			maxZ = glm::max(maxZ, triangle.vertex[vert].pos.z);
			
		}
	}
	boundingBox.x = minX;
	boundingBox.y = minY;
	boundingBox.z = maxZ;
	boundingBox.width = abs(maxX - minX);
	boundingBox.height = abs(maxY - minY);
	boundingBox.depth = abs(maxZ - minZ);
}
