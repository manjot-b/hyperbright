#include <glad/glad.h>
#include "VertexArray.h"

VertexArray::VertexArray(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    glGenBuffers(1, &vertexBufferId); // gen buffer and store id in VBO
	glGenBuffers(1, &elementBufferId);
	glGenVertexArrays(1, &id);
    
    glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER,  vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


	// unbind VAO, VBO, and EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteBuffers(1, &elementBufferId);
}       

unsigned int VertexArray::getId() const
{
	return id;
}

void VertexArray::bind() const
{
	glBindVertexArray(id);
}
