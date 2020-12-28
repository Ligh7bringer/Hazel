#include "EditorLayer.hpp"

#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Hazel
{

EditorLayer::EditorLayer()
	: Layer("EditorLayer")
	, m_CameraController(1280.f / 720.f)
	, m_NumQuads(400)
	, m_ViewportFocused(false)
{ }

EditorLayer::~EditorLayer() { HZ_PROFILE_FUNCTION(); }

void EditorLayer::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	FramebufferSpecification fbSpec;
	// FIXME: Shouldn't be hardcoded
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = Framebuffer::Create(fbSpec);

	m_CheckerTexture = Texture2D::Create("assets/textures/checker.png");

	m_ActiveScene = MakeRef<Scene>();

	m_SquareEntity = m_ActiveScene->CreateEntity("Square");
	m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{0.f, 1.f, 0.f, 1.f});

	m_CameraEntity = m_ActiveScene->CreateEntity("Camera Entity");
	m_CameraEntity.AddComponent<CameraComponent>();

	m_SecondCamera = m_ActiveScene->CreateEntity("Clip Space Camera");
	auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
	cc.Primary = false;

	class CameraController : public ScriptableEntity
	{
	public:
		void OnCreate()
		{
			auto& transform = GetComponent<TransformComponent>().Transform;
			transform[3][0] = rand() % 10 - 5.f;
		}

		void OnDestroy() { }

		void OnUpdate(Timestep ts)
		{
			auto& transform = GetComponent<TransformComponent>().Transform;
			constexpr float speed = 5.f;

			if(Input::IsKeyPressed(HZ_KEY_A)) transform[3][0] -= speed * ts;
			if(Input::IsKeyPressed(HZ_KEY_D)) transform[3][0] += speed * ts;
			if(Input::IsKeyPressed(HZ_KEY_W)) transform[3][1] += speed * ts;
			if(Input::IsKeyPressed(HZ_KEY_S)) transform[3][1] -= speed * ts;
		}
	};

	m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
	m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
}

void EditorLayer::OnDetach() { HZ_PROFILE_FUNCTION(); }

void EditorLayer::OnUpdate(Timestep dt)
{
	HZ_PROFILE_FUNCTION();

	// Resize
	if(FramebufferSpecification spec = m_Framebuffer->GetSpecification();
	   m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
	   (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
	{
		m_Framebuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);

		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x),
										static_cast<uint32_t>(m_ViewportSize.y));
	}

	// Update camera
	if(m_ViewportFocused) m_CameraController.OnUpdate(dt);

	// Prepare for rendering
	m_Framebuffer->Bind();
	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
	RenderCommand::Clear();

	// Render
	Renderer2D::ResetStats();

	m_ActiveScene->OnUpdate(dt);

	m_Framebuffer->Unbind();
}

void EditorLayer::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if(opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
						ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if(dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if(opt_fullscreen) ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Exit")) Application::Get().Close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("Performance");
	ImGui::Text(
		"%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Settings");
	auto stats = Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats:");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::End();

	ImGui::Begin("Renderer settings");
	static int maxQuads = 20000;
	ImGui::InputInt("Max quads per draw call", &maxQuads);
	ImGui::InputInt("Quads to draw", &m_NumQuads);
	Renderer2D::SetMaxQuadsPerDrawCall(maxQuads);

	ImGui::Separator();
	auto& tag = m_SquareEntity.GetComponent<TagComponent>().Tag;
	ImGui::Text("Tag: %s", tag.c_str());

	auto& squareCol = m_SquareEntity.GetComponent<SpriteRendererComponent>().Colour;
	ImGui::ColorEdit4("Square Colour", glm::value_ptr(squareCol));

	ImGui::Separator();
	ImGui::DragFloat3(
		"Camera Transform",
		glm::value_ptr(m_PrimaryCamera
						   ? m_CameraEntity.GetComponent<TransformComponent>().Transform[3]
						   : m_SecondCamera.GetComponent<TransformComponent>().Transform[3]));

	if(ImGui::Checkbox("Camera A", &m_PrimaryCamera))
	{
		m_SecondCamera.GetComponent<CameraComponent>().Primary = !m_PrimaryCamera;
		m_CameraEntity.GetComponent<CameraComponent>().Primary = m_PrimaryCamera;
	}

	{
		auto& camera = m_SecondCamera.GetComponent<CameraComponent>().Camera;
		float orthoSize = camera.GetOrthographicSize();
		if(ImGui::DragFloat("Second Camera Ortho Size", &orthoSize))
		{
			camera.SetOrthographicSize(orthoSize);
		}
	}

	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	ImGui::Begin("Viewport");

	m_ViewportFocused = ImGui::IsWindowFocused();
	m_ViewportHovered = ImGui::IsWindowHovered();
	Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused || !m_ViewportHovered);

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image(
		(void*)textureID, ImVec2{m_ViewportSize.x, m_ViewportSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

void EditorLayer::OnEvent(Event& event) { m_CameraController.OnEvent(event); }

} // namespace Hazel