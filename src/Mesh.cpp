#include <glad/glad.h>

#include "Mesh.h"

Mesh::Mesh(const aiMesh* mesh)
{
	extractDataFromMesh(mesh);
	vertexArray = std::make_unique<VertexArray>(vertices, indices);
	calcBoundingBox();
}

Mesh::~Mesh() {}

/**
 * Fills the buffer with the vertex data from the mesh.
 */
void Mesh::extractDataFromMesh(const aiMesh* mesh)
{
	// First, extract all the vertex data, i.e. position, normal, etc.
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
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
}

void Mesh::draw() const
{
	vertexArray->bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
