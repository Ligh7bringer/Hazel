#include "Hazel/Core/Application.hpp"
#include "Hazel/Core/Input.hpp"

#include "Hazel/Renderer/Renderer.hpp"

#include <GLFW/glfw3.h>

namespace Hazel
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name)
{
	HZ_PROFILE_FUNCTION();

	HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = Window::Create(WindowProps(name));
	m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

	Renderer::Init();

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}

void Application::PushLayer(Layer* layer)
{
	HZ_PROFILE_FUNCTION();

	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void Application::PushOverlay(Layer* overlay)
{
	HZ_PROFILE_FUNCTION();

	m_LayerStack.PushOverlay(overlay);
	overlay->OnAttach();
}

void Application::OnEvent(Event& event)
{
	HZ_PROFILE_FUNCTION();

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

	for(auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
	{
		(*--it)->OnEvent(event);
		if(event.Handled) break;
	}
}

void Application::Run()
{
	HZ_PROFILE_FUNCTION();

	while(m_Running)
	{
		HZ_PROFILE_SCOPE("RunLoop");

		float time = static_cast<float>(glfwGetTime());
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		if(!m_Minimised)
		{
			{
				HZ_PROFILE_SCOPE("LayerStack OnUpdate");
				for(Layer* layer : m_LayerStack) layer->OnUpdate(timestep);
			}

			m_ImGuiLayer->Begin();
			{
				HZ_PROFILE_SCOPE("LayerStack OnImGuiRender")
				for(Layer* layer : m_LayerStack) layer->OnImGuiRender();
			}
			m_ImGuiLayer->End();
		}

		m_Window->OnUpdate();
	}
}

void Application::Close() { m_Running = false; }

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
	HZ_PROFILE_FUNCTION();

	if(e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimised = true;
		return false;
	}

	m_Minimised = false;
	Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

	return false;
}

} // namespace Hazel
