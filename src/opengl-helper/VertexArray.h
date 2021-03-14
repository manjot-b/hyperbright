#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>

#include "model/MeshUtils.h"

namespace hyperbright {
namespace openGLHelper {
class VertexArray
{
	public:
        /**
		 * parameters:
		 * 		vertices: The vertex with all of its data.
		 * 		indices: Used to index into vertices allowing triangles to share vertices.
        */
		VertexArray(const std::vector<model::Vertex> &vertices, const std::vector<unsigned int> &indices);
		VertexArray(const std::vector<model::Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<glm::mat4>& modelMatrices);
		VertexArray(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
		~VertexArray();

		void setInstanceModelMatrices(const std::vector<glm::mat4>& modelMatrices);
		void setInstanceColors(const std::vector<glm::vec4>& colors);
		unsigned int getId() const;
		void bind() const;

	private:
		unsigned int id;
		unsigned int vertexBufferId;
		unsigned int elementBufferId;
		unsigned int instanceModelBufferId;
		unsigned int instanceColorBufferId;

		void initVertexArray(const std::vector<model::Vertex>& vertices, const std::vector<unsigned int>& indices);
		template <typename T>
		void initInstanceArray(
			unsigned int& bufferId,
			const std::vector<T>& data,
			unsigned int attribLocation,
			size_t attribSize,
			unsigned int components,
			unsigned int stride,
			GLenum drawType);
};
}	// namespace openGLHelper
}	// namespace hyperbright