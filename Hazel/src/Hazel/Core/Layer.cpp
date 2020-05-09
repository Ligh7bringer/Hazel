#include "Layer.hpp"

namespace Hazel
{

Layer::Layer(const std::string& name)
	: m_DebugName(name)
{}

void Layer::OnAttach() {}
void Layer::OnDetach() {}
void Layer::OnUpdate(Timestep dt) {}
void Layer::OnImGuiRender() {}
void Layer::OnEvent(Event& event) {}

} // namespace Hazel
