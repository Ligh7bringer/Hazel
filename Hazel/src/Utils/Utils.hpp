#pragma once

#include <filesystem>
#include <nfd.hpp>
#include <string>

namespace Hazel
{

class FileDialogs
{
public:
	static std::string OpenFile(const char* description, const char* extensions);

	static std::string SaveFile(const char* description, const char* extensions);

	static const std::string GetCurrentWorkingDirectory()
	{
		return std::filesystem::current_path().string();
	}
};

} // namespace Hazel
