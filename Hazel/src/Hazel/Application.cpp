#include "hzpch.h"

#include "Application.h"
#include "Input.h"

#include "Hazel/Renderer/Renderer.h"

#include <GLFW/glfw3.h>

namespace Hazel
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;

Application::Application()
{
	HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
	s_Instance = this;

	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

	Renderer::Init();

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}

void Application::PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }

void Application::PushOverlay(Layer* overlay) { m_LayerStack.PushOverlay(overlay); }

void Application::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

	for(auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
	{
		(*--it)->OnEvent(event);
		if(event.Handled) break;
	}
}

void Application::Run()
{
	while(m_Running)
	{
		float time = static_cast<float>(glfwGetTime());
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		for(Layer* layer : m_LayerStack) layer->OnUpdate(timestep);

		m_ImGuiLayer->Begin();
		for(Layer* layer : m_LayerStack) layer->OnImGuiRender();
		m_ImGuiLayer->End();

		m_Window->OnUpdate();
	}
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

} // namespace Hazel
