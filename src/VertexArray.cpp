#include <glad/glad.h>
#include "VertexArray.h"

// Anonymouse namespace
namespace
{
	int sumArray(int start, int end, const int array[]) 
	{
		int sum = 0;
		for (int i = start; i < end; i++)
		{
			sum += array[i];
		}
		return sum;
	}
}

VertexArray::VertexArray(const int vertexComponentsCount[], size_t vertexComponentsSize, const float buffer[], size_t bufferSize)
{
    glGenBuffers(1, &vertexBufferId); // gen buffer and store id in VBO
	glGenVertexArrays(1, &id);
    
    glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER,  bufferSize * sizeof(float), buffer, GL_STATIC_DRAW);

    const int totalComponents = sumArray(0, vertexComponentsSize, vertexComponentsCount);
    const int stride = totalComponents * sizeof(float);

    for(unsigned int i = 0; i < vertexComponentsSize; i++)
    {
        int components = vertexComponentsCount[i];
        int offset = sumArray(0, i, vertexComponentsCount);

        glVertexAttribPointer(i, components, GL_FLOAT, GL_FALSE, stride, (void*)(offset * sizeof(float)));
	    glEnableVertexAttribArray(i);    
    }


	// unbind VAO, VBO, and EBO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
	glDeleteBuffers(1, &vertexBufferId);
}       

unsigned int VertexArray::getId() const
{
	return id;
}

void VertexArray::bind() const
{
	glBindVertexArray(id);
}
