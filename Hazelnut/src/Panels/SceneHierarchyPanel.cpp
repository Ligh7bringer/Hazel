#include "SceneHierarchyPanel.hpp"
#include "Hazel/Scene/Components.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <string>

namespace Hazel
{

SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
{
	SetContext(context);
	m_DefaultTexture = Texture2D::Create(RESOURCE_PATH "icons/checker.png");
}

void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
{
	m_Context = context;
	m_SelectionContext = {};
}

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

	if(ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if(ImGui::MenuItem("Create Empty Entity"))
		{
			m_Context->CreateEntity("Empty Entity");
		}

		ImGui::EndPopup();
	}

	ImGui::End();

	ImGui::Begin("Properties");

	if(m_SelectionContext)
	{
		DrawComponents(m_SelectionContext);
	}

	ImGui::End();
}

void SceneHierarchyPanel::SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }

void SceneHierarchyPanel::DrawEntityNode(Entity entity)
{
	const auto& tag = entity.GetComponent<TagComponent>().Tag;

	ImGuiTreeNodeFlags flags = (m_SelectionContext == entity ? ImGuiTreeNodeFlags_Selected : 0) |
							   ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	const bool opened = ImGui::TreeNodeEx(
		reinterpret_cast<void*>(static_cast<uint32_t>(entity)), flags, "%s", tag.c_str());
	if(ImGui::IsItemClicked())
	{
		m_SelectionContext = entity;
	}

	bool entityDeleted = false;
	if(ImGui::BeginPopupContextItem())
	{
		if(ImGui::MenuItem("Delete Entity"))
		{
			entityDeleted = true;
		}

		ImGui::EndPopup();
	}

	if(opened)
	{
		ImGui::TreePop();
	}

	if(entityDeleted)
	{
		m_Context->DestroyEntity(entity);
		if(m_SelectionContext == entity) m_SelectionContext = {};
	}
}

static void DrawVec3Control(const std::string& label,
							glm::vec3& values,
							float resetValue = 0.f,
							float columnWidth = 100.f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text("%s", label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

	const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
	ImVec2 buttonSize = {lineHeight + 3.f, lineHeight};

	// "X" button
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.f});
	ImGui::PushFont(boldFont);
	if(ImGui::Button("X", buttonSize))
	{
		values.x = resetValue;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f, 0.f, 0.f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// "Y" button
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.f});
	ImGui::PushFont(boldFont);
	if(ImGui::Button("Y", buttonSize))
	{
		values.y = resetValue;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f, 0.f, 0.f, "%.2f");
	ImGui::PopItemWidth();
	ImGui::SameLine();

	// "Z" button
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.f});
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.f});
	ImGui::PushFont(boldFont);
	if(ImGui::Button("Z", buttonSize))
	{
		values.z = resetValue;
	}
	ImGui::PopFont();
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f, 0.f, 0.f, "%.2f");
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

template <typename ComponentTy, typename UIFunction>
static void DrawComponent(const std::string& label, Entity entity, UIFunction uiFunction)
{
	const ImGuiTreeNodeFlags treeNodeFlags =
		ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap |
		ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_FramePadding;

	if(entity.HasComponent<ComponentTy>())
	{
		auto& component = entity.GetComponent<ComponentTy>();
		const ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
		ImGui::Separator();
		const bool open =
			ImGui::TreeNodeEx(reinterpret_cast<void*>(typeid(ComponentTy).hash_code()),
							  treeNodeFlags,
							  "%s",
							  label.c_str());
		ImGui::PopStyleVar();
		ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);
		if(ImGui::Button("+", ImVec2{lineHeight, lineHeight}))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if(ImGui::BeginPopup("ComponentSettings"))
		{
			if(ImGui::MenuItem("Remove component")) removeComponent = true;

			ImGui::EndPopup();
		}

		if(open)
		{
			uiFunction(component);
			ImGui::TreePop();
		}

		if(removeComponent)
		{
			entity.RemoveComponent<ComponentTy>();
		}
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
		if(ImGui::InputText("##Tag", buffer, sizeof(buffer)))
		{
			tag = std::string(buffer);
		}
	}

	// "Add Component" button
	{
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if(ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if(ImGui::BeginPopup("AddComponent"))
		{
#define COMPONENT_CASE(LABEL, CMP_TYPE)                                                            \
	if(!m_SelectionContext.HasComponent<CMP_TYPE>())                                               \
	{                                                                                              \
		if(ImGui::MenuItem(LABEL))                                                                 \
		{                                                                                          \
			m_SelectionContext.AddComponent<CMP_TYPE>();                                           \
			ImGui::CloseCurrentPopup();                                                            \
		}                                                                                          \
	}

			COMPONENT_CASE("Camera", CameraComponent)
			COMPONENT_CASE("Sprite Renderer", SpriteRendererComponent)
			COMPONENT_CASE("Rigidbody 2D", Rigidbody2DComponent)
			COMPONENT_CASE("Box Collider 2D", BoxCollider2DComponent)

#undef COMPONENT_CASE

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();
	}

	DrawComponent<TransformComponent>("Transform", entity, [](auto& component) -> void {
		DrawVec3Control("Translation", component.Translation);

		glm::vec3 rotation = glm::degrees(component.Rotation);
		DrawVec3Control("Rotation", rotation);
		component.Rotation = glm::radians(rotation);

		DrawVec3Control("Scale", component.Scale, 1.f);
	});

	DrawComponent<CameraComponent>("Camera", entity, [](auto& component) -> void {
		auto& camera = component.Camera;

		ImGui::Checkbox("Primary", &component.Primary);

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

			float perspectiveFar = camera.GetPerspectiveFarClip();
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

			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		}
	});

	DrawComponent<SpriteRendererComponent>(
		"Sprite Renderer", entity, [this](auto& component) -> void {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Colour));

			const auto textureId = component.Texture ? component.Texture->GetRendererID()
													 : m_DefaultTexture->GetRendererID();
			if(ImGui::ImageButton(reinterpret_cast<void*>(static_cast<size_t>(textureId)),
								  ImVec2(100.f, 100.f),
								  {0, 1},
								  {1, 0}))
			{
				component.Texture = nullptr;
			}

			if(ImGui::BeginDragDropTarget())
			{
				if(const ImGuiPayload* payload =
					   ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const auto* path =
						reinterpret_cast<std::filesystem::path::value_type*>(payload->Data);
					std::filesystem::path texturePath = std::filesystem::path(ASSET_PATH) / path;
					component.Texture = Texture2D::Create(texturePath.string());
				}
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.f, 100.f);
		});

	DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [this](auto& component) -> void {
		constexpr size_t numBodyTypes = 3;
		const char* bodyTypeStrings[] = {"Static", "Dynamic", "Kinematic"};
		const char* currentBodyTypeString = bodyTypeStrings[static_cast<int>(component.Type)];
		if(ImGui::BeginCombo("Body Type", currentBodyTypeString))
		{
			for(int i = 0; i < numBodyTypes; ++i)
			{
				const bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
				if(ImGui::Selectable(bodyTypeStrings[i], isSelected))
				{
					currentBodyTypeString = bodyTypeStrings[i];
					component.Type = static_cast<Rigidbody2DComponent::BodyType>(i);
				}

				if(isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::Checkbox("Fixed rotation", &component.FixedRotation);
	});

	DrawComponent<BoxCollider2DComponent>(
		"Box Collider 2D", entity, [this](auto& component) -> void {
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
			ImGui::DragFloat("Density", &component.Density, 0.01f, 0.f, 1.f);
			ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.f, 1.f);
			ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.f, 1.f);
			ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.f);
		});
}

} // namespace Hazel
