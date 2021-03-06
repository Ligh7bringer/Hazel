#include "SceneCamera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Hazel
{

SceneCamera::SceneCamera() { RecalculateProjection(); }

void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
{
	m_ProjectionType = ProjectionType::Orthographic;
	m_OrthographicSize = size;
	m_OrthographicNear = nearClip;
	m_OrthographicFar = farClip;
	RecalculateProjection();
}

void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
{
	m_ProjectionType = ProjectionType::Perspective;
	m_PerspectiveFOV = verticalFov;
	m_PerspectiveNear = nearClip;
	m_PerspectiveFar = farClip;
	RecalculateProjection();
}

void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
{
	m_AspectRatio = (float)width / (float)height;
	RecalculateProjection();
}

void SceneCamera::RecalculateProjection()
{
	switch(m_ProjectionType)
	{
	default: HZ_CORE_ERROR("Unsupported camera projection type!"); break;
	case ProjectionType::Perspective: {
		m_Projection =
			glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		break;
	}
	case ProjectionType::Orthographic: {
		float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoBottom = -m_OrthographicSize * 0.5f;
		float orthoTop = m_OrthographicSize * 0.5f;

		m_Projection = glm::ortho(
			orthoLeft, orthoRight, orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		break;
	}
	}
}

} // namespace Hazel
