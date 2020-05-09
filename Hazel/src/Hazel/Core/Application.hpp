#pragma once

#include "Core.hpp"

#include "Hazel/Core/LayerStack.hpp"
#include "Hazel/Events/ApplicationEvent.hpp"
#include "Hazel/Events/Event.hpp"
#include "Window.hpp"

#include "Hazel/Core/Timestep.hpp"

#include "Hazel/ImGui/ImGuiLayer.hpp"

namespace Hazel
{

class Application
{
public:
	Application();
	virtual ~Application() = default;

	void Run();

	void OnEvent(Event& event);

	void PushLayer(Layer* layer);
	void PushOverlay(Layer* overlay);

	static Application& Get() { return *s_Instance; }
	Window& GetWindow() { return *m_Window; }

private:
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);

private:
	std::unique_ptr<Window> m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	LayerStack m_LayerStack;
	Timestep m_Timestep;
	float m_LastFrameTime = 0.f;
	bool m_Minimised = false;

private:
	static Application* s_Instance;
};

// to be defined in the client
Scope<Application> CreateApplication();

} // namespace Hazel
