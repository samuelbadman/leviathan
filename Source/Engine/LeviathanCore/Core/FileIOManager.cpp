#include "FileIOManager.h"

std::string Core::FileIOManager::ReadFileToString(const std::string& File)
{
	std::ifstream InFile(File);
	return std::string(std::istreambuf_iterator<char>(InFile), std::istreambuf_iterator<char>());
}
