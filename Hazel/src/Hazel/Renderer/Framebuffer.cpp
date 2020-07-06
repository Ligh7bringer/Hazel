#include "Framebuffer.hpp"

#include "Hazel/Core/Core.hpp"
#include "Hazel/Renderer/Renderer.hpp"
#include "Platform/OpenGL/OpenGLFramebuffer.hpp"

namespace Hazel
{

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None: HZ_CORE_ASSERT(false, "RendererAPI::NONE not supported"); break;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLFramebuffer>(spec);
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI");
	return nullptr;
}

} // namespace Hazel
