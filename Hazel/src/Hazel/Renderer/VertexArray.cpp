#include "VertexArray.hpp"

#include "Hazel/Renderer/Renderer.hpp"
#include "Platform/OpenGL/OpenGLVertexArray.hpp"

namespace Hazel
{

Ref<VertexArray> VertexArray::Create()
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLVertexArray>();
	}

	HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
	return nullptr;
}

} // namespace Hazel
