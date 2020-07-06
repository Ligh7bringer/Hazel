#pragma once

#include "Hazel/Core/Timestep.hpp"
#include "Hazel/Events/ApplicationEvent.hpp"
#include "Hazel/Events/MouseEvent.hpp"
#include "Hazel/Renderer/OrthographicCamera.hpp"

namespace Hazel
{

struct OrthographicCameraBounds
{
	float Left, Right;
	float Bottom, Top;

	float GetWidth() { return Right - Left; }
	float GetHeight() { return Top - Bottom; }
};

class OrthographicCameraController
{
public:
	explicit OrthographicCameraController(float aspectRatio, bool rotation = false);

	void OnUpdate(Timestep dt);
	void OnEvent(Event& e);

	void Resize(float width, float height);

	OrthographicCamera& GetCamera() { return m_Camera; }
	const OrthographicCamera& GetCamera() const { return m_Camera; }

	float GetZoomLevel() const { return m_ZoomLevel; }
	void SetZoomLevel(float level) { m_ZoomLevel = level; }

	const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }

private:
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizeEvent& e);

private:
	float m_AspectRatio;
	float m_ZoomLevel = 1.0f;
	OrthographicCameraBounds m_Bounds;
	OrthographicCamera m_Camera;

	bool m_Rotation;

	glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
	float m_CameraRotation = 0.0f;
	float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
};

} // namespace Hazel
