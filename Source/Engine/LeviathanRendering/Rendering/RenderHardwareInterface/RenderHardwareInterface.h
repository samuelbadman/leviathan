#pragma once

namespace Rendering
{
	namespace RenderHardwareInterface
	{
		struct Context;
		struct Mesh;
		struct Pipeline;

		enum class VertexInputAttributeValueDataType : uint8_t
		{
			Float3 = 0,
			MAX
		};

		struct VertexInputAttribute
		{
			uint32_t Index = 0;
			int32_t ValueCount = 0;
			VertexInputAttributeValueDataType ValueType = VertexInputAttributeValueDataType::MAX;
			size_t ByteStrideToNextAttribute = 0;
			size_t ByteOffsetFromVertexStart = 0;
		};

		struct VertexInputAttributeLayout
		{
			std::vector<VertexInputAttribute> Attributes = {};
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

		Rendering::RenderHardwareInterface::Mesh* NewMesh(
			Rendering::RenderHardwareInterface::Context* const Context,
			const void* const VertexData,
			const size_t VertexDataSizeBytes,
			const VertexInputAttributeLayout& AttributeLayout,
			const uint32_t* const IndexData,
			const size_t IndexCount
		);
		bool DeleteMesh(Rendering::RenderHardwareInterface::Context* const Context, Rendering::RenderHardwareInterface::Mesh* const Mesh);

		Rendering::RenderHardwareInterface::Pipeline* NewPipeline(
			Rendering::RenderHardwareInterface::Context* const Context,
			const std::string& VertexShaderSource,
			const std::string& PixelShaderSource
		);
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