#include "FrameBuffer.h"

#include <glad/glad.h>


namespace hyperbright {
namespace openGLHelper {

FrameBuffer::FrameBuffer(std::shared_ptr<Texture>& depthTexture) :
	texture(depthTexture)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	// Might need to bind texture first.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->getId(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

const std::shared_ptr<Texture>& FrameBuffer::getTexture() const
{
	return texture;
}

}	// namespace openGLHelper
}	// namespace hyperbright