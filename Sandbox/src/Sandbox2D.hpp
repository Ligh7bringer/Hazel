#pragma once

#include "Hazel.hpp"
#include "ParticleSystem.hpp"

class Sandbox2D : public Hazel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Hazel::Timestep dt) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Hazel::Event& event) override;

private:
	Hazel::OrthographicCameraController m_CameraController;
	Hazel::Ref<Hazel::Texture2D> m_CheckerTexture;
	Hazel::Ref<Hazel::Texture2D> m_SpriteSheet;
	Hazel::Ref<Hazel::SubTexture2D> m_StairsTexture, m_BarrelTexture, m_TreeTexture;

	int m_NumQuads;

	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;
};
