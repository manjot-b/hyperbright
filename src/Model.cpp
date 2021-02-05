#include "Model.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>

Model::Model(const std::string &objPath, MoveType type, std::shared_ptr<Texture> texture) :
	 modelMatrix(1.0f), m_rotate(0), m_scale(1), m_translation(0), dynamicObject(type), m_texture(texture)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(objPath,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals);	
	if (!scene)
	{
		std::cerr <<  "Error loading " << objPath << ".\n" << importer.GetErrorString() << std::endl;
	}

	extractDataFromNode(scene, scene->mRootNode);	

	scaleToViewport();
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
 * Assumes the shader is already in use.
 */
void Model::draw(const Shader& shader) const
{
	shader.setUniformMatrix4fv("model", modelMatrix);

	if (m_texture)
	{
		m_texture->bind(GL_TEXTURE0);
	}

	for(auto &mesh : meshes)
	{
		mesh->draw();
	}
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

void Model::updateModelMatrix(glm::mat4& modelPose)
{
	modelMatrix = modelPose;
}

void Model::translate(const glm::vec3& _translate)
{
	m_translation = _translate;
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

void Model::setPosition(glm::vec3 _position)
{
	wPosition = _position;
}

glm::vec3 Model::getPosition()
{
	return wPosition;
}


/**
 * Iterate over every mesh to calculate the bounding box of the whole model.
 * Assumes the standard OpenGL viewport of -1 to 1.
 */
void Model::scaleToViewport()
{
	float minX = meshes[0]->getBoundingBox().x;
	float maxX = meshes[0]->getBoundingBox().x + meshes[0]->getBoundingBox().width;
	float minY = meshes[0]->getBoundingBox().y;
	float maxY = meshes[0]->getBoundingBox().y + meshes[0]->getBoundingBox().height;
	float minZ = meshes[0]->getBoundingBox().z;
	float maxZ = meshes[0]->getBoundingBox().z + meshes[0]->getBoundingBox().depth;

	for (const auto& mesh : meshes)
	{
		minX = glm::min(minX, mesh->getBoundingBox().x);
		maxX = glm::max(maxX, mesh->getBoundingBox().x + mesh->getBoundingBox().width);
		
		minY = glm::min(minY, mesh->getBoundingBox().y);
		maxY = glm::max(maxY, mesh->getBoundingBox().y + mesh->getBoundingBox().height);
		
		minZ = glm::min(minZ, mesh->getBoundingBox().z);
		maxZ = glm::max(maxZ, mesh->getBoundingBox().z + mesh->getBoundingBox().depth);
	}

	boundingBox.x = minX;
	boundingBox.y = minY;
	boundingBox.z = minZ;
	boundingBox.width = glm::abs(maxX - minX);
	boundingBox.height = glm::abs(maxY - minY);
	boundingBox.depth = glm::abs(maxZ - minZ);

	// Scale by the longest edge.
	m_scale = 1 / glm::max(boundingBox.width, glm::max(boundingBox.height, boundingBox.depth));

	// Put center of bounding at (0, 0, 0).
	float xTrans = -(boundingBox.x + boundingBox.width*0.5f) * m_scale;
	float yTrans = -(boundingBox.y + boundingBox.height*0.5f) * m_scale;
	float zTrans = -(boundingBox.z + boundingBox.depth*0.5f) * m_scale;
	m_translation = glm::vec3(xTrans, yTrans, zTrans);
	update();
}

