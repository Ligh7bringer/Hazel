#include "hzpch.h"
#include "VertexArray.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Hazel {

	Hazel::VertexArray* Hazel::VertexArray::Create()
	{
		switch (Renderer::GetAPI()) {
			case RendererAPI::API::None:   HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLVertexArray();
		}

		HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
		return nullptr;
	}

}
