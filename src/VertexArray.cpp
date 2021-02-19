#include "VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) :
	vertexBufferId(0), elementBufferId(0), instanceBufferId(0)
{
	initVertexArray(vertices, indices);
}

VertexArray::VertexArray(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::mat4>& modelMatrices) :
	VertexArray::VertexArray(vertices, indices)
{
	initInstanceArray(modelMatrices);
}

void VertexArray::initVertexArray(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &vertexBufferId); // gen buffer and store id in VBO
	glGenBuffers(1, &elementBufferId);
	glGenVertexArrays(1, &id);

	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture));


	// unbind VAO, VBO, and EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VertexArray::initInstanceArray(const std::vector<glm::mat4>& modelMatrices)
{
	if (!instanceBufferId)
	{
		glGenBuffers(1, &instanceBufferId);
	}
		
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, instanceBufferId);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);

	// Assume that the vertex attribute starts at position 3.
	// A glm::mat4 is made up of 4 glm::vec4s. We need to create an attribute pointer
	// for each glm::vec4.
	size_t vec4Size = sizeof(glm::vec4);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	// Update the data in the vertex attribute pointer for each instance.
	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteBuffers(1, &elementBufferId);

	if (instanceBufferId)
		glDeleteBuffers(1, &instanceBufferId);
}       

void VertexArray::setInstanceModelMatrices(const std::vector<glm::mat4>& modelMatrices)
{
	initInstanceArray(modelMatrices);
}

unsigned int VertexArray::getId() const
{
	return id;
}

void VertexArray::bind() const
{
	glBindVertexArray(id);
}
