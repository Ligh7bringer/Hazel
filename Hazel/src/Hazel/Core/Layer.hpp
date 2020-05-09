#pragma once

#include "Hazel/Core/Core.hpp"
#include "Hazel/Core/Timestep.hpp"
#include "Hazel/Events/Event.hpp"

namespace Hazel
{

class HAZEL_API Layer
{
public:
	Layer(const std::string& name = "Layer");
	virtual ~Layer() = default;

	virtual void OnAttach();
	virtual void OnDetach();
	virtual void OnUpdate(Timestep dt);
	virtual void OnImGuiRender();
	virtual void OnEvent(Event& event);

	const std::string& GetName() const { return m_DebugName; }

protected:
	std::string m_DebugName;
};

} // namespace Hazel
