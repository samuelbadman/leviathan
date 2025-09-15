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
	/*
	* The rendering module is designed so that how objects are rendered is defined by the rendering module and what is rendered is defined by the client application.
	*/
	class RenderingModule : public Core::Module
	{
	private:
		Core::Uuid StaticMeshRenderPipelineUuid = {};

		Core::Window* CurrentRenderOutputWindow = nullptr;

	public:
		~RenderingModule();

		bool Initialize(void* const OutputWindowPlatformHandle);
		bool CreateRenderOutputWindowResources(void* const OutputWindowPlatformHandle);
		bool DestroyRenderOutputWindowResources(void* const OutputWindowPlatformHandle);
		void SetRenderOutputWindow(Core::Window* OutputWindow);

		Core::Uuid AllocateStaticRenderMesh(const size_t VertexDataSize, const void* VertexData);
		void ReleaseStaticRenderMesh(const Core::Uuid& StaticRenderMeshUuid);

		// Render into the currently set render output window. Does nothing if there is not a render output window set
		void Render();

	private:
		void OnCurrentRenderOutputWindowResized(const Core::WindowResizedDelegateParameters& Parameters);
	};
}