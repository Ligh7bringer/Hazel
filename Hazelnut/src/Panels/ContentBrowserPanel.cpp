#include "ContentBrowserPanel.hpp"
#include "Hazel/Utils/Utils.hpp"

#include <imgui.h>

#include <algorithm>
#include <string>

namespace Hazel
{

static const std::filesystem::path s_AssetsPath{"assets"};

ContentBrowserPanel::ContentBrowserPanel()
	: m_CurrentDirectory(s_AssetsPath)
{
	m_DirectoryIcon = Texture2D::Create("resources/icons/directory.png");
	// FIXME: Load icon based on file extension
	m_FileIcon = Texture2D::Create("resources/icons/file.png");
}

void ContentBrowserPanel::OnImGuiRender()
{
	ImGui::Begin("Content Browser");

	if(m_CurrentDirectory != s_AssetsPath)
	{
		if(ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}

	static float padding = 16.f;
	static float thumbnailSize = 128;
	const float cellSize = thumbnailSize + padding;
	const float panelWidth = ImGui::GetContentRegionAvail().x;
	const int columnCount = std::max(static_cast<int>(panelWidth / cellSize), 1);
	ImGui::Columns(columnCount, 0, false);

	// FIXME: Don't iterate the filesystem every frame
	for(auto& it : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = it.path();
		auto relativePath = std::filesystem::relative(path, s_AssetsPath);
		const std::string filename = relativePath.filename().string();
		// FIXME: Full path is better because it should be unique
		ImGui::PushID(filename.c_str());

		const auto iconRendererId =
			it.is_directory() ? m_DirectoryIcon->GetRendererID() : m_FileIcon->GetRendererID();
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton(reinterpret_cast<void*>(iconRendererId),
						   {thumbnailSize, thumbnailSize},
						   {0, 1},
						   {1, 0});

		if(ImGui::BeginDragDropSource())
		{
			const std::filesystem::path::value_type* itemPath = itemPath = relativePath.c_str();
			const size_t strSize = Utils::StrSizeC(itemPath);
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, strSize, ImGuiCond_Always);
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();
		if(ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if(it.is_directory())
			{
				m_CurrentDirectory /= path.filename();
			}
		}

		ImGui::TextWrapped("%s", filename.c_str());
		ImGui::NextColumn();
		ImGui::PopID();
	}

	ImGui::Columns(1);

#if defined HZ_DEBUG
	ImGui::SliderFloat("Thumbnail Size", &thumbnailSize, 16, 512);
	ImGui::SliderFloat("Padding", &padding, 0, 32);
#endif

	ImGui::End();
}

} // namespace Hazel
