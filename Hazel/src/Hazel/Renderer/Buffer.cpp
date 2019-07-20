#include "hzpch.h"
#include "Buffer.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Hazel {

	VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::None: {
				// not implemented
				HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
				return nullptr;
			}
			case RendererAPI::OpenGL: return new OpenGLVertexBuffer(vertices, size);
		}
		
		HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count) {
		switch (Renderer::GetAPI()) {
			case RendererAPI::None: {
				// not implemented
				HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
				return nullptr;
			}
			case RendererAPI::OpenGL: return new OpenGLIndexBuffer(indices, count);
		}

		HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
		return nullptr;
	}

}