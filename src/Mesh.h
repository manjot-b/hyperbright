#include <vector>
#include <assimp/scene.h>
#include <memory>

#include "Vertex.h"
#include "VertexArray.h"

class Mesh
{
	public:
		Mesh(const aiMesh* mesh);
		~Mesh();
		void draw() const;
		void extractDataFromMesh(const aiMesh* mesh);

	private:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::unique_ptr<VertexArray> vertexArray;
};
