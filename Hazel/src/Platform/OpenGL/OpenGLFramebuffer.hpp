#pragma once

#include "Hazel/Renderer/Framebuffer.hpp"

namespace Hazel
{

class OpenGLFramebuffer : public Framebuffer
{
public:
	OpenGLFramebuffer(const FramebufferSpecification& spec);
	virtual ~OpenGLFramebuffer();

	void Invalidate();

	virtual void Bind() override;
	virtual void Unbind() override;

	virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColourAttachment; }

	virtual const FramebufferSpecification& GetSpecification() const override
	{
		return m_Specification;
	}

private:
	uint32_t m_RendererID, m_ColourAttachment, m_DepthAttachment;
	FramebufferSpecification m_Specification;
};

} // namespace Hazel
