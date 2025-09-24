#pragma once

namespace Rendering
{
	namespace RenderHardwareInterface
	{
		struct Context;
		struct Buffer;
		struct Shader;
		struct Pipeline;

		enum class BufferType : uint8_t
		{
			Vertex = 0,
			Index,
			//Constant
			MAX
		};

		enum class ShaderStage : uint8_t
		{
			Vertex = 0,
			Pixel,
			MAX
		};

		enum class InputVertexAttributeValueDataType : uint8_t
		{
			Float = 0,
			MAX
		};

		struct InputVertexAttributeDesc
		{
			uint32_t Index = 0;
			int32_t ValueCount = 0;
			InputVertexAttributeValueDataType ValueType = InputVertexAttributeValueDataType::MAX;
			int32_t ByteStrideToNextAttribute = 0;
			size_t ByteOffsetFromVertexStart = 0;
		};

		struct InputVertexAttributeLayout
		{
			std::vector<InputVertexAttributeDesc> AttributeDescriptions = {};
		};

		/*
			Resource management -- create, read, update, delete
			Frame management -- When to start a frame, what swapchain image is being used etc
			Command recording -- essentially one big class that records all commands using the same API
		*/

		bool Initialize(void* const InitWindowPlatformHandle);
		bool Shutdown();

		// Resource management
		Rendering::RenderHardwareInterface::Context* NewContext(void* const WindowPlatformHandle);
		bool DeleteContext(Rendering::RenderHardwareInterface::Context* const Context);

		Rendering::RenderHardwareInterface::Buffer* NewBuffer(
			Rendering::RenderHardwareInterface::Context* const Context,
			const Rendering::RenderHardwareInterface::BufferType Type,
			const void* const BufferDataStart,
			const size_t BufferDataSizeBytes
		);
		bool DeleteBuffer(
			Rendering::RenderHardwareInterface::Context* const Context,
			Rendering::RenderHardwareInterface::Buffer* const Buffer
		);

		Rendering::RenderHardwareInterface::Shader* NewShader(
			Rendering::RenderHardwareInterface::Context* const Context,
			const Rendering::RenderHardwareInterface::ShaderStage Stage,
			const std::string& Source
		);
		bool DeleteShader(
			Rendering::RenderHardwareInterface::Context* const Context, 
			Rendering::RenderHardwareInterface::Shader* const Shader
		);

		Rendering::RenderHardwareInterface::Pipeline* NewPipeline(
			Rendering::RenderHardwareInterface::Context* const Context,
			Rendering::RenderHardwareInterface::Shader* const VertexShader,
			Rendering::RenderHardwareInterface::Shader* const PixelShader,
			const Rendering::RenderHardwareInterface::InputVertexAttributeLayout& InputVertexAttributeLayout
		);
		bool DeletePipeline(
			Rendering::RenderHardwareInterface::Context* const Context, 
			Rendering::RenderHardwareInterface::Pipeline* const Pipeline
		);

		// Frame management
		bool Present(Rendering::RenderHardwareInterface::Context* const Context);
		bool BeginFrame(Rendering::RenderHardwareInterface::Context* const Context);
		bool EndFrame(Rendering::RenderHardwareInterface::Context* const Context);

		// Command recording
		void SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height);
		void ClearColorBuffer(const float R, const float G, const float B, const float A);
		void SetPipeline(Rendering::RenderHardwareInterface::Pipeline* const Pipeline);
		void DrawIndexed(
			Rendering::RenderHardwareInterface::Buffer* const VertexBuffer,
			Rendering::RenderHardwareInterface::Buffer* const IndexBuffer,
			const size_t IndexCount
		);
	};
}