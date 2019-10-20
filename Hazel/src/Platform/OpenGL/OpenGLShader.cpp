#include "OpenGLShader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>

namespace Hazel
{

static GLenum ShaderTypeFromString(const std::string& type, const std::string& filepath)
{
	if(type == "vertex") return GL_VERTEX_SHADER;
	if(type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

	HZ_CORE_ASSERT(false, "Unknown shader type '" + type + "'");
	return 0;
}

OpenGLShader::OpenGLShader(const std::string& filepath)
	: m_ShaderFilepath(filepath)
{
	std::string source = ReadFile(filepath);
	auto shaderSources = PreProcess(source);
	Compile(shaderSources);

	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	auto lastDot = filepath.rfind('.');
	auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);
}

OpenGLShader::OpenGLShader(const std::string& name,
						   const std::string& vertexSrc,
						   const std::string& fragmentSrc)
	: m_Name(name)
	, m_ShaderFilepath("")
{
	shader_umap sources;
	sources[GL_VERTEX_SHADER] = vertexSrc;
	sources[GL_FRAGMENT_SHADER] = fragmentSrc;
	Compile(sources);
}

OpenGLShader::~OpenGLShader() { glDeleteProgram(m_RendererID); }

std::string OpenGLShader::ReadFile(const std::string& filepath)
{
	std::string result;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	if(in)
	{
		in.seekg(0, std::ios::end);
		result.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&result[0], result.size());
		in.close();
	}
	else
	{
		HZ_CORE_ERROR("Could not open shader file '{0}'", filepath);
	}

	return result;
}

shader_umap OpenGLShader::PreProcess(const std::string& source)
{
	shader_umap shaderSources;

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = source.find(typeToken, 0);

	while(pos != std::string::npos)
	{
		size_t eol = source.find_first_of("\r\n", pos);
		HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error in shader:\n" + source);
		size_t begin = pos + typeTokenLength + 1;
		std::string type = source.substr(begin, eol - begin);
		HZ_CORE_ASSERT(ShaderTypeFromString(type, m_ShaderFilepath),
					   "Invalid shader type specified in:\n" + source);

		size_t nextLinePos = source.find_first_not_of("\r\n", eol);
		pos = source.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type, m_ShaderFilepath)] = source.substr(
			nextLinePos,
			pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
	}

	return shaderSources;
}

void OpenGLShader::Compile(const shader_umap& shaderSources)
{
	GLuint program = glCreateProgram();
	constexpr int shaderArraySize = 2;
	HZ_CORE_ASSERT(shaderSources.size() <= 2, "Maximum number of shaders (for now) exceeded");
	std::array<GLenum, shaderArraySize> glShaderIDs;
	int shaderIDIdx = 0;
	for(auto& kv : shaderSources)
	{
		GLenum type = kv.first;
		const std::string& source = kv.second;

		GLuint shader = glCreateShader(type);

		const GLchar* sourceCStr = source.c_str();
		glShaderSource(shader, 1, &sourceCStr, 0);

		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if(isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);

			HZ_CORE_ERROR("{0}", infoLog.data());
			HZ_CORE_ASSERT(false, "Shader compilation failed!");
			break;
		}

		glAttachShader(program, shader);
		glShaderIDs[shaderIDIdx++] = shader;
	}

	m_RendererID = program;

	glLinkProgram(program);

	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if(isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

		glDeleteProgram(program);

		for(auto id : glShaderIDs) glDeleteShader(id);

		HZ_CORE_ERROR("{0}", infoLog.data());
		HZ_CORE_ASSERT(false, "Shader linking failed!");
		return;
	}

	for(auto id : glShaderIDs)
	{
		glDetachShader(program, id);
		glDeleteShader(id);
	}
}

void OpenGLShader::Bind() const { glUseProgram(m_RendererID); }

void OpenGLShader::Unbind() const { glUseProgram(0); }

GLint OpenGLShader::GetUniformLocation(const std::string& name) const
{
	auto keyValue = m_UniformLocationCache.find(name);
	if(keyValue != m_UniformLocationCache.end())
	{
		return keyValue->second;
	}

	GLint location = glGetUniformLocation(m_RendererID, name.c_str());
	m_UniformLocationCache[name] = location;

	return location;
}

void OpenGLShader::UploadUniformInt(const std::string& name, int value)
{
	GLint location = GetUniformLocation(name);
	glUniform1i(location, value);
}

void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
{
	GLint location = GetUniformLocation(name);
	glUniform1f(location, value);
}

void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& values)
{
	GLint location = GetUniformLocation(name);
	glUniform2f(location, values.x, values.y);
}

void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& values)
{
	GLint location = GetUniformLocation(name);
	glUniform3f(location, values.x, values.y, values.z);
}

void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& values)
{
	GLint location = GetUniformLocation(name);
	glUniform4fv(location, 1, glm::value_ptr(values));
}

void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
{
	GLint location = GetUniformLocation(name);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
{
	GLint location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

} // namespace Hazel
