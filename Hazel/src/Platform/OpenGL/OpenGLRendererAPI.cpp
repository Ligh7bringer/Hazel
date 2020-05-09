#include "OpenGLRendererAPI.hpp"

#include <glad/glad.h>

namespace Hazel
{

#ifdef HZ_ENABLE_RENDERER_LOG
void OpenGLMessageCallback(unsigned source,
						   unsigned type,
						   unsigned id,
						   unsigned severity,
						   int length,
						   const char* message,
						   const void* userParam)
{
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_HIGH: HZ_CORE_CRITICAL(message); return;
	case GL_DEBUG_SEVERITY_MEDIUM: HZ_CORE_ERROR(message); return;
	case GL_DEBUG_SEVERITY_LOW: HZ_CORE_WARN(message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: HZ_CORE_TRACE(message); return;
	}

	HZ_CORE_ASSERT(false, "Unknown severity level!");
}
#endif

void OpenGLRendererAPI::Init()
{
	HZ_PROFILE_FUNCTION();

#ifdef HZ_ENABLE_RENDERER_LOG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);

	glDebugMessageControl(
		GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
}

void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void Hazel::OpenGLRendererAPI::SetClearColor(const glm::vec4& col)
{
	glClearColor(col.r, col.g, col.b, col.a);
}

void Hazel::OpenGLRendererAPI::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void Hazel::OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace Hazel
