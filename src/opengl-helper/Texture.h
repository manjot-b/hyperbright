# pragma once

#include <glad/glad.h>

namespace hyperbright {
namespace openGLHelper {
/**
 * A basic texture loader.
 */

class Texture
{
	public:
		Texture(const char* filename);
		Texture(unsigned int width, unsigned int height, bool isDepth = false);
		~Texture();
		unsigned int getId() const;
		int getWidth() const;
		int getHeight() const;
		void bind(GLenum texture) const;

	private:
		unsigned int id;
		int width;
		int height;
};
}	// namespace openGLHelper
}	// namespace hyperbright