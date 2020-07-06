#pragma once

#include "Hazel.hpp"

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
	OrthographicCameraController m_CameraController;
	Ref<Texture2D> m_CheckerTexture;
	Ref<Framebuffer> m_Framebuffer;

	int m_NumQuads;
};

} // namespace Hazel