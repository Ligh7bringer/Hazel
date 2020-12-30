#include "SceneHierarchyPanel.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Hazel/Scene/Components.hpp"

namespace Hazel
{

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context) { SetContext(context); }

void SceneHierarchyPanel::SetContext(const Ref<Scene>& context) { m_Context = context; }

void SceneHierarchyPanel::OnImGuiRender()
{
	ImGui::Begin("Scene Hierarchy");

	m_Context->m_Registry.each([&](auto entityID) {
		Entity entity = {entityID, m_Context.get()};
		DrawEntityNode(entity);
	});

	if(ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
	{
		m_SelectionContext = {};
	}

	ImGui::End();

	ImGui::Begin("Properties");

	if(m_SelectionContext)
	{
		DrawComponents(m_SelectionContext);
	}

	ImGui::End();
}

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
	const auto& tag = entity.GetComponent<TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = (m_SelectionContext == entity ? ImGuiTreeNodeFlags_Selected : 0) |
							   ImGuiTreeNodeFlags_OpenOnArrow;
	const bool opened = ImGui::TreeNodeEx(
		reinterpret_cast<void*>(static_cast<uint32_t>(entity)), flags, "%s", tag.c_str());
	if(ImGui::IsItemClicked())
	{
		m_SelectionContext = entity;
	}

	if(opened)
	{
		ImGui::TreePop();
	}
}

void SceneHierarchyPanel::DrawComponents(Entity entity)
{
	if(entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer, sizeof(buffer), "%s", tag.c_str());
		if(ImGui::InputText("Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
		}
	}

	if(entity.HasComponent<TransformComponent>())
	{
		if(ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(TransformComponent).hash_code()),
							 ImGuiTreeNodeFlags_DefaultOpen,
							 "%s",
							 "Transform"))
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			ImGui::DragFloat3("Position", glm::value_ptr(tc.Translation), 0.1f);
			ImGui::TreePop();
		}
	}

	if(entity.HasComponent<CameraComponent>())
	{
		if(ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(CameraComponent).hash_code()),
							 ImGuiTreeNodeFlags_DefaultOpen,
							 "%s",
							 "Camera"))
		{
			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			ImGui::Checkbox("Primary", &cameraComponent.Primary);

			const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
			const char* currentProjectionTypeString =
				projectionTypeStrings[static_cast<int>(camera.GetProjectionType())];
			if(ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for(int i = 0; i < 2; ++i)
				{
					const bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if(ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
					}

					if(isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			if(camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
				if(ImGui::DragFloat("Vertical FOV", &perspectiveFOV))
				{
					camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveFOV));
				}

				float perspectiveNear = camera.GetPerspectiveNearClip();
				if(ImGui::DragFloat("Near", &perspectiveNear))
				{
					camera.SetPerspectiveNearClip(perspectiveNear);
				}

				float perspectiveFar = camera.GePerspectiveFarClip();
				if(ImGui::DragFloat("Far", &perspectiveFar))
				{
					camera.SetPerspectiveFarClip(perspectiveFar);
				}
			}

			if(camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = camera.GetOrthographicSize();
				if(ImGui::DragFloat("Size", &orthoSize))
				{
					camera.SetOrthographicSize(orthoSize);
				}

				float orthoNear = camera.GetOrthographicNearClip();
				if(ImGui::DragFloat("Near", &orthoNear))
				{
					camera.SetOrthographicNearClip(orthoNear);
				}

				float orthoFar = camera.GetOrthographicFarClip();
				if(ImGui::DragFloat("Far", &orthoFar))
				{
					camera.SetOrthographicFarClip(orthoFar);
				}

				ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
			}

			ImGui::TreePop();
		}
	}

	if(entity.HasComponent<SpriteRendererComponent>())
	{
		if(ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(SpriteRendererComponent).hash_code()),
							 ImGuiTreeNodeFlags_DefaultOpen,
							 "%s",
							 "Sprite Renderer"))
		{
			auto& src = entity.GetComponent<SpriteRendererComponent>();
			ImGui::ColorEdit4("Color", glm::value_ptr(src.Colour));
			ImGui::TreePop();
		}
	}
}

} // namespace Hazel
