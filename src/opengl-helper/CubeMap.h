#pragma once

#include <glad/glad.h>
#include <array>
#include <cstdint>

namespace hyperbright {
namespace openGLHelper {
/**
* A CubeMap loader.
*/

class CubeMap
{
public:
	CubeMap(const char* filename);
	unsigned int getId() const;
	int getFaceWidth() const;
	int getFaceHeight() const;
	void bind(GLenum texture) const;

private:
	std::array<unsigned char*, 6> extractFaces(const unsigned char* cubemap, int width, int height, int nrChannels);
	void extractFace(const unsigned char* cubemap, uint32_t mapWidth, uint32_t mapHeight, uint32_t nrChannels,
		unsigned char* face, uint32_t faceWidth, uint32_t faceHeight,
		uint32_t startX, uint32_t startY);
	unsigned int id;
	int faceWidth;
	int faceHeight;
};
}	// namespace openGLHelper
}	// namespace hyperbright