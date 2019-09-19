#pragma once

#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Renderer/OrthographicCamera.h"

namespace Hazel
{

class OrthographicCameraController
{
public:
	OrthographicCameraController(float aspectRatio, bool rotation = false);

	void OnUpdate(Timestep dt);
	void OnEvent(Event& e);

	OrthographicCamera& GetCamera() { return m_Camera; }
	const OrthographicCamera& GetCamera() const { return m_Camera; }

private:
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizeEvent& e);

private:
	float m_AspectRatio;
	float m_ZoomLevel = 1.f;
	OrthographicCamera m_Camera;

	bool m_Rotation;

	glm::vec3 m_CameraPosition = {0.f, 0.f, 0.f};
	float m_CameraRotation = 0.f;
	float m_CameraTranslationSpeed = 5.f;
	float m_CameraRotationSpeed = 180.f;
};

} // namespace Hazel
