#pragma once

#include "Hazel/Renderer/Camera.hpp"

namespace Hazel
{

class SceneCamera : public Camera
{
public:
	enum class ProjectionType : int
	{
		Perspective = 0,
		Orthographic
	};

	SceneCamera();
	virtual ~SceneCamera() = default;

	void SetOrthographic(float size, float nearClip, float farClip);
	void SetPerspective(float verticalFov, float nearClip, float farClip);

	void SetViewportSize(uint32_t width, uint32_t height);

	ProjectionType GetProjectionType() const { return m_ProjectionType; }
	void SetProjectionType(ProjectionType type)
	{
		m_ProjectionType = type;
		RecalculateProjection();
	}

	// Perspective camera
	float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
	void SetPerspectiveVerticalFOV(float value)
	{
		m_PerspectiveFOV = value;
		RecalculateProjection();
	}

	float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
	void SetPerspectiveNearClip(float value)
	{
		m_PerspectiveNear = value;
		RecalculateProjection();
	}

	float GePerspectiveFarClip() const { return m_PerspectiveFar; }
	void SetPerspectiveFarClip(float value)
	{
		m_PerspectiveFar = value;
		RecalculateProjection();
	}

	// Orthographic camera
	float GetOrthographicSize() const { return m_OrthographicSize; }
	void SetOrthographicSize(float value)
	{
		m_OrthographicSize = value;
		RecalculateProjection();
	}

	float GetOrthographicNearClip() const { return m_OrthographicNear; }
	void SetOrthographicNearClip(float value)
	{
		m_OrthographicNear = value;
		RecalculateProjection();
	}

	float GetOrthographicFarClip() const { return m_OrthographicFar; }
	void SetOrthographicFarClip(float value)
	{
		m_OrthographicFar = value;
		RecalculateProjection();
	}

private:
	void RecalculateProjection();

private:
	ProjectionType m_ProjectionType{ProjectionType::Orthographic};

	float m_PerspectiveFOV{glm::radians(45.f)};
	float m_PerspectiveNear{0.01f};
	float m_PerspectiveFar{1000.f};

	float m_OrthographicSize{10.f};
	float m_OrthographicNear{-1.f};
	float m_OrthographicFar{1.f};

	float m_AspectRatio{0.f};
};

} // namespace Hazel
