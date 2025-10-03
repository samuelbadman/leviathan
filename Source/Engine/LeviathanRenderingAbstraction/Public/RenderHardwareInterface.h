#pragma once

namespace RenderingAbstraction
{
	namespace RenderHardwareInterface
	{
		struct Context;
		struct Buffer;
		struct Shader;
		struct Pipeline;
		struct Texture;

		enum class BufferType : uint8_t
		{
			VERTEX = 0,
			INDEX,
			CONSTANT,
			MAX
		};

		enum class ShaderStage : uint8_t
		{
			VERTEX = 0,
			PIXEL,
			MAX
		};

		enum class InputVertexAttributeValueDataType : uint8_t
		{
			FLOAT2 = 0,
			FLOAT3,
			FLOAT4,
			MAX
		};

		struct InputVertexAttributeDesc
		{
			uint32_t Index = 0;
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
			TRIANGLE = 0,
			//LINE,
			//POINT,
			MAX
		};

		enum class Format : uint8_t
		{
			R8G8B8 = 0, // 3 color channel Red, Green, Blue 24-bit image, 8 bits per color channel
			R8G8B8A8, // 4 color channel Red, Green, Blue, Alpha 32-bit image, 8 bits per color channel
			R8, // 1 color channel Red 8-bit image, 8 bits per color channel
			R8G8, // 2 color channel Red, Green 16-bit image, 8 bits per color channel
			MAX
		};

		enum class TextureResourceDimension : uint8_t
		{
			TEXTURE2D = 0,
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
		bool UpdateConstantBufferData(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			RenderingAbstraction::RenderHardwareInterface::Buffer* ConstantBuffer,
			const size_t UpdateStartOffsetIntoConstantBufferBytes,
			const void* const UpdateDataStart,
			const size_t UpdateDataSizeBytes
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

		RenderingAbstraction::RenderHardwareInterface::Texture* NewTexture(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			const RenderingAbstraction::RenderHardwareInterface::TextureResourceDimension Dimension,
			const RenderingAbstraction::RenderHardwareInterface::Format Format,
			const size_t Width,
			const size_t Height,
			//const RenderingAbstraction::RenderHardwareInterface::Format PixelDataMemoryLayout,
			const void* const PixelData, // Pixel data is expected as unsigned bytes (unsigned char, uint8_t)
			const bool GenerateMipmaps
		);
		bool DeleteTexture(
			RenderingAbstraction::RenderHardwareInterface::Context* const Context,
			RenderingAbstraction::RenderHardwareInterface::Texture* const Texture
		);

		// Frame management
		bool Present(RenderingAbstraction::RenderHardwareInterface::Context* const Context);
		bool BeginFrame(RenderingAbstraction::RenderHardwareInterface::Context* const Context);
		bool EndFrame(RenderingAbstraction::RenderHardwareInterface::Context* const Context);

		// Command recording
		void SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height);
		void SetPipeline(RenderingAbstraction::RenderHardwareInterface::Pipeline* const Pipeline);
		void SetPrimitiveTopology(const RenderingAbstraction::RenderHardwareInterface::PrimitiveTopologyType Type);
		void SetConstantBuffer(const uint32_t Binding, RenderingAbstraction::RenderHardwareInterface::Buffer* const ConstantBuffer);
		void SetConstantBuffer(
			const uint32_t Binding,
			RenderingAbstraction::RenderHardwareInterface::Buffer* const ConstantBuffer,
			const size_t OffsetBindingStartBytes, // Offset into the constant buffer to start the binding at
			const size_t BufferBoundRangeSizeBytes // Size length from OffsetBindingStartBytes into the constant buffer to bind to the binding
		);
		void ClearColorBuffer(const float R, const float G, const float B, const float A);
		void DrawIndexed(
			RenderingAbstraction::RenderHardwareInterface::Buffer* const VertexBuffer,
			const size_t VertexStrideBytes,
			RenderingAbstraction::RenderHardwareInterface::Buffer* const IndexBuffer,
			const size_t IndexCount
		);
	};
}