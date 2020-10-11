#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>

#include "Model.h"

Model::Model(const std::string &objPath, const Shader& shader) :
	shader(shader), modelMatrix(1.0f), m_rotate(0), m_scale(1), m_translation(0)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(objPath,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals);	
	if (!scene)
	{
		std::cerr <<  "Error loading " << objPath << ".\n" << importer.GetErrorString() << std::endl;
	}

	extractDataFromNode(scene, scene->mRootNode);	

//	calcBoundingBox(obj);
	// Scale model so that the longest side of its BoundingBox
	// has a length of 1.
//	m_scale = 1 / glm::max(boundingBox.width, glm::max(boundingBox.height, boundingBox.depth));
//	update();
}

Model::~Model() {}

/**
 * Recursively process each node by first processing all meshes of the current node,
 * then repeating the process for all children nodes.
 */
void Model::extractDataFromNode(const aiScene* scene, const aiNode* node)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// aiNode contains indicies to index the objects in aiScene.
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(std::make_unique<Mesh>(mesh));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		// process all children nodes.
		extractDataFromNode(scene, node->mChildren[i]);
	}

}


/**
 * Draws the model. Remember to update() the model first.
 */
void Model::draw() const
{
	shader.use();
	shader.setUniformMatrix4fv("model", modelMatrix);

	for(auto &mesh : meshes)
	{
		mesh->draw();
	}
	glUseProgram(0);
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
/*
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
*/
