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

namespace Utils
{

template <typename T>
inline size_t StrSizeC(T str);

template <>
inline size_t StrSizeC(const char* str)
{
	return (strlen(str) + 1) * sizeof(char);
}

template <>
inline size_t StrSizeC(const wchar_t* str)
{
	return (wcslen(str) + 1) * sizeof(wchar_t);
}

} // namespace Utils

} // namespace Hazel
