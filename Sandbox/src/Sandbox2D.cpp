#include "Sandbox2D.h"

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, m_CameraController(1280.f / 720.f)
{
	m_CheckerTexture = Hazel::Texture2D::Create("assets/textures/checker.png");
}

Sandbox2D::~Sandbox2D() { HZ_PROFILE_FUNCTION(); }

void Sandbox2D::OnAttach() { HZ_PROFILE_FUNCTION(); }

void Sandbox2D::OnDetach() { HZ_PROFILE_FUNCTION(); }

void Sandbox2D::OnUpdate(Hazel::Timestep dt)
{
	HZ_PROFILE_FUNCTION();

	// Update camera
	m_CameraController.OnUpdate(dt);

	// Prepare for rendering
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
		Hazel::RenderCommand::Clear();
	}

	// Render
	{
		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawQuad({-1.f, 0.f}, {0.8f, 0.8f}, {0.8f, 0.2f, 0.3f, 1.f});
		Hazel::Renderer2D::DrawRotatedQuad(
			{0.5f, -0.5f}, {0.5f, 0.75f}, glm::radians(-45.f), {0.2f, 0.3f, 0.8f, 1.f});
		Hazel::Renderer2D::DrawQuad(
			{0.f, 0.f, -0.1f}, {10.f, 10.f}, m_CheckerTexture, 10.f, {1.f, 0.9f, 0.8f, 1.f});
		Hazel::Renderer2D::EndScene();
	}
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();
	ImGui::Begin("Performance");
	ImGui::Text(
		"%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square colour", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event) { m_CameraController.OnEvent(event); }
