#include "ContentBrowserPanel.hpp"

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

		const auto dotIdx = filename.rfind(".");
		std::string extension{"none"};
		if(dotIdx != std::string::npos)
		{
			extension = filename.substr(dotIdx, filename.size() - dotIdx);
		}
		if(extension == ".hazel" && ImGui::BeginDragDropSource())
		{
			const std::filesystem::path::value_type* itemPath = itemPath = relativePath.c_str();
			size_t strSize = 0;
			// FIXME: Refactor this
#if defined HZ_PLATFORM_LINUX
			if(std::is_same_v<std::filesystem::path::value_type, char>)
			{
				// Presumably char is used on Linux
				strSize = (strlen(itemPath) + 1) * sizeof(char);
			}
#elif defined HZ_PLATFORM_WINDOWS
			if(std::is_same_v<std::filesystem::path::value_type, wchar_t>)
			{
				// Presumably w_chr_t is used on Windows
				strSize = (wcslen(itemPath) + 1) * sizeof(wchar_t);
			}
#else
#	error "Unsupported platform"
#endif
			else { HZ_CORE_ASSERT(false, "Unsupported type"); }

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
