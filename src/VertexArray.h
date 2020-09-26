#pragma once

#include <cstddef>

class VertexArray
{
	public:
        /*
           parameters:
				vertexComponentsCount:
					The number of components for each type of data for the vertex 
                   	should be at every element. If the first 3 elements in buffer are the vertex
                    positions, the next 2 are texture coordinates, and the last 3 are RGB colors 
                    for the vertex then vertexComponents = {3, 2, 3}
				vertCompSize:
					Size of vertexComponents
                buffer:
					The buffer that holds all the vertex data, eg position, color, texture coords etc...
                bufferSize:
					The size of the buffer
        */
		VertexArray(const int vertexComponentsCount[],
				size_t vertexComponentsSize,
				const float buffer[],
				size_t bufferSize);
		~VertexArray();
		unsigned int getId() const;
		void bind() const;

	private:
		unsigned int id;
		unsigned int vertexBufferId;
};
