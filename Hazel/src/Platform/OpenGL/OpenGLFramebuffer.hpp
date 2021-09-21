#pragma once

#include "Hazel/Renderer/Framebuffer.hpp"

#include <vector>

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
	// Framebuffer should be bound manually before calling this
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

	virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

	virtual uint32_t GetColorAttachmentRendererID(size_t index = 0) const override
	{
		HZ_CORE_ASSERT(index < m_ColorAttachments.size(), "Index out of bounds");
		return m_ColorAttachments[index];
	}

	virtual const FramebufferSpecification& GetSpecification() const override
	{
		return m_Specification;
	}

private:
	void CleanUp();

private:
	uint32_t m_RendererID = 0;
	FramebufferSpecification m_Specification;

	std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecs;
	FramebufferTextureSpecification m_DepthAttachmentSpec = FramebufferTextureFormat::None;

	std::vector<uint32_t> m_ColorAttachments;
	uint32_t m_DepthAttachment{0};
};

} // namespace Hazel
