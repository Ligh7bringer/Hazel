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

	virtual void Resize(uint32_t width, uint32_t height) override;

	virtual uint32_t GetColorAttachmentRendererID() const override { return m_ColourAttachment; }

	virtual const FramebufferSpecification& GetSpecification() const override
	{
		return m_Specification;
	}

private:
	void CleanUp();

private:
	uint32_t m_RendererID = 0, m_ColourAttachment = 0, m_DepthAttachment = 0;
	FramebufferSpecification m_Specification;
};

} // namespace Hazel
