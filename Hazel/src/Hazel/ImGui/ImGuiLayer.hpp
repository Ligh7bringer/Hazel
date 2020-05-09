#pragma once

#include "Hazel/Core/Layer.hpp"
#include "Hazel/Events/ApplicationEvent.hpp"
#include "Hazel/Events/KeyEvent.hpp"
#include "Hazel/Events/MouseEvent.hpp"

#include <glm/glm.hpp>

namespace Hazel
{

class HAZEL_API ImGuiLayer : public Layer
{
public:
	ImGuiLayer();
	~ImGuiLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnImGuiRender() override;

	void Begin();
	void End();

private:
	float m_Time = 0.0f;
};

} // namespace Hazel
