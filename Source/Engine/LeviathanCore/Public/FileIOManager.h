#pragma once

namespace Core
{
	class FileIOManager
	{
	public:
		std::string ReadFileToString(const std::string& File);
	};
}