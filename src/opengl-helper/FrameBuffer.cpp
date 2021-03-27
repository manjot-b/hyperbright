#include "FrameBuffer.h"

#include <glad/glad.h>

#include <iostream>

namespace hyperbright {
namespace openGLHelper {

FrameBuffer::FrameBuffer(std::shared_ptr<Texture>& texture, bool isDepth) :
	id(0), renderBufferId(0), texture(texture)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	// Might need to bind texture first.
	glFramebufferTexture2D(GL_FRAMEBUFFER, isDepth ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->getId(), 0);

	if (isDepth)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}
	else
	{
		glGenRenderbuffers(1, &renderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, texture->getWidth(), texture->getHeight());
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "ERROR: " << status << " Could not create framebuffer.\n";

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	if (renderBufferId)
		glDeleteRenderbuffers(1, &renderBufferId);
	glDeleteFramebuffers(1, &id);
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