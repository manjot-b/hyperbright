#include "Model.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>

namespace hyperbright {
namespace model {
Model::Model(const std::string& objPath,
	const std::string& id,
	const std::shared_ptr<openGLHelper::Shader>& shader,
	std::shared_ptr<openGLHelper::Texture> texture,
	InstanceModelMatricesPtr instanceModelMatrices,
	InstanceColorsPtr instanceColors,
	bool fitToViewPort) : IRenderable(shader),
	modelMatrix(1.0f), m_rotate(0), m_scale(1), m_translation(0), id(id), m_texture(texture),
	m_position(.0f), m_instanceModelMatrices(instanceModelMatrices), m_instanceColors(instanceColors)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(objPath,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals);
	if (!scene)
	{
		std::cerr <<  "Error loading " << objPath << ".\n" << importer.GetErrorString() << std::endl;
	}

	extractDataFromNode(scene, scene->mRootNode);

	if (m_instanceColors)
	{
		for (auto& mesh : meshes)
		{
			mesh->setInstanceColors(*m_instanceColors); 
		}
	}

	computeBoundingBox();

	if (fitToViewPort)
	{
		scaleToViewport();
	}
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
		meshes.push_back(std::make_unique<Mesh>(scene, mesh, m_instanceModelMatrices));
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
void Model::render() const
{
	
	bool hasTexture = m_texture != nullptr;
	bool isInstance = m_instanceModelMatrices != nullptr;
	bool isInstanceColor = m_instanceColors != nullptr;

	_shader->setUniform1i("hasTexture", hasTexture);
	_shader->setUniform1i("isInstance", isInstance);
	_shader->setUniform1i("isInstanceColor", isInstanceColor);
	
	_shader->setUniformMatrix4fv("model", modelMatrix);

	if (m_texture)
	{
		m_texture->bind(GL_TEXTURE0);
	}

	for(auto &mesh : meshes)
	{
		_shader->setUniform1i("shadingModel", static_cast<int>(mesh->material.shadingModel));
		_shader->setUniform4fv("color", mesh->material.color);	// Not used if model has instanceColors
		_shader->setUniform1f("diffuseCoeff", mesh->material.diffuse);
		_shader->setUniform1f("specularCoeff", mesh->material.specular);
		_shader->setUniform1f("shininess", mesh->material.shininess);
		_shader->setUniform1i("isEmission", mesh->material.isEmission);
		_shader->setUniform1f("reflectiveStrength", mesh->material.reflectiveStrength);
		_shader->setUniform1f("roughness", mesh->material.roughness);
		_shader->setUniform3fv("fresnel", mesh->material.fresnel);
		_shader->setUniform1i("useBeckmann", mesh->material.useBeckmann);
		_shader->setUniform1i("useGGX", mesh->material.useGGX);

		unsigned int count = m_instanceModelMatrices ? m_instanceModelMatrices->size() : 0;

		mesh->render(count);
	}
}

void Model::renderShadow(const std::shared_ptr<openGLHelper::Shader>& shadowShader) const
{
	bool isInstance = m_instanceModelMatrices != nullptr;

	shadowShader->setUniform1i("isInstance", isInstance);
	shadowShader->setUniformMatrix4fv("model", modelMatrix);

	for (auto& mesh : meshes)
	{
		unsigned int count = m_instanceModelMatrices ? m_instanceModelMatrices->size() : 0;
		mesh->render(count);
	}
}

/**
 * Updates the model matrix and position. Should be called before draw().
 */
void Model::update()
{
	// Apply transformations
	modelMatrix = glm::translate(modelMatrix, m_translation);
	modelMatrix = modelMatrix * glm::eulerAngleXYZ(m_rotate.x, m_rotate.y, m_rotate.z);
	modelMatrix = glm::scale(modelMatrix, m_scale);

	m_position = modelMatrix * glm::vec4(m_position, 1.f);

	// Reset transformation values
	m_translation = glm::vec3(0);
	m_rotate = glm::vec3(0);
	m_scale = glm::vec3(1);
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
	this->scale(glm::vec3(scale));
}

void Model::scale(const glm::vec3& scale)
{
	m_scale = scale;
}

void Model::computeBoundingBox()
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
}

/**
 * Iterate over every mesh to calculate the bounding box of the whole model.
 * Assumes the standard OpenGL viewport of -1 to 1.
 * 
 * Note: The BoundingBox must be calculated first.
 */
void Model::scaleToViewport()
{
	// Scale by the longest edge.
	m_scale = glm::vec3(1 / glm::max(boundingBox.width, glm::max(boundingBox.height, boundingBox.depth)));

	// Put center of bounding at (0, 0, 0).
	float xTrans = -(boundingBox.x + boundingBox.width*0.5f) * m_scale.x;
	float yTrans = -(boundingBox.y + boundingBox.height*0.5f) * m_scale.y;
	float zTrans = -(boundingBox.z + boundingBox.depth*0.5f) * m_scale.z;
	m_translation = glm::vec3(xTrans, yTrans, zTrans);
	update();
}

void Model::setInstanceModelMatrices(InstanceModelMatricesPtr instanceModelMatrices)
{
	m_instanceModelMatrices = instanceModelMatrices;
	for (auto& mesh : meshes)
	{
		mesh->setInstanceModelMatrices(*instanceModelMatrices);
	}
}

void Model::setInstanceColors(InstanceColorsPtr instanceColors)
{
	m_instanceColors = instanceColors;
	for (auto& mesh : meshes)
	{
		mesh->setInstanceColors(*instanceColors);
	}
}

const glm::mat4& Model::getModelMatrix() const { return modelMatrix; }

const std::vector<std::unique_ptr<Mesh>>& Model::getMeshes() const { return meshes; }

const BoundingBox& Model::getBoundingBox() const { return boundingBox; }

void Model::setModelMatrix(const glm::mat4& modelPose) { modelMatrix = modelPose; }

void Model::setColor(const glm::vec4& color)
{
	for (auto& mesh : meshes)
	{
		mesh->material.color = color;
	}
}

void Model::setPosition(const glm::vec3& position) { m_position = position; }

const glm::vec3& Model::getPosition() const { return m_position; }
}	// namespace model
}	// namespace hyperbright