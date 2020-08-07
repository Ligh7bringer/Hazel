#include "OpenGLFramebuffer.hpp"

#include <glad/glad.h>

namespace Hazel
{
static const uint32_t s_MaxFrameBufferSize = 8192;

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
	: m_Specification(spec)
{
	Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer() { CleanUp(); }

void OpenGLFramebuffer::Invalidate()
{
	if(m_RendererID)
	{
		CleanUp();
	}

	glCreateFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColourAttachment);
	glBindTexture(GL_TEXTURE_2D, m_ColourAttachment);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA8,
				 m_Specification.Width,
				 m_Specification.Height,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColourAttachment, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
	glTexStorage2D(
		GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);
	HZ_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
				   "Framebuffer is incomplete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGLFramebuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
	if(width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
	{
		HZ_CORE_WARN("Attempted to resize framebuffer to {0}x{1}", width, height);
		return;
	}

	m_Specification.Width = width;
	m_Specification.Height = height;
	Invalidate();
}

void OpenGLFramebuffer::CleanUp()
{
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures(1, &m_ColourAttachment);
	glDeleteTextures(1, &m_DepthAttachment);
}
} // namespace Hazel
