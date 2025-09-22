#pragma once

namespace Rendering
{
	namespace RenderHardwareInterface
	{
		struct Context;
		struct Mesh;
		struct Pipeline;

		struct MeshVertex
		{
			std::array<float, 3> Position = {};
		};

		/*
			Resource management -- create, read, update, delete
			Frame management -- When to start a frame, what swapchain image is being used etc
			Command recording -- essentially one big class that records all commands using the same API
		*/

		std::string GetShaderSourceFileExtension();

		bool Initialize(void* const InitWindowPlatformHandle);

		// Resource management
		Rendering::RenderHardwareInterface::Context* NewContext(void* const WindowPlatformHandle);
		bool DeleteContext(Rendering::RenderHardwareInterface::Context* const Context);
		// TODO: Accept vertex data in any format and expose agnostic element/attribute description as input when creating new render geometry
		Rendering::RenderHardwareInterface::Mesh* NewMesh(Rendering::RenderHardwareInterface::Context* const Context, const std::vector<MeshVertex>& Vertices);
		bool DeleteMesh(Rendering::RenderHardwareInterface::Context* const Context, Rendering::RenderHardwareInterface::Mesh* const Mesh);
		// TODO: Separate shader compilation into its own step to support d3d12/vulkan precompiled shader workflow.
		Rendering::RenderHardwareInterface::Pipeline* NewPipeline(Rendering::RenderHardwareInterface::Context* const Context,
			const std::string& VertexShaderSource,
			const std::string& PixelShaderSource);
		bool DeletePipeline(Rendering::RenderHardwareInterface::Context* const Context, Rendering::RenderHardwareInterface::Pipeline* const Pipeline);

		// Frame management
		bool Present(Rendering::RenderHardwareInterface::Context* const Context);
		bool BeginFrame(Rendering::RenderHardwareInterface::Context* const Context);
		bool EndFrame(Rendering::RenderHardwareInterface::Context* const Context);

		// Command recording
		void SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height);
		void ClearColorBuffer(const float R, const float G, const float B, const float A);
		void SetPipeline(Rendering::RenderHardwareInterface::Pipeline* const Pipeline);
		void DrawMesh(Rendering::RenderHardwareInterface::Mesh* const Mesh);
	};
}