#include "Texture.hpp"

#include "Platform/OpenGL/OpenGLTexture.hpp"
#include "Renderer.hpp"

namespace Hazel
{

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLTexture2D>(width, height);
	}

	HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
	return nullptr;
}

Ref<Texture2D> Texture2D::Create(const std::string& path)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLTexture2D>(path);
	}

	HZ_CORE_ASSERT(false, "Unsupported and/or unknown Render API selected!");
	return nullptr;
}

} // namespace Hazel
