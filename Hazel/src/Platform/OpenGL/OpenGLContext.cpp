// clang-format off
#include "OpenGLContext.hpp"

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
	HZ_PROFILE_FUNCTION();

	glfwMakeContextCurrent(m_WindowHandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	HZ_CORE_ASSERT(status, "Failed to init glad!");

	HZ_CORE_INFO("----------------------------------------------------------");
	HZ_CORE_INFO("OpenGL info:");
	HZ_CORE_INFO("\tGL Vendor: {0}", glGetString(GL_VENDOR));
	HZ_CORE_INFO("\tGL Renderer: {0}", glGetString(GL_RENDERER));
	HZ_CORE_INFO("\tGL Version: {0}", glGetString(GL_VERSION));
	HZ_CORE_INFO("\tGLSL Version: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));
	HZ_CORE_INFO("----------------------------------------------------------");

#ifdef HZ_ENABLE_ASSERTS
	int versionMajor;
	int versionMinor;
	glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
	glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

	HZ_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5),
				   "Hazel requires at least OpenGL version 4.5!");
#endif
}

void OpenGLContext::SwapBuffers()
{
	HZ_PROFILE_FUNCTION();

	glfwSwapBuffers(m_WindowHandle);
}

} // namespace Hazel
