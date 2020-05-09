#include "Sandbox2D.hpp"

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D")
	, m_CameraController(1280.f / 720.f)
	, m_NumQuads(400)
{}

Sandbox2D::~Sandbox2D() { HZ_PROFILE_FUNCTION(); }

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	m_CheckerTexture = Hazel::Texture2D::Create("assets/textures/checker.png");
	m_SpriteSheet = Hazel::Texture2D::Create("assets/demo/textures/RPGpack_sheet_2X.png");
	m_StairsTexture = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {7, 6}, {128, 128});
	m_BarrelTexture = Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {8, 2}, {128, 128});
	m_TreeTexture =
		Hazel::SubTexture2D::CreateFromCoords(m_SpriteSheet, {2, 1}, {128, 128}, {1, 2});

	m_ParticleSystem = ParticleSystem();
	m_Particle.ColorBegin = {254 / 255.0f, 212 / 255.0f, 123 / 255.0f, 1.0f};
	m_Particle.ColorEnd = {254 / 255.0f, 109 / 255.0f, 41 / 255.0f, 1.0f};
	m_Particle.SizeBegin = 0.5f, m_Particle.SizeVariation = 0.3f, m_Particle.SizeEnd = 0.0f;
	m_Particle.LifeTime = 5.0f;
	m_Particle.Velocity = {0.0f, 0.0f};
	m_Particle.VelocityVariation = {3.0f, 1.0f};
	m_Particle.Position = {0.0f, 0.0f};
}

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

#if 0
	{
		static float rotation = 0.f;
		rotation += dt * 50.f;

		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawRotatedQuad(
			{1.f, 0.f}, {0.8f, 0.8f}, glm::radians(-45.f), {0.8f, 0.3f, 0.2f, 1.f});
		Hazel::Renderer2D::DrawQuad({-1.f, 0.f}, {0.8f, 0.8f}, {0.2f, 0.3f, 0.8f, 1.f});
		Hazel::Renderer2D::DrawQuad({0.5f, -0.5f}, {0.5f, 0.75f}, {0.8f, 0.2f, 0.3f, 1.f});
		Hazel::Renderer2D::DrawQuad({0.f, 0.f, -0.1f}, {20.f, 20.f}, m_CheckerTexture, 10.f);
		Hazel::Renderer2D::DrawRotatedQuad(
			{-2.f, 0.f, 0.f}, {1.f, 1.f}, glm::radians(rotation), m_CheckerTexture, 1.f);

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
#endif

	if(Hazel::Input::IsMouseBtnPressed(HZ_MOUSE_BUTTON_LEFT))
	{
		auto [x, y] = Hazel::Input::GetMousePos();
		auto width = Hazel::Application::Get().GetWindow().GetWidth();
		auto height = Hazel::Application::Get().GetWindow().GetHeight();

		auto bounds = m_CameraController.GetBounds();
		auto pos = m_CameraController.GetCamera().GetPosition();
		x = (x / width) * bounds.GetWidth() - bounds.GetWidth() * 0.5f;
		y = bounds.GetHeight() * 0.5f - (y / height) * bounds.GetHeight();
		m_Particle.Position = {x + pos.x, y + pos.y};

		for(int i = 0; i < 5; i++) m_ParticleSystem.Emit(m_Particle);
	}

	m_ParticleSystem.OnUpdate(dt);
	m_ParticleSystem.OnRender(m_CameraController.GetCamera());

	Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
	Hazel::Renderer2D::DrawQuad({0.f, 0.f, 0.5f}, {1.f, 1.f}, m_StairsTexture);
	Hazel::Renderer2D::DrawQuad({1.f, 0.f, 0.5f}, {1.f, 1.f}, m_BarrelTexture);
	Hazel::Renderer2D::DrawQuad({-1.f, 0.f, 0.5f}, {1.f, 2.f}, m_TreeTexture);
	Hazel::Renderer2D::EndScene();
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Performance");
	ImGui::Text(
		"%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Settings");
	static int maxQuads = 20000;
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
