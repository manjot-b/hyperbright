#include <glad/glad.h>

#include "Mesh.h"

Mesh::Mesh(const aiMesh* mesh)
{
	extractDataFromMesh(mesh);
	vertexArray = new VertexArray(vertices, indices);
}

Mesh::~Mesh()
{
	delete vertexArray;
}

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
