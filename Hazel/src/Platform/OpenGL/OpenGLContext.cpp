#include "hzpch.h"

// clang-format off
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace Hazel
{

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
	HZ_CORE_ASSERT(windowHandle, "WindowHandle is null!");
}

void OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_WindowHandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	HZ_CORE_ASSERT(status, "Failed to init glad!");

	HZ_CORE_INFO("----------------------------------------------------------");
	HZ_CORE_INFO("OpenGL info:");
	HZ_CORE_INFO("	GL Vendor: {0}", glGetString(GL_VENDOR));
	HZ_CORE_INFO("	GL Renderer: {0}", glGetString(GL_RENDERER));
	HZ_CORE_INFO("	GL Version: {0}", glGetString(GL_VERSION));
	HZ_CORE_INFO("	GLSL Version: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));
	HZ_CORE_INFO("----------------------------------------------------------");
}

void OpenGLContext::SwapBuffers() { glfwSwapBuffers(m_WindowHandle); }

} // namespace Hazel
