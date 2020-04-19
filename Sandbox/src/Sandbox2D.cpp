#include "Sandbox2D.h"

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, m_CameraController(1280.f / 720.f)
	, m_NumQuads(400)
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
	Hazel::Renderer2D::ResetStats();
	{
		static float rotation = 0.f;
		rotation += dt * 50.f;

		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawRotatedQuad(
			{1.f, 0.f}, {0.8f, 0.8f}, -45.f, {0.8f, 0.3f, 0.2f, 1.f});
		Hazel::Renderer2D::DrawQuad({-1.f, 0.f}, {0.8f, 0.8f}, {0.2f, 0.3f, 0.8f, 1.f});
		Hazel::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.8f, 0.2f, 0.3f, 1.f});
		Hazel::Renderer2D::DrawQuad({0.f, 0.f, -0.1f}, {20.f, 20.f}, m_CheckerTexture, 10.f);
		Hazel::Renderer2D::DrawRotatedQuad(
			{-2.f, 0.f, 0.f}, {1.f, 1.f}, rotation, m_CheckerTexture, 1.f);

		auto gridWidth = sqrt(m_NumQuads);
		auto gridHeight = m_NumQuads / gridWidth + 1;

		const glm::vec2 size{0.5f, 0.5f};
		const glm::vec2 offset{0.05f, 0.05f};
		const glm::vec2 startPos{-5.f, -5.f};
		auto currPos = startPos;

		for(int y = 0; y < gridHeight; ++y)
		{
			for(int x = 0; x < gridWidth; ++x)
			{
				glm::vec4 colour = {(x + 5.f) / 10.f, 0.4f, (y + 5.f) / 10.f, .5f};
				Hazel::Renderer2D::DrawQuad(currPos, size, colour);
				currPos.x += size.x + offset.x;
			}
			currPos.x = startPos.x;
			currPos.y += size.y + offset.y;
		}
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
	static int maxQuads = 100;
	ImGui::InputInt("Max quads per draw call", &maxQuads);
	ImGui::InputInt("Draw quads", &m_NumQuads);
	Hazel::Renderer2D::SetMaxQuadsPerDrawCall(maxQuads);
	ImGui::End();

	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Begin("Renderer2D Stats");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& event) { m_CameraController.OnEvent(event); }
