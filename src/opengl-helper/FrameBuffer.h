#pragma once

#include <memory>

#include "Texture.h"

namespace hyperbright {
namespace openGLHelper {

/*
 A FrameBuffer object that can only hold a texture.
*/
class FrameBuffer
{
public:
	FrameBuffer(std::shared_ptr<Texture>& texture, bool isDepth);
	void bind() const;
	const std::shared_ptr<Texture>& getTexture() const;

private:
	unsigned int id;
	unsigned int renderBufferId;
	std::shared_ptr<Texture> texture;
};
}	// namespace openGLHelper
}	// namespace hyperbright