#pragma once

#include <glm/glm.hpp>

#include <vector>

#include "Vertex.h"

class VertexArray
{
	public:
        /**
		 * parameters:
		 * 		vertices: The vertex with all of its data.
		 * 		indices: Used to index into vertices allowing triangles to share vertices.
        */
		VertexArray(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
		VertexArray(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::mat4>& modelMatrices);
		~VertexArray();

		void setInstanceModelMatrices(const std::vector<glm::mat4>& modelMatrices);
		unsigned int getId() const;
		void bind() const;

	private:
		unsigned int id;
		unsigned int vertexBufferId;
		unsigned int elementBufferId;
		unsigned int instanceBufferId;

		void initVertexArray(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
		void initInstanceArray(const std::vector<glm::mat4>& modelMatrices);
};
