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
	virtual void OnEvent(Event& e) override;

	void Begin();
	void End();

	void SetBlockEvents(bool value) { m_BlockEvents = value; }

private:
	bool m_BlockEvents = true;
	float m_Time = 0.0f;
};

} // namespace Hazel
