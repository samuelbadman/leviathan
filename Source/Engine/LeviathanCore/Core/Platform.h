#pragma once

namespace Core
{
	namespace Platform
	{
		bool Initialize();
		void PerFrameUpdate();
		double GetFrameMicroseconds();
		bool CreateConsole();
		bool RemoveConsole();
	}
}