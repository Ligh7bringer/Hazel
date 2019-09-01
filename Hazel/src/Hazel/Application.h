#pragma once

#include "Core.h"

#include "Events/ApplicationEvent.h"
#include "Events/Event.h"
#include "Hazel/LayerStack.h"
#include "Window.h"

#include "Hazel/Core/Timestep.h"

#include "Hazel/ImGui/ImGuiLayer.h"

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

	inline static Application& Get() { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }

private:
	bool OnWindowClose(WindowCloseEvent& e);

private:
	std::unique_ptr<Window> m_Window;
	ImGuiLayer* m_ImGuiLayer;
	bool m_Running = true;
	LayerStack m_LayerStack;
	Timestep m_Timestep;
	float m_LastFrameTime = 0.f;

private:
	static Application* s_Instance;
};

// to be defined in the client
Application* CreateApplication();

} // namespace Hazel
