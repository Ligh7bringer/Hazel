#pragma once

#include "Hazel/Core/Core.hpp"
#include "Hazel/Scene/Entity.hpp"
#include "Hazel/Scene/Scene.hpp"

namespace Hazel
{

class SceneHierarchyPanel
{
public:
	SceneHierarchyPanel() = default;
	explicit SceneHierarchyPanel(const Ref<Scene>& context);

	void SetContext(const Ref<Scene>& context);

	void OnImGuiRender();

	Entity GetSelectedEntity() const { return m_SelectionContext; }

private:
	void DrawEntityNode(Entity entity);
	void DrawComponents(Entity entity);

private:
	Ref<Scene> m_Context;
	Entity m_SelectionContext;
};

} // namespace Hazel
