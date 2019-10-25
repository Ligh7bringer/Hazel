#include "GraphicsContext.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Renderer.h"

namespace Hazel
{

Scope<GraphicsContext> GraphicsContext::Create(void* window)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL:
		return MakeScope<OpenGLContext>(static_cast<GLFWwindow*>(window));
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

} // namespace Hazel
