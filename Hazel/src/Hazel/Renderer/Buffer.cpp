#include "Buffer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Renderer.h"

namespace Hazel
{

Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLVertexBuffer>(size);
	}

	HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
	return nullptr;
}

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLVertexBuffer>(vertices, size);
	}

	HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLIndexBuffer>(indices, count);
	}

	HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
	return nullptr;
}

} // namespace Hazel
