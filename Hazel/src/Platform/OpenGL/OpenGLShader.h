#pragma once

#include "Hazel/Renderer/Shader.h"

#include <glm/glm.hpp>

#include <unordered_map>

// temporary
typedef unsigned int GLenum;
using shader_umap = std::unordered_map<GLenum, std::string>;

namespace Hazel
{

class OpenGLShader : public Shader
{
public:
	OpenGLShader(const std::string& filepath);
	OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
	virtual ~OpenGLShader();

	virtual void Bind() const override;
	virtual void Unbind() const override;

	void UploadUniformInt(const std::string& name, int value);

	void UploadUniformFloat(const std::string& name, float value);
	void UploadUniformFloat2(const std::string& name, const glm::vec2& values);
	void UploadUniformFloat3(const std::string& name, const glm::vec3& values);
	void UploadUniformFloat4(const std::string& name, const glm::vec4& values);

	void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
	void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

private:
	std::string ReadFile(const std::string& filepath);
	shader_umap PreProcess(const std::string& source);
	void Compile(const shader_umap& shaderSources);

private:
	uint32_t m_RendererID;
	std::string m_ShaderFilepath;
};

} // namespace Hazel
