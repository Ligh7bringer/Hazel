#include "Utils.hpp"

// #include

namespace Hazel
{

std::string FileDialogs::OpenFile(const char* description, const char* extensions)
{
	NFD::Guard nfdGuard;

	NFD::UniquePath filepath;

	nfdfilteritem_t filterItem[2] = {{description, extensions}};

	nfdresult_t result = NFD::OpenDialog(filepath, filterItem, 1);
	if(result == NFD_OKAY)
	{
		return std::string(filepath.get());
	}
	else if(result == NFD_CANCEL)
	{
		return std::string();
	}

	HZ_CORE_ERROR("Error opening file: {0}", NFD::GetError());
	return std::string();
}

std::string FileDialogs::SaveFile(const char* description, const char* extensions)
{
	NFD::Guard nfdGuard;

	NFD::UniquePath filepath;

	nfdfilteritem_t filterItem[2] = {{description, extensions}};

	nfdresult_t result = NFD::SaveDialog(filepath, filterItem, 1);
	if(result == NFD_OKAY)
	{
		return std::string(filepath.get());
	}
	else if(result == NFD_CANCEL)
	{
		return std::string();
	}

	HZ_CORE_ERROR("Error saving file: {0}", NFD::GetError());
	return std::string();
}

} // namespace Hazel