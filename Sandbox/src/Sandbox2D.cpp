#include "hzpch.h"

#include "Sandbox2D.h"

#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, m_CameraController(1280.f / 720.f)
{}

void Sandbox2D::OnAttach() {}

void Sandbox2D::OnUpdate(Hazel::Timestep dt)
{
	// Update
	m_CameraController.OnUpdate(dt);

	// Render
	Hazel::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
	Hazel::RenderCommand::Clear();

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Hazel::Renderer2D::DrawQuad({0.f, 0.f}, {1.f, 1.f}, {0.8f, 0.2f, 0.3f, 1.f});
	Hazel::Renderer2D::EndScene();
}

void Sandbox2D::OnDetach() {}

void Sandbox2D::OnImGuiRender()
{
	ImGui::Begin("FPS");
	ImGui::Text(
		"%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Settings");
	ImGui::ColorEdit4("Square colour", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event) { m_CameraController.OnEvent(event); }
