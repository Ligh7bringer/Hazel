#pragma once

#include "Hazel.h"

class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Hazel::Timestep dt) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Hazel::Event& event) override;

private:
	Hazel::Ref<Hazel::Shader> m_FlatColShader;
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	glm::vec4 m_SquareColor = {.2f, .3f, .8f, 1.f};
	Hazel::OrthographicCameraController m_CameraController;
};
