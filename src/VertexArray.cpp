#include "VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices) :
	vertexBufferId(0), elementBufferId(0), instanceModelBufferId(0), instanceColorBufferId(0)
{
	initVertexArray(vertices, indices);
}

VertexArray::VertexArray(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::mat4>& modelMatrices) :
	VertexArray::VertexArray(vertices, indices)
{
	setInstanceModelMatrices(modelMatrices);
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

/*
Generates an instance buffer.
	Parameters:
		data: A vector of T containing the data. T could be glm::vec4 or even glm::mat4 for example.
		attribLocation: The starting location for the attribute pointer.
		attribSize: How many attribute locations a T takes up. GLSL allows for a max size of vec4 for a single attribute location.
		components: The number of components in a single attribute location.
		stride: The size in bytes between consecutive vertex attributes. A value of 0 means tightly packed. For exapmle, a tightly packed array of mat4
				would have a stride of sizeof(glm::vec4), because a mat4 is made up of 4 vec4s.
		bufferId: The bufferId to store the buffer into. If it already exists then ovewrite the data in it.
*/
template <typename T>
void VertexArray::initInstanceArray(const std::vector<T>& data,
	unsigned int attribLocation,
	size_t attribSize,
	unsigned int components,
	unsigned int stride,
	unsigned int& bufferId)
{
	if (!bufferId)
	{
		glGenBuffers(1, &bufferId);
	}
		
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);

	// We need to create as many attribute pointers as requested.
	unsigned int count = 0;
	for (unsigned int i = attribLocation; i < attribLocation + attribSize; i++, count++)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, components, GL_FLOAT, GL_FALSE, stride, (void*)(count * sizeof(GL_FLOAT) * components));
		// Update the data in the vertex attribute pointer for each instance.
		glVertexAttribDivisor(i, 1);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
	glDeleteBuffers(1, &vertexBufferId);
	glDeleteBuffers(1, &elementBufferId);

	if (instanceModelBufferId)
		glDeleteBuffers(1, &instanceModelBufferId);
}       

void VertexArray::setInstanceModelMatrices(const std::vector<glm::mat4>& modelMatrices)
{
	// Assume that the vertex attribute starts at position 3.
	initInstanceArray(modelMatrices, 3, 4, 4, sizeof(glm::mat4), instanceModelBufferId);
}

void VertexArray::setInstanceColors(const std::vector<glm::vec3>& colors)
{
	initInstanceArray(colors, 7, 1, 4, 0, instanceColorBufferId);
}

unsigned int VertexArray::getId() const
{
	return id;
}

void VertexArray::bind() const
{
	glBindVertexArray(id);
}
