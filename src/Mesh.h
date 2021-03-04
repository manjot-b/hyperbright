#pragma once

#include <assimp/scene.h>

#include <vector>
#include <memory>

#include "MeshUtils.h"
#include "VertexArray.h"

using InstanceModelMatricesPtr = std::shared_ptr<std::vector<glm::mat4>>;

class Mesh
{
	public:
		Material material;

		Mesh(const aiScene* scene, const aiMesh* mesh, const InstanceModelMatricesPtr& instancedModelMatrices);
		Mesh(const Mesh& mesh);
		~Mesh();
		void render(unsigned int instanceCount) const;
		void extractDataFromMesh(const aiScene* scene, const aiMesh* mesh);
		const BoundingBox& getBoundingBox() const;
		
		void setInstanceModelMatrices(const std::vector<glm::mat4>& instanceModelMatrices);
		void setInstanceColors(const std::vector<glm::vec4>& instanceModelMatrices);
		const std::vector<Vertex>& getVertices() const { return vertices; }
		const std::vector<unsigned int>& getIndices() const { return indices; }

	private:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::unique_ptr<VertexArray> vertexArray;
		BoundingBox boundingBox;

		void calcBoundingBox();
};
