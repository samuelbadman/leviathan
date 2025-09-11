#pragma once

#include "Core/Module.h"
#include "Core/Platform/Uuid.h"

namespace Core
{
	class Window;

	struct WindowResizedDelegateParameters;
}

namespace Rendering
{
	class RenderingModule : public Core::Module
	{
	private:
		Core::Window* CurrentRenderOutputWindow = nullptr;

	public:
		~RenderingModule();

		bool Initialize(void* const OutputWindowPlatformHandle);
		bool CreateRenderOutputWindowResources(void* const OutputWindowPlatformHandle);
		bool DestroyRenderOutputWindowResources(void* const OutputWindowPlatformHandle);
		void SetRenderOutputWindow(Core::Window* OutputWindow);

		Core::Uuid CreateRenderMeshObject(const size_t VertexDataSize, const void* VertexData);
		void DestroyRenderMeshObject(const Core::Uuid& RenderMeshObjectUuid);

		// Render into the currently set render output window. Does nothing if there is not a render output window set
		void Render();

	private:
		void OnCurrentRenderOutputWindowResized(const Core::WindowResizedDelegateParameters& Parameters);
	};
}