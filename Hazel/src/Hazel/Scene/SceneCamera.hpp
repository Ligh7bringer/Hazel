#pragma once

#include "Hazel/Renderer/Camera.hpp"

namespace Hazel
{

class SceneCamera : public Camera
{
public:
	SceneCamera();
	virtual ~SceneCamera() = default;

	void SetOrthographic(float size, float nearClip, float farClip);
	void SetViewportSize(uint32_t width, uint32_t height);

	float GetOrthographicSize() const { return m_OrthographicSize; }
	void SetOrthographicSize(float value);

private:
	void RecalculateProjection();

private:
	float m_OrthographicSize{10.f};
	float m_OrthographicNear{-1.f};
	float m_OrthographicFar{1.f};

	float m_AspectRatio{0.f};
};

} // namespace Hazel
