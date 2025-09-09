#include "RenderingModule.h"
#include "RenderHardwareInterface/RenderHardwareInterface.h"

bool Rendering::RenderingModule::Initialize(void* const OutputWindowPlatformHandle)
{
	return RenderHardwareInterface::Initialize(OutputWindowPlatformHandle);
}

bool Rendering::RenderingModule::ShutdownOutputWindow(void* const OutputWindowPlatformHandle)
{
	return RenderHardwareInterface::ShutdownOutputWindow(OutputWindowPlatformHandle);
}