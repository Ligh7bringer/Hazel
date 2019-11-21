#include "Sandbox2D.h"

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

template <typename Fn>
class Timer
{
public:
	Timer(const char* name, Fn&& func)
		: m_Name(name)
		, m_Func(std::move(func))
		, m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~Timer()
	{
		if(!m_Stopped) Stop();
	}

	void Stop()
	{
		auto endTimepoint = std::chrono::high_resolution_clock::now();

		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint)
							  .time_since_epoch()
							  .count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint)
							.time_since_epoch()
							.count();

		m_Stopped = true;

		float duration = (end - start) * 0.001f;
		m_Func({m_Name, duration});
	}

private:
	const char* m_Name;
	Fn m_Func;
	bool m_Stopped;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
};

#define PROFILE_SCOPE(name)                                                                        \
	Timer timer##__LINE__(                                                                         \
		name, [&](ProfileResult profileResult) { m_ProfileResults.push_back(profileResult); })

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, m_CameraController(1280.f / 720.f)
{
	m_CheckerTexture = Hazel::Texture2D::Create("assets/textures/checker.png");
}

void Sandbox2D::OnAttach() {}

void Sandbox2D::OnUpdate(Hazel::Timestep dt)
{
	PROFILE_SCOPE(__FUNCTION__);

	// Update camera
	{
		PROFILE_SCOPE("CameraController::OnUpdate");
		m_CameraController.OnUpdate(dt);
	}

	// Prepare for rendering
	{
		PROFILE_SCOPE("Renderer Prep");
		Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
		Hazel::RenderCommand::Clear();
	}

	// Render
	{
		PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawQuad({-1.f, 0.f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.f});
		Hazel::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.2f, 0.3f, 0.8f, 1.f});
		Hazel::Renderer2D::DrawQuad({0.f, 0.f, -0.1f}, {10.f, 10.f}, m_CheckerTexture);
		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnDetach() {}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("Performance");
	ImGui::Text(
		"%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	for(auto& result : m_ProfileResults)
	{
		char label[50];
		strcpy(label, "%.3fms  ");
		strcat(label, result.Name);
		ImGui::Text(label, result.Time);
	}
	m_ProfileResults.clear();
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square colour", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event) { m_CameraController.OnEvent(event); }
