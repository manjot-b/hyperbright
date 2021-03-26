#pragma once

#include <memory>

#include "Texture.h"

namespace hyperbright {
namespace openGLHelper {

/*
 A FrameBuffer object that can only hold a depth texture for now.
*/
class FrameBuffer
{
public:
	FrameBuffer(std::shared_ptr<Texture>& depthTexture);
	void bind() const;
	const std::shared_ptr<Texture>& getTexture() const;

private:
	unsigned int id;
	std::shared_ptr<Texture> texture;
};
}	// namespace openGLHelper
}	// namespace hyperbright