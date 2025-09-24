#include "RenderingAbstraction/RenderHardwareInterface/RenderHardwareInterface.h"
#include "glad/glad.h"
#include "Core/ConsoleOutput.h"

namespace
{
	namespace GL_RHI
	{
		struct GL_Context
		{
			void* Context = nullptr;
			void* PlatformWindowHandle = nullptr;
		};

		struct GL_Buffer
		{
			GLuint Buffer = 0;
		};

		struct GL_Shader
		{
			GLuint Shader = 0;
		};

		struct GL_InputVertexAttributeDesc
		{
			GLuint Index = 0;
			GLint Size = 0;
			GLenum Type = GL_NONE;
			GLboolean Normalized = GL_FALSE;
			GLsizei Stride = 0;
			const void* Offset = 0;
		};

		struct GL_Pipeline
		{
			uint32_t ShaderProgram = 0;
			std::vector<GL_RHI::GL_InputVertexAttributeDesc> InputVertexAttributeLayout = {};
		};

		// Single vertex array object bound throughout gl rhi initialized duration
		GLuint VertexArrayObject = 0;

		bool LoadGLFunctions()
		{
			return gladLoadGL() != 0;
		}

		void PrintGLVersion()
		{
			CONSOLE_PRINTF("OpenGL version and driver version: %s\n", glGetString(GL_VERSION));
		}

		bool MakeContextCurrent(RenderingAbstraction::RenderHardwareInterface::Context* const Context)
		{
#ifdef PLATFORM_WINDOWS
			if (!Context)
			{
				return false;
			}

			GL_RHI::GL_Context* const GLContext = reinterpret_cast<GL_RHI::GL_Context* const>(Context);

			if (!GLContext)
			{
				return false;
			}

			return wglMakeCurrent(GetDC(static_cast<HWND>(GLContext->PlatformWindowHandle)), static_cast<HGLRC>(GLContext->Context)) == TRUE;
#else
			return false;
#endif // PLATFORM_WINDOWS
		}

		bool MakeContextNotCurrent(RenderingAbstraction::RenderHardwareInterface::Context* const Context)
		{
#ifdef PLATFORM_WINDOWS
			if (!Context)
			{
				return false;
			}

			GL_RHI::GL_Context* const GLContext = reinterpret_cast<GL_RHI::GL_Context* const>(Context);

			if (!GLContext)
			{
				return false;
			}

			return wglMakeCurrent(GetDC(static_cast<HWND>(GLContext->PlatformWindowHandle)), NULL) == TRUE;
#else
			return false;
#endif // PLATFORM_WINDOWS
		}

		bool CompileShaderSourceString(GLenum ShaderStage, const std::string& Source, uint32_t& OutShader, std::array<char, 512>& ErrorMessageBuffer)
		{
			OutShader = glCreateShader(ShaderStage);

			const char* const SourceCString = Source.c_str();
			glShaderSource(OutShader, 1, &SourceCString, nullptr);
			glCompileShader(OutShader);

			int32_t Success;
			glGetShaderiv(OutShader, GL_COMPILE_STATUS, &Success);

			if (!Success)
			{
				glGetShaderInfoLog(OutShader, ErrorMessageBuffer.size(), nullptr, ErrorMessageBuffer.data());
				return false;
			}

			return true;
		}

		GLenum GetInputVertexAttributeDataTypeGLType(const RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeValueDataType DataType)
		{
			switch (DataType)
			{
			case RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeValueDataType::Float: return GL_FLOAT;
			default: return GL_NONE;
			}
		}

		GLenum GetBufferTypeGLType(const RenderingAbstraction::RenderHardwareInterface::BufferType Type)
		{
			switch (Type)
			{
			case RenderingAbstraction::RenderHardwareInterface::BufferType::Vertex: return GL_ARRAY_BUFFER;
			case RenderingAbstraction::RenderHardwareInterface::BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
			default: return GL_NONE;
			}
		}

		GLenum GetShaderStageGLType(const RenderingAbstraction::RenderHardwareInterface::ShaderStage Stage)
		{
			switch (Stage)
			{
			case RenderingAbstraction::RenderHardwareInterface::ShaderStage::Vertex: return GL_VERTEX_SHADER;
			case RenderingAbstraction::RenderHardwareInterface::ShaderStage::Pixel: return GL_FRAGMENT_SHADER;
			default: return GL_NONE;
			}
		}
	}
}

bool RenderingAbstraction::RenderHardwareInterface::Initialize(void* const InitWindowPlatformHandle)
{
	// Create init context and make it current. A current gl context is needed to load api functions from os/driver
	if (RenderingAbstraction::RenderHardwareInterface::Context* InitContext = RenderingAbstraction::RenderHardwareInterface::NewContext(InitWindowPlatformHandle))
	{
		if (GL_RHI::MakeContextCurrent(InitContext))
		{
			// Load gl api
			if (!GL_RHI::LoadGLFunctions())
			{
				return false;
			}

			GL_RHI::PrintGLVersion();

			// Create vertex array object
			glGenVertexArrays(1, &GL_RHI::VertexArrayObject);
			glBindVertexArray(GL_RHI::VertexArrayObject);

			// Clear current context, currently set as the init context
			if (!GL_RHI::MakeContextNotCurrent(InitContext))
			{
				return false;
			}

			// Delete init context
			if (!RenderingAbstraction::RenderHardwareInterface::DeleteContext(InitContext))
			{
				return false;
			}

			return true;
		}
	}

	return false;
}

bool RenderingAbstraction::RenderHardwareInterface::Shutdown()
{
	// Destroy vertex array object
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &GL_RHI::VertexArrayObject);

	return true;
}

RenderingAbstraction::RenderHardwareInterface::Context* RenderingAbstraction::RenderHardwareInterface::NewContext(void* const WindowPlatformHandle)
{
#ifdef PLATFORM_WINDOWS
	// Get the window device context
	HDC WindowHandleToDeviceContext = GetDC(static_cast<HWND>(WindowPlatformHandle));

	// Set the pixel format in the device context
	PIXELFORMATDESCRIPTOR PFD =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	int PixelFormatNumber = ChoosePixelFormat(WindowHandleToDeviceContext, &PFD);
	if (PixelFormatNumber == 0)
	{
		// Error
		return nullptr;
	}

	SetPixelFormat(WindowHandleToDeviceContext, PixelFormatNumber, &PFD);

	// Create gl context
	GL_RHI::GL_Context* GLContext = new GL_RHI::GL_Context();
	GLContext->Context = static_cast<void*>(wglCreateContext(WindowHandleToDeviceContext));
	GLContext->PlatformWindowHandle = WindowPlatformHandle;

	return reinterpret_cast<RenderingAbstraction::RenderHardwareInterface::Context*>(GLContext);
#else
	return nullptr;
#endif // PLATFORM_WINDOWS
}

bool RenderingAbstraction::RenderHardwareInterface::DeleteContext(RenderingAbstraction::RenderHardwareInterface::Context* const Context)
{
#ifdef PLATFORM_WINDOWS

	GL_RHI::GL_Context* const GLContext = reinterpret_cast<GL_RHI::GL_Context* const>(Context);

	if (!GLContext)
	{
		return false;
	}

	// Ensure context being deleted is not the current gl context. Each gl implementation of an rhi function ensures a context is set before operating
	if (!GL_RHI::MakeContextNotCurrent(Context))
	{
		return false;
	}

	if (wglDeleteContext(static_cast<HGLRC>(GLContext->Context)) != TRUE)
	{
		return false;
	}

	delete GLContext;
	return true;
#else
	return false;
#endif // PLATFORM_WINDOWS
}

RenderingAbstraction::RenderHardwareInterface::Buffer* RenderingAbstraction::RenderHardwareInterface::NewBuffer(
	RenderingAbstraction::RenderHardwareInterface::Context* const Context,
	const RenderingAbstraction::RenderHardwareInterface::BufferType Type,
	const void* const BufferDataStart,
	const size_t BufferDataSizeBytes
)
{
	GL_RHI::GL_Buffer* const GLBuffer = new GL_RHI::GL_Buffer();

	const GLenum GLType = GL_RHI::GetBufferTypeGLType(Type);

	glGenBuffers(1, &GLBuffer->Buffer);
	glBindBuffer(GLType, GLBuffer->Buffer);
	glBufferData(GLType, BufferDataSizeBytes, BufferDataStart, GL_STATIC_DRAW);

	return reinterpret_cast<RenderingAbstraction::RenderHardwareInterface::Buffer*>(GLBuffer);
}

bool RenderingAbstraction::RenderHardwareInterface::DeleteBuffer(
	RenderingAbstraction::RenderHardwareInterface::Context* const Context,
	RenderingAbstraction::RenderHardwareInterface::Buffer* const Buffer
)
{
	GL_RHI::GL_Buffer* const GLBuffer = reinterpret_cast<GL_RHI::GL_Buffer* const>(Buffer);

	glDeleteBuffers(1, &GLBuffer->Buffer);

	delete GLBuffer;

	return true;
}

RenderingAbstraction::RenderHardwareInterface::Shader* RenderingAbstraction::RenderHardwareInterface::NewShader(
	RenderingAbstraction::RenderHardwareInterface::Context* const Context,
	const RenderingAbstraction::RenderHardwareInterface::ShaderStage Stage,
	const std::string& Source
)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	GL_RHI::GL_Shader* const GLShader = new GL_RHI::GL_Shader();

	std::array<char, 512> ErrorMessageBuffer = {};
	if (!GL_RHI::CompileShaderSourceString(GL_RHI::GetShaderStageGLType(Stage), Source, GLShader->Shader, ErrorMessageBuffer))
	{
		CONSOLE_PRINTF("Shader compilation error: %s\n", ErrorMessageBuffer.data());
		return nullptr;
	}

	return reinterpret_cast<RenderingAbstraction::RenderHardwareInterface::Shader*>(GLShader);
}

bool RenderingAbstraction::RenderHardwareInterface::DeleteShader(
	RenderingAbstraction::RenderHardwareInterface::Context* const Context,
	RenderingAbstraction::RenderHardwareInterface::Shader* const Shader
)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	GL_RHI::GL_Shader* const GLShader = reinterpret_cast<GL_RHI::GL_Shader* const>(Shader);

	glDeleteShader(GLShader->Shader);

	delete GLShader;

	return true;
}

RenderingAbstraction::RenderHardwareInterface::Pipeline* RenderingAbstraction::RenderHardwareInterface::NewPipeline(
	RenderingAbstraction::RenderHardwareInterface::Context* const Context,
	RenderingAbstraction::RenderHardwareInterface::Shader* const VertexShader,
	RenderingAbstraction::RenderHardwareInterface::Shader* const PixelShader,
	const RenderingAbstraction::RenderHardwareInterface::InputVertexAttributeLayout& InputVertexAttributeLayout
)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	GL_RHI::GL_Pipeline* GLPipeline = new GL_RHI::GL_Pipeline();

	// Link shader program
	GLPipeline->ShaderProgram = glCreateProgram();
	glAttachShader(GLPipeline->ShaderProgram, reinterpret_cast<GL_RHI::GL_Shader* const>(VertexShader)->Shader);
	glAttachShader(GLPipeline->ShaderProgram, reinterpret_cast<GL_RHI::GL_Shader* const>(PixelShader)->Shader);
	glLinkProgram(GLPipeline->ShaderProgram);

	int32_t LinkSuccess;
	std::array<char, 512> ErrorMessageBuffer = {};
	glGetProgramiv(GLPipeline->ShaderProgram, GL_LINK_STATUS, &LinkSuccess);
	if (!LinkSuccess)
	{
		CONSOLE_PRINTF("Shader program linking error: %s\n", ErrorMessageBuffer.data());
		delete GLPipeline;
		return nullptr;
	}

	// Translate input vertex attribute layout to gl input vertex attribute layout. Used when setting pipeline to use for drawing
	const size_t AttributeCount = InputVertexAttributeLayout.AttributeDescriptions.size();
	GLPipeline->InputVertexAttributeLayout.reserve(AttributeCount);

	for (uint32_t AttributeDescIndex = 0; AttributeDescIndex < AttributeCount; ++AttributeDescIndex)
	{
		GLPipeline->InputVertexAttributeLayout.emplace_back(GL_RHI::GL_InputVertexAttributeDesc
			{
				InputVertexAttributeLayout.AttributeDescriptions[AttributeDescIndex].Index,
				InputVertexAttributeLayout.AttributeDescriptions[AttributeDescIndex].ValueCount,
				GL_RHI::GetInputVertexAttributeDataTypeGLType(InputVertexAttributeLayout.AttributeDescriptions[AttributeDescIndex].ValueType),
				GL_FALSE,
				InputVertexAttributeLayout.AttributeDescriptions[AttributeDescIndex].ByteStrideToNextAttribute,
				reinterpret_cast<const void*>(InputVertexAttributeLayout.AttributeDescriptions[AttributeDescIndex].ByteOffsetFromVertexStart)
			}
		);
	}

	return reinterpret_cast<RenderingAbstraction::RenderHardwareInterface::Pipeline*>(GLPipeline);
}

bool RenderingAbstraction::RenderHardwareInterface::DeletePipeline(
	RenderingAbstraction::RenderHardwareInterface::Context* const Context,
	RenderingAbstraction::RenderHardwareInterface::Pipeline* const Pipeline
)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	// Delete gl resources
	GL_RHI::GL_Pipeline* const GLPipeline = reinterpret_cast<GL_RHI::GL_Pipeline* const>(Pipeline);

	glDeleteProgram(GLPipeline->ShaderProgram);

	// Delete pipeline struct on the heap
	delete GLPipeline;

	return true;
}

bool RenderingAbstraction::RenderHardwareInterface::Present(RenderingAbstraction::RenderHardwareInterface::Context* const Context)
{
#ifdef PLATFORM_WINDOWS
	return SwapBuffers(GetDC(static_cast<HWND>(reinterpret_cast<GL_RHI::GL_Context* const>(Context)->PlatformWindowHandle))) == TRUE;
#else
	return false;
#endif // PLATFORM_WINDOWS
}

bool RenderingAbstraction::RenderHardwareInterface::BeginFrame(RenderingAbstraction::RenderHardwareInterface::Context* const Context)
{
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	return true;
}

bool RenderingAbstraction::RenderHardwareInterface::EndFrame(RenderingAbstraction::RenderHardwareInterface::Context* const Context)
{
	return true;
}

void RenderingAbstraction::RenderHardwareInterface::SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height)
{
	glViewport(X, Y, Width, Height);
}

void RenderingAbstraction::RenderHardwareInterface::ClearColorBuffer(const float R, const float G, const float B, const float A)
{
	glClearColor(R, G, B, A);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderingAbstraction::RenderHardwareInterface::SetPipeline(RenderingAbstraction::RenderHardwareInterface::Pipeline* const Pipeline)
{
	GL_RHI::GL_Pipeline* const GLPipeline = reinterpret_cast<GL_RHI::GL_Pipeline* const>(Pipeline);

	// Bind shader program
	glUseProgram(GLPipeline->ShaderProgram);

	// Bind input vertex attributes
	const size_t AttributeCount = GLPipeline->InputVertexAttributeLayout.size();
	for (size_t i = 0; i < AttributeCount; ++i)
	{
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(
			GLPipeline->InputVertexAttributeLayout[i].Index,
			GLPipeline->InputVertexAttributeLayout[i].Size,
			GLPipeline->InputVertexAttributeLayout[i].Type,
			GLPipeline->InputVertexAttributeLayout[i].Normalized,
			GLPipeline->InputVertexAttributeLayout[i].Stride,
			GLPipeline->InputVertexAttributeLayout[i].Offset
		);
	}
}

void RenderingAbstraction::RenderHardwareInterface::DrawIndexed(
	RenderingAbstraction::RenderHardwareInterface::Buffer* const VertexBuffer,
	RenderingAbstraction::RenderHardwareInterface::Buffer* const IndexBuffer,
	const size_t IndexCount
)
{
	glBindBuffer(GL_ARRAY_BUFFER, reinterpret_cast<GL_RHI::GL_Buffer* const>(VertexBuffer)->Buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<GL_RHI::GL_Buffer* const>(IndexBuffer)->Buffer);
	glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);
}
