#pragma once

#include <filesystem>

namespace Hazel
{

class ContentBrowserPanel
{

	std::filesystem::path m_CurrentDirectory;

public:
	ContentBrowserPanel();

	void OnImGuiRender();
};

} // namespace Hazel
