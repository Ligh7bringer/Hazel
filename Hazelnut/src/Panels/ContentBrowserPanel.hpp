#pragma once

#include "Hazel/Renderer/Texture.hpp"

#include <filesystem>

namespace Hazel
{

class ContentBrowserPanel
{
	std::filesystem::path m_CurrentDirectory;
	Ref<Texture2D> m_DirectoryIcon, m_FileIcon;

public:
	ContentBrowserPanel();

	void OnImGuiRender();
};

} // namespace Hazel
