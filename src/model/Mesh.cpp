#include "Mesh.h"

#include <glad/glad.h>

namespace hyperbright {
namespace model {
Mesh::Mesh(const aiScene* scene, const aiMesh* mesh, const InstanceModelMatricesPtr& instancedModelMatrices)
{
	extractDataFromMesh(scene, mesh);
	if (instancedModelMatrices)
		vertexArray = std::make_unique<openGLHelper::VertexArray>(vertices, indices, *instancedModelMatrices);
	else
		vertexArray = std::make_unique<openGLHelper::VertexArray>(vertices, indices);
	calcBoundingBox();
}

Mesh::~Mesh() {}

/**
 * Fills the buffer with the vertex data from the mesh.
 */
void Mesh::extractDataFromMesh(const aiScene* scene, const aiMesh* mesh)
{
	// First, extract all the vertex data, i.e. position, normal, etc.
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		// A vertex can have up to 8 texture coords but we assume the our models
		// have only 1.
		if (mesh->HasTextureCoords(0))
		{
			vertex.texture.x = mesh->mTextureCoords[0][i].x;
			vertex.texture.y = mesh->mTextureCoords[0][i].y;
		}
		vertices.push_back(vertex);
	}

	// Now, store the indices by iterating over the meshes faces.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (scene->HasMaterials())
	{
		const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		// Each mesh should have 1 material, so set the name of the mesh to the name
		// of the material.
		aiString aiName;
		mat->Get(AI_MATKEY_NAME, aiName);
		name = std::string(aiName.C_Str());

		float gamma = 1 / 2.2f;
		aiColor3D diff;
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, diff);
		std::memcpy(&(material.color), &diff, sizeof(aiColor3D));
		material.color.r = glm::pow(material.color.r, gamma);
		material.color.g = glm::pow(material.color.g, gamma);
		material.color.b = glm::pow(material.color.b, gamma);
		material.color.a = 1.f;

		float shininess;
		mat->Get(AI_MATKEY_SHININESS, shininess);
		material.shininess = shininess < 1.f ? 1.f : shininess;

		material.shadingModel = Material::ShadingModel::PHONG;
		material.diffuse = 1.f;
		material.specular = 1.f;
		material.isEmission = false;
		material.roughness = 1.f;
		material.fresnel = glm::vec3(1.f);
		material.useBeckmann = false;
		material.useGGX = false;
	}
}

/*
 * Draws the mesh to the screen. If instanceCount is greater than 0 then use instancing to
 * draw the mesh. Otherwise, draw only a single mesh.
*/

void Mesh::render(unsigned int instanceCount) const
{
	vertexArray->bind();
	if (instanceCount > 0)
	{
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instanceCount);
	}
	else
	{
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

void Mesh::calcBoundingBox()
{
	float minX = vertices[0].position.x;
	float maxX = minX;
	float minY = vertices[0].position.y;
	float maxY = minY;
	float minZ = vertices[0].position.z;
	float maxZ = minZ;

	for (const auto& vertex : vertices)
	{
		minX = glm::min(minX, vertex.position.x);
		maxX = glm::max(maxX, vertex.position.x);
		
		minY = glm::min(minY, vertex.position.y);
		maxY = glm::max(maxY, vertex.position.y);
		
		minZ = glm::min(minZ, vertex.position.z);
		maxZ = glm::max(maxZ, vertex.position.z);
	}
	boundingBox.x = minX;
	boundingBox.y = minY;
	boundingBox.z = minZ;
	boundingBox.width = glm::abs(maxX - minX);
	boundingBox.height = glm::abs(maxY - minY);
	boundingBox.depth = glm::abs(maxZ - minZ);
}

const BoundingBox& Mesh::getBoundingBox() const
{
	return boundingBox;
}

void Mesh::setInstanceModelMatrices(const std::vector<glm::mat4>& instanceModelMatrices)
{
	vertexArray->setInstanceModelMatrices(instanceModelMatrices);
}

void Mesh::setInstanceColors(const std::vector<glm::vec4>& instanceColors)
{
	vertexArray->setInstanceColors(instanceColors);
}

const std::vector<Vertex>& Mesh::getVertices() const { return vertices; }

const std::vector<unsigned int>& Mesh::getIndices() const { return indices; }

const std::string& Mesh::getName() const { return name; }
}	// namespace model
}	// namespace hyperbright