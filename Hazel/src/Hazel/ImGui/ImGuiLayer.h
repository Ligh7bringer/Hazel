#pragma once

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Layer.h"

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
