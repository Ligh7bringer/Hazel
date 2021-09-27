#include "EditorLayer.hpp"

#include "Hazel/Math/Math.hpp"
#include "Hazel/Scene/SceneSerializer.hpp"
#include "Hazel/Utils/Utils.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <string>
#include <filesystem>

#include <ImGuizmo.h>

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
	fbSpec.Attachments = {FramebufferTextureFormat::RGBA8,
						  FramebufferTextureFormat::RED_INTEGER,
						  FramebufferTextureFormat::Depth};
	// FIXME: Shouldn't be hardcoded
	fbSpec.Width = 1280;
	fbSpec.Height = 720;
	m_Framebuffer = Framebuffer::Create(fbSpec);

	m_CheckerTexture = Texture2D::Create("assets/textures/checker.png");

	m_ActiveScene = MakeRef<Scene>();

	m_EditorCamera = EditorCamera(30.f, 16.f / 9.f, 0.1f, 1000.f);

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

	m_Framebuffer->ClearAttachment(1, -1);

	m_ActiveScene->OnUpdateEditor(dt, m_EditorCamera);

	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds[0].x;
	my -= m_ViewportBounds[0].y;
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	// Flip y
	my = viewportSize.y - my;
	const auto mouseX = static_cast<int>(mx);
	const auto mouseY = static_cast<int>(my);

	if(mouseX >= 0 && mouseY >= 0 && mouseX < static_cast<int>(viewportSize.x) &&
	   mouseY < static_cast<int>(viewportSize.y))
	{
		const int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
		m_HoveredEntity = pixelData == -1
							  ? Entity()
							  : Entity(static_cast<entt::entity>(pixelData), m_ActiveScene.get());
	}

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
	m_ContentBrowserPanel.OnImGuiRender();

	ImGui::Begin("Performance");
	ImGui::Text(
		"%.3f ms/frame\n%.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	ImGui::Begin("Stats");

	std::string name = "None";
	if(m_HoveredEntity)
	{
		name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
	}
	ImGui::Text("Hovered Entity: %s", name.c_str());

	auto stats = Renderer2D::GetStats();
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	ImGui::Begin("Viewport");
	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	m_ViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x,
						   viewportMinRegion.y + viewportOffset.y};
	m_ViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x,
						   viewportMaxRegion.y + viewportOffset.y};
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

	if(ImGui::BeginDragDropTarget())
	{
		if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
// FIXME: Does this actually work as expected?
#if defined HZ_PLATFORM_LINUX
			const char* path = nullptr;
			if(std::is_same_v<std::filesystem::path::value_type, char>)
			{
				path = reinterpret_cast<char*>(payload->Data);
			}
#elif defined HZ_PLATFORM_WINDOWS
			const wchar_t* path = nullptr;
			if(std::is_same_v<std::filesystem::path::value_type, wchar_t>)
			{
				path = reinterpret_cast<wchar_t*>(payload->Data);
			}
#else
#	error "Unsupported platform"
#endif
			else { HZ_CORE_ASSERT(false, "Unexpected type"); }

			// FIXME: Change how the path is handled
			OpenScene(std::filesystem::path(ASSET_PATH) / path);
		}
		ImGui::EndDragDropTarget();
	}

	// Gizmos
	Entity selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
	if(selectedEntity && m_GizmoType != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_ViewportBounds[0].x,
						  m_ViewportBounds[0].y,
						  m_ViewportBounds[1].x - m_ViewportBounds[0].x,
						  m_ViewportBounds[1].y - m_ViewportBounds[0].y);

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
	dispatcher.Dispatch<MouseButtonPressedEvent>(
		HZ_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event)
{
	if(event.GetMouseButton() == HZ_MOUSE_BUTTON_LEFT && !ImGuizmo::IsOver() &&
	   !Input::IsKeyPressed(HZ_KEY_LEFT_ALT))
	{
		if(m_ViewportHovered) m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
	}

	return false;
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
		OpenScene(filepath);
	}
}

void EditorLayer::OpenScene(const std::filesystem::path& path)
{
	NewScene();
	SceneSerializer serializer(m_ActiveScene);
	serializer.Deserialize(path.string());
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
