#pragma once

#include <assimp/scene.h>

#include <vector>
#include <memory>

#include "Vertex.h"
#include "VertexArray.h"

struct BoundingBox
{
	float x, y, z, width, height, depth;
};

class Mesh
{
	public:
		Mesh(const aiMesh* mesh);
		~Mesh();
		void draw() const;
		void extractDataFromMesh(const aiMesh* mesh);
		const BoundingBox& getBoundingBox() const;
		
		std::vector<Vertex> getVertices() const { return vertices; }
		std::vector<unsigned int> getIndices() const { return indices; }

	private:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::unique_ptr<VertexArray> vertexArray;
		BoundingBox boundingBox;

		void calcBoundingBox();
};
