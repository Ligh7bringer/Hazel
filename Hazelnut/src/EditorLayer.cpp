#include "EditorLayer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include <ImGuizmo.h>

#include "Hazel/Math/Math.hpp"
#include "Hazel/Scene/SceneSerializer.hpp"
#include "Hazel/Utils/Utils.hpp"

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

	m_EditorCamera = EditorCamera(30.f, 16.f / 9.f, 0.1f, 1000.f);

#if 0
	auto square = m_ActiveScene->CreateEntity("Green Square");
	square.AddComponent<SpriteRendererComponent>(glm::vec4{0.f, 1.f, 0.f, 1.f});

	auto redSquare = m_ActiveScene->CreateEntity("Red Square");
	redSquare.AddComponent<SpriteRendererComponent>(glm::vec4{1.f, 0.f, 0.f, 1.f});

	m_CameraEntity = m_ActiveScene->CreateEntity("Camera A");
	m_CameraEntity.AddComponent<CameraComponent>();

	m_SecondCamera = m_ActiveScene->CreateEntity("Camera B");
	auto& cc = m_SecondCamera.AddComponent<CameraComponent>();
	cc.Primary = false;

	class CameraController : public ScriptableEntity
	{
	public:
		void OnCreate()
		{
			auto& translation = GetComponent<TransformComponent>().Translation;
			translation.x = rand() % 10 - 5.f;
		}

		void OnDestroy() { }

		void OnUpdate(Timestep ts)
		{
			auto& translation = GetComponent<TransformComponent>().Translation;
			constexpr float speed = 5.f;

			if(Input::IsKeyPressed(HZ_KEY_A)) translation.x -= speed * ts;
			if(Input::IsKeyPressed(HZ_KEY_D)) translation.x += speed * ts;
			if(Input::IsKeyPressed(HZ_KEY_W)) translation.y += speed * ts;
			if(Input::IsKeyPressed(HZ_KEY_S)) translation.y -= speed * ts;
		}
	};

	m_CameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
	m_SecondCamera.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif

	m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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
		m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
		m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x),
										static_cast<uint32_t>(m_ViewportSize.y));
	}

	// Update camera
	if(m_ViewportFocused)
	{
		m_CameraController.OnUpdate(dt);
	}

	m_EditorCamera.OnUpdate(dt);

	// Render
	Renderer2D::ResetStats();
	m_Framebuffer->Bind();
	RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1.f});
	RenderCommand::Clear();

	m_ActiveScene->OnUpdateEditor(dt, m_EditorCamera);

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
	ImGuiStyle& style = ImGui::GetStyle();
	const auto defaultMinWinSize = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.f;
	if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = defaultMinWinSize;

	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("New", "Ctrl+N"))
			{
				NewScene();
			}
			ImGui::Separator();

			if(ImGui::MenuItem("Open...", "Ctrl+O"))
			{
				OpenScene();
			}

			if(ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
			{
				SaveSceneAs();
			}
			ImGui::Separator();

			if(ImGui::MenuItem("Exit")) Application::Get().Close();
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Gizmos"))
		{
			if(ImGui::MenuItem("Hidden", "Q", m_GizmoType == -1)) m_GizmoType = -1;

			if(ImGui::MenuItem("Translation", "W", m_GizmoType == ImGuizmo::OPERATION::TRANSLATE))
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;

			if(ImGui::MenuItem("Rotation", "E", m_GizmoType == ImGuizmo::OPERATION::ROTATE))
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;

			if(ImGui::MenuItem("Scale", "R", m_GizmoType == ImGuizmo::OPERATION::SCALE))
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
			ImGui::Separator();

			if(ImGui::MenuItem("Snap", "Ctrl", m_ShouldSnap)) m_ShouldSnap = !m_ShouldSnap;

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	m_SceneHierarchyPanel.OnImGuiRender();

	ImGui::Begin("Performance");
	ImGui::Text(
		"%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Stats");
	auto stats = Renderer2D::GetStats();
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	ImGui::Begin("Viewport");
	m_ViewportFocused = ImGui::IsWindowFocused();
	m_ViewportHovered = ImGui::IsWindowHovered();
	Application::Get().GetImGuiLayer()->SetBlockEvents(!m_ViewportFocused && !m_ViewportHovered);

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

	uint32_t textureID = m_Framebuffer->GetColorAttachmentRendererID();
	ImGui::Image(reinterpret_cast<void*>(static_cast<uint64_t>(textureID)),
				 ImVec2{m_ViewportSize.x, m_ViewportSize.y},
				 ImVec2{0, 1},
				 ImVec2{1, 0});

	// Gizmos
	Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
	if(selectedEntity && m_GizmoType != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		const float windowWidth = ImGui::GetWindowWidth();
		const float windowHeight = ImGui::GetWindowHeight();
		ImGuizmo::SetRect(
			ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

		// Runtime camera
		// auto cameraEntity = m_ActiveScene->GetPrimaryCameraEntity();
		// const auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
		// const glm::mat4& cameraProjection = camera.GetProjection();
		// glm::mat4 cameraView =
		// 	glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

		// Editor camera
		const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
		glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

		// Entity
		auto& tc = selectedEntity.GetComponent<TransformComponent>();
		glm::mat4 transform = tc.GetTransform();

		// Snapping
		const bool snap = Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL);
		// Snap value for translation
		float snapValue = 0.5f;
		// Snap value for rotation
		if(m_GizmoType == ImGuizmo::OPERATION::ROTATE) snapValue = 45.f;
		float snapValues[3] = {snapValue, snapValue, snapValue};

		ImGuizmo::Manipulate(glm::value_ptr(cameraView),
							 glm::value_ptr(cameraProjection),
							 static_cast<ImGuizmo::OPERATION>(m_GizmoType),
							 ImGuizmo::MODE::LOCAL,
							 glm::value_ptr(transform),
							 nullptr,
							 (snap || m_ShouldSnap) ? snapValues : nullptr);

		if(ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);

			tc.Translation = translation;
			// Use delta rotation to avoid Gimbal lock
			glm::vec3 deltaRotation = rotation - tc.Rotation;
			tc.Rotation += deltaRotation;
			tc.Scale = scale;
		}
	}

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::End();
}

void EditorLayer::OnEvent(Event& event)
{
	m_CameraController.OnEvent(event);
	m_EditorCamera.OnEvent(event);

	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(HZ_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& event)
{
	if(event.GetRepeatCount() > 0) return false;

	const bool controlPressed =
		Input::IsKeyPressed(HZ_KEY_LEFT_CONTROL) || Input::IsKeyPressed(HZ_KEY_RIGHT_CONTROL);
	const bool shiftPressed =
		Input::IsKeyPressed(HZ_KEY_LEFT_SHIFT) || Input::IsKeyPressed(HZ_KEY_RIGHT_SHIFT);
	switch(event.GetKeyCode())
	{
	case HZ_KEY_N: {
		if(controlPressed) NewScene();
		return true;
	}
	case HZ_KEY_O: {
		if(controlPressed) OpenScene();
		return true;
	}
	case HZ_KEY_S: {
		if(controlPressed && shiftPressed) SaveSceneAs();
		return true;
	}

	// Gizmos
	case HZ_KEY_Q: {
		m_GizmoType = -1;
		return true;
	}
	case HZ_KEY_W: {
		m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
		return true;
	}
	case HZ_KEY_E: {
		m_GizmoType = ImGuizmo::OPERATION::ROTATE;
		return true;
	}
	case HZ_KEY_R: {
		m_GizmoType = ImGuizmo::OPERATION::SCALE;
		return true;
	}

	default: return false;
	}
}

void EditorLayer::NewScene()
{
	m_ActiveScene = MakeRef<Scene>();
	m_ActiveScene->OnViewportResize(static_cast<uint32_t>(m_ViewportSize.x),
									static_cast<uint32_t>(m_ViewportSize.y));
	m_SceneHierarchyPanel.SetContext(m_ActiveScene);
}

void EditorLayer::OpenScene()
{
	const std::string filepath = FileDialogs::OpenFile("Hazel Scene", "hazel");
	if(!filepath.empty())
	{
		NewScene();

		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(filepath);
	}
}

void EditorLayer::SaveSceneAs()
{
	const std::string filepath = FileDialogs::SaveFile("Hazel Scene", "hazel");
	if(!filepath.empty())
	{
		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize(filepath);
	}
}

} // namespace Hazel
