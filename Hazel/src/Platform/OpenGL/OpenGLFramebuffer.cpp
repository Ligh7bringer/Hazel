#include "OpenGLFramebuffer.hpp"

#include <glad/glad.h>

namespace Hazel
{
static const uint32_t s_MaxFrameBufferSize = 8192;

namespace Utils
{

static constexpr GLenum TextureTarget(bool multisampled)
{
	return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
}

static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
{
	glCreateTextures(TextureTarget(multisampled), count, outID);
}

static void BindTexture(bool multisampled, uint32_t id)
{
	glBindTexture(TextureTarget(multisampled), id);
}

static void AttachColorTexture(uint32_t id,
							   int samples,
							   GLenum internalFormat,
							   GLenum format,
							   uint32_t width,
							   uint32_t height,
							   int index)
{
	const bool multisampled = samples > 1;
	if(multisampled)
	{
		glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
	}
	else
	{
		glTexImage2D(
			GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
}

static void AttachDepthTexture(
	uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
{
	const bool multisampled = samples > 1;
	if(multisampled)
	{
		glTexImage2DMultisample(
			GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
	}
	else
	{
		glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
}

static bool IsDepthFormat(FramebufferTextureFormat format)
{
	return format == FramebufferTextureFormat::Depth ||
		   format == FramebufferTextureFormat::DEPTH24STENCIL8;
}

static GLenum HazelFBTextureFormatToGL(FramebufferTextureFormat format)
{
	switch(format)
	{
	case FramebufferTextureFormat::RGBA8: return GL_RGBA8;
	case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
	default: HZ_CORE_ASSERT(false, "Unreachable"); break;
	}

	return 0;
}

} // namespace Utils

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
	: m_Specification(spec)
{
	for(auto& format : m_Specification.Attachments.Attachments)
	{
		if(!Utils::IsDepthFormat(format.TextureFormat))
		{
			m_ColorAttachmentSpecs.push_back(format);
		}
		else
		{
			m_DepthAttachmentSpec = format;
		}
	}

	Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer() { CleanUp(); }

void OpenGLFramebuffer::Invalidate()
{
	if(m_RendererID)
	{
		CleanUp();

		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
	}

	glCreateFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	const bool multisample = m_Specification.Samples > 1;
	if(!m_ColorAttachmentSpecs.empty())
	{
		m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
		Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

		for(size_t i = 0; i < m_ColorAttachments.size(); ++i)
		{
			Utils::BindTexture(multisample, m_ColorAttachments[i]);
			switch(m_ColorAttachmentSpecs[i].TextureFormat)
			{
			case FramebufferTextureFormat::RGBA8:
				Utils::AttachColorTexture(m_ColorAttachments[i],
										  m_Specification.Samples,
										  GL_RGBA8,
										  GL_RGBA,
										  m_Specification.Width,
										  m_Specification.Height,
										  i);
				break;
			case FramebufferTextureFormat::RED_INTEGER:
				Utils::AttachColorTexture(m_ColorAttachments[i],
										  m_Specification.Samples,
										  GL_R32I,
										  GL_RED_INTEGER,
										  m_Specification.Width,
										  m_Specification.Height,
										  i);
				break;
			default: HZ_CORE_ASSERT(false, "Unsupported color attachment format"); break;
			}
		}
	}

	if(m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None)
	{
		Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
		Utils::BindTexture(multisample, m_DepthAttachment);
		switch(m_DepthAttachmentSpec.TextureFormat)
		{
		case FramebufferTextureFormat::DEPTH24STENCIL8:
			Utils::AttachDepthTexture(m_DepthAttachment,
									  m_Specification.Samples,
									  GL_DEPTH24_STENCIL8,
									  GL_DEPTH_STENCIL_ATTACHMENT,
									  m_Specification.Width,
									  m_Specification.Height);
			break;
		default: HZ_CORE_ASSERT(false, "Unsupported depth attachment format"); break;
		}
	}

	if(m_ColorAttachments.size() > 1)
	{
		HZ_CORE_ASSERT(m_ColorAttachments.size() <= 4,
					   "Up to 4 color attachments supported currently");
		const GLenum buffers[4] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
		glDrawBuffers(m_ColorAttachments.size(), buffers);
	}
	else if(m_ColorAttachments.empty())
	{
		glDrawBuffer(GL_NONE);
	}

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

int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Attachment index out of bounds");
	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	int pixelData;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

	return pixelData;
}

void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
	HZ_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "Attachment index out of bounds");

	const auto& spec = m_ColorAttachmentSpecs[attachmentIndex];
	glClearTexImage(m_ColorAttachments[attachmentIndex],
					0,
					Utils::HazelFBTextureFormatToGL(spec.TextureFormat),
					GL_INT,
					&value);
}

void OpenGLFramebuffer::CleanUp()
{
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	glDeleteTextures(1, &m_DepthAttachment);
}
} // namespace Hazel
