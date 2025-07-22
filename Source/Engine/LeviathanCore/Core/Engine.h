#pragma once

namespace Core
{
	class Application;

	class Engine
	{
	private:
		std::unique_ptr<Application> ApplicationInstance = nullptr;
		bool RunningApplicationInstance = false;

	public:
		void BeginApplication(std::unique_ptr<Application> pApplication);

	private:
		void BeginApplicationMainLoop();
	};
}