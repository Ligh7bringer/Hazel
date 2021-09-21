#pragma once

#include "Hazel.hpp"
#include "Hazel/Renderer/EditorCamera.hpp"
#include "Panels/SceneHierarchyPanel.hpp"

namespace Hazel
{

class EditorLayer : public Layer
{
public:
	EditorLayer();
	virtual ~EditorLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Timestep dt) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Event& event) override;

private:
	bool OnKeyPressed(KeyPressedEvent& event);

	void NewScene();
	void OpenScene();
	void SaveSceneAs();

private:
	OrthographicCameraController m_CameraController;
	Ref<Texture2D> m_CheckerTexture;
	Ref<Framebuffer> m_Framebuffer;

	Ref<Scene> m_ActiveScene;
	Entity m_CameraEntity, m_SecondCamera;
	bool m_PrimaryCamera = true;
	EditorCamera m_EditorCamera;
	Entity m_HoveredEntity;

	glm::vec2 m_ViewportSize{0.f, 0.f};
	glm::vec2 m_ViewportBounds[2];

	int m_NumQuads;
	bool m_ViewportFocused = false, m_ViewportHovered = false;

	int m_GizmoType = -1;
	bool m_ShouldSnap = false;

	SceneHierarchyPanel m_SceneHierarchyPanel;
};

} // namespace Hazel
