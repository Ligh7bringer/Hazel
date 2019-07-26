#pragma once

#include "Hazel/Renderer/Shader.h"

namespace Hazel {

	class OpenGLShader : public Shader {
	public:
		OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc, bool fromFile = false);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) override;
		
	private:
		void CompileShaders(const std::string& vertexSrc, const std::string& fragmentSrc);
		void ReadFiles(const std::string& vertexPath, const std::string& fragmentPath);
		std::string ReadFile(const std::string& filepath);

	private:
		uint32_t m_RendererID;
	};

}