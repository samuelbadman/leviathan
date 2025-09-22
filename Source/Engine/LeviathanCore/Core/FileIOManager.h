#pragma once

namespace Core
{
	class FileIOManager
	{
	public:
		std::string ReadDiskFileToString(const std::string& File);
	};
}