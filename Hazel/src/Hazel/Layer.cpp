#include "hzpch.h"
#include "Layer.h"

namespace Hazel {

	Layer::Layer(const std::string& name) : m_DebugName(name) {}

	Layer::~Layer() {}

	void Layer::OnAttach() {}
	void Layer::OnDetach() {}
	void Layer::OnUpdate() {}
	void Layer::OnImGuiRender() {}
	void Layer::OnEvent(Event& event) {}

}