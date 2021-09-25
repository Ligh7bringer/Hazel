#include "ContentBrowserPanel.hpp"

#include <imgui.h>

#include <string>

namespace Hazel
{

static const std::filesystem::path s_AssetsPath{"assets"};

ContentBrowserPanel::ContentBrowserPanel()
	: m_CurrentDirectory(s_AssetsPath)
{ }

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

	// FIXME: Don't iterate the filesystem every frame
	for(auto& it : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = it.path();
		auto relativePath = std::filesystem::relative(path, s_AssetsPath);
		const std::string filename = relativePath.filename().string();
		if(ImGui::Button(filename.c_str()))
		{
			if(it.is_directory())
			{
				m_CurrentDirectory /= path.filename();
			}
		}
	}

	ImGui::End();
}

} // namespace Hazel
