#include "Shader.hpp"

#include "Hazel/Renderer/Renderer.hpp"
#include "Platform/OpenGL/OpenGLShader.hpp"

namespace Hazel
{

// class Shader
Ref<Shader> Shader::Create(const std::string& filepath)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLShader>(filepath);
	}

	HZ_CORE_ASSERT(false, "Unknown Render API!");
	return nullptr;
}

Ref<Shader> Shader::Create(const std::string& name,
						   const std::string& vertexSrc,
						   const std::string& fragmentSrc)
{
	switch(Renderer::GetAPI())
	{
	case RendererAPI::API::None:
		HZ_CORE_ASSERT(false, "RenderAPI::NONE is not implemented!");
		return nullptr;
	case RendererAPI::API::OpenGL: return MakeRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
	}

	HZ_CORE_ASSERT(false, "Unknown Render API!");
	return nullptr;
}

// class ShaderLibrary
void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
{
	HZ_CORE_ASSERT(!Exists(name), "Shader already exists!");
	m_Shaders[name] = shader;
}

void ShaderLibrary::Add(const Ref<Shader>& shader)
{
	auto& name = shader->GetName();
	Add(name, shader);
}

Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& filepath)
{
	auto shader = Shader::Create(filepath);
	Add(shader);
	return shader;
}

Hazel::Ref<Hazel::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
	auto shader = Shader::Create(filepath);
	Add(name, shader);
	return shader;
}

Hazel::Ref<Hazel::Shader> ShaderLibrary::Get(const std::string& name)
{
	auto shader = m_Shaders.find(name);
	HZ_CORE_ASSERT(shader != m_Shaders.end(), "Shader not found!");

	return shader->second;
}

bool ShaderLibrary::Exists(const std::string& name)
{
	return m_Shaders.find(name) != m_Shaders.end();
}

} // namespace Hazel
