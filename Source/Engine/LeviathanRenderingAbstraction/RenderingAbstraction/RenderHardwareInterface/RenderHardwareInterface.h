#pragma once

namespace RenderingAbstraction
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
			Float3 = 0,
			MAX
		};

		struct InputVertexAttributeDesc
		{
			InputVertexAttributeValueDataType ValueType = InputVertexAttributeValueDataType::MAX;
			int32_t StrideToSameAttributeInNextVertexBytes = 0;
			uint32_t OffsetFromVertexStartBytes = 0;
		};

		struct InputVertexAttributeLayout
		{
			std::vector<InputVertexAttributeDesc> AttributeDescriptions = {};
		};

		enum class PrimitiveTopologyType : uint8_t
		{
			Triangle = 0,
			//Line,
			//Point,
			MAX
		};

		/*
			Resource management -- create, read, update, delete
			Frame management -- When to start a frame, what swapchain image is being used etc
			Command recording -- essentially one big class that records all commands using the same API
		*/

		bool Initialize(void* const InitWindowPlatformHandle);
		bool Shutdown();

		// Resource management
		RenderingAbstraction::RenderHardwareInterface::Context* NewContext(void* const WindowPlatformHandle);
		bool DeleteContext(RenderingAbstraction::RenderHardwareInterface::Context* const Context);

		RenderingAbstraction::RenderHardwareInterface::Buffer* NewBuffer(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			const RenderingAbstraction::RenderHardwareInterface::BufferType Type,
			const void* const BufferDataStart,
			const size_t BufferDataSizeBytes
		);
		bool DeleteBuffer(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			RenderingAbstraction::RenderHardwareInterface::Buffer* const Buffer
		);

		RenderingAbstraction::RenderHardwareInterface::Shader* NewShader(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			const RenderingAbstraction::RenderHardwareInterface::ShaderStage Stage,
			const std::string& Source
		);
		bool DeleteShader(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			RenderingAbstraction::RenderHardwareInterface::Shader* const Shader
		);

		RenderingAbstraction::RenderHardwareInterface::Pipeline* NewPipeline(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			RenderingAbstraction::RenderHardwareInterface::Shader* const VertexShader,
			RenderingAbstraction::RenderHardwareInterface::Shader* const PixelShader,
			const RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeLayout& InputVertexAttributeLayout,
			const RenderingAbstraction::RenderHardwareInterface::PrimitiveTopologyType PrimitiveTopologyType
		);
		bool DeletePipeline(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			RenderingAbstraction::RenderHardwareInterface::Pipeline* const Pipeline
		);

		// Frame management
		bool Present(RenderingAbstraction::RenderHardwareInterface::Context* const Context);
		bool BeginFrame(RenderingAbstraction::RenderHardwareInterface::Context* const Context);
		bool EndFrame(RenderingAbstraction::RenderHardwareInterface::Context* const Context);

		// Command recording
		void SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height);
		void SetPipeline(RenderingAbstraction::RenderHardwareInterface::Pipeline* const Pipeline);
		void SetPrimitiveTopology(const RenderingAbstraction::RenderHardwareInterface::PrimitiveTopologyType Type);
		void ClearColorBuffer(const float R, const float G, const float B, const float A);
		void DrawIndexed(
			RenderingAbstraction::RenderHardwareInterface::Buffer* const VertexBuffer,
			const size_t VertexStrideBytes,
			RenderingAbstraction::RenderHardwareInterface::Buffer* const IndexBuffer,
			const size_t IndexCount
		);
	};
}