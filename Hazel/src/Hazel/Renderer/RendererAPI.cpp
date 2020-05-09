#include "RendererAPI.hpp"

#include "Platform/OpenGL/OpenGLRendererAPI.hpp"

namespace Hazel
{

RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

Scope<RendererAPI> RendererAPI::Create()
{
	switch(s_API)
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeScope<OpenGLRendererAPI>();
	}

	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	return nullptr;
}

} // namespace Hazel
