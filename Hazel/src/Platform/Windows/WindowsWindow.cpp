#include "WindowsWindow.hpp"

#include "Hazel/Events/ApplicationEvent.hpp"
#include "Hazel/Events/KeyEvent.hpp"
#include "Hazel/Events/MouseEvent.hpp"

#include "Hazel/Renderer/Renderer.hpp"

#include "Platform/OpenGL/OpenGLContext.hpp"

namespace Hazel
{

float Window::s_HighDPIScaleFactor = 1.0f;

static uint8_t s_GLFWWindowCount = 0;

static void GLFWErrorCallback(int error, const char* description)
{
	HZ_CORE_ERROR("GLFW Error {0}: {1}", error, description);
}

Scope<Window> Window::Create(const WindowProps& props) { return MakeScope<WindowsWindow>(props); }

WindowsWindow::WindowsWindow(const WindowProps& props)
{
	HZ_PROFILE_FUNCTION();

	Init(props);
}

WindowsWindow::~WindowsWindow()
{
	HZ_PROFILE_FUNCTION();

	Shutdown();
}

void WindowsWindow::Init(const WindowProps& props)
{
	HZ_PROFILE_FUNCTION();

	m_Data.Title = props.Title;
	m_Data.Width = props.Width;
	m_Data.Height = props.Height;

	HZ_CORE_INFO("Creating window {0} ({1}x{2})", props.Title, props.Width, props.Height);

	if(s_GLFWWindowCount == 0)
	{
		HZ_PROFILE_SCOPE("glfwInit")
		HZ_CORE_INFO("Initializing GLFW");
		int success = glfwInit();
		HZ_CORE_ASSERT(success, "Could not init GLFW!");
		glfwSetErrorCallback(GLFWErrorCallback);
	}

	// set window hints, i.e. opengl version!
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

#ifdef HZ_ENABLE_RENDERER_LOG
	if(Renderer::GetAPI() == RendererAPI::API::OpenGL)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	{
		HZ_PROFILE_SCOPE("glfwCreateWindow")

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		float xscale, yscale;
		glfwGetMonitorContentScale(monitor, &xscale, &yscale);

		if(xscale > 1.0f || yscale > 1.0f)
		{
			s_HighDPIScaleFactor = yscale;
			glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
		}

		m_Window = glfwCreateWindow(
			(int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
		++s_GLFWWindowCount;
	}

	m_Context = GraphicsContext::Create(m_Window);
	m_Context->Init();

	glfwSetWindowUserPointer(m_Window, &m_Data);
	SetVSync(false);

	/* GLFW callbacks: */
	// window resized
	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.Width = width;
		data.Height = height;

		WindowResizeEvent event(width, height);
		data.EventCallback(event);
	});

	// window closed
	glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		WindowCloseEvent event;
		data.EventCallback(event);
	});

	// keyboard key pressed, released, repeated
	glfwSetKeyCallback(m_Window,
					   [](GLFWwindow* window, int key, int scancode, int action, int mods) {
						   WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

						   switch(action)
						   {
						   case GLFW_PRESS: {
							   KeyPressedEvent event(key, 0);
							   data.EventCallback(event);
							   break;
						   }
						   case GLFW_RELEASE: {
							   KeyReleasedEvent event(key);
							   data.EventCallback(event);
							   break;
						   }
						   case GLFW_REPEAT: {
							   // repeat count is just set to 1 for now
							   KeyPressedEvent event(key, 1);
							   data.EventCallback(event);
							   break;
						   }
						   }
					   });

	// key typed
	glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		KeyTypedEvent event(keycode);
		data.EventCallback(event);
	});

	// mouse button pressed, released
	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int modes) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		switch(action)
		{
		case GLFW_PRESS: {
			MouseButtonPressedEvent event(button);
			data.EventCallback(event);
			break;
		}
		case GLFW_RELEASE: {
			MouseButtonReleasedEvent event(button);
			data.EventCallback(event);
			break;
		}
		}
	});

	// mouse scroll used
	glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseScrolledEvent event((float)xOffset, (float)yOffset);
		data.EventCallback(event);
	});

	// mouse moved
	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double x, double y) {
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

		MouseMovedEvent event((float)x, (float)y);
		data.EventCallback(event);
	});
}

void WindowsWindow::Shutdown()
{
	HZ_PROFILE_FUNCTION();

	glfwDestroyWindow(m_Window);

	if(--s_GLFWWindowCount == 0)
	{
		HZ_CORE_INFO("Terminating GLFW");
		glfwTerminate();
	}
}

void WindowsWindow::OnUpdate()
{
	HZ_PROFILE_FUNCTION();

	glfwPollEvents();
	m_Context->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled)
{
	HZ_PROFILE_FUNCTION();

	if(enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	m_Data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const { return m_Data.VSync; }

} // namespace Hazel
