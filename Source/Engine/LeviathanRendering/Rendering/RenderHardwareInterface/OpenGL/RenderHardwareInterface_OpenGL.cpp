#include "Rendering/RenderHardwareInterface/RenderHardwareInterface.h"
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

		struct GL_VertexBuffer
		{
			GLuint VBO = 0;
		};

		struct GL_ElementBuffer
		{
			GLuint EBO = 0;
			GLuint ElementCount = 0;
		};

		struct GL_Pipeline
		{
			uint32_t ShaderProgram = 0;
		};

		// Single vertex array object bound throughout gl rhi initialized duration
		GLuint VAO = 0;

		bool LoadGLFunctions()
		{
			return gladLoadGL() != 0;
		}

		void PrintGLVersion()
		{
			CONSOLE_PRINTF("OpenGL version and driver version: %s\n", glGetString(GL_VERSION));
		}

		bool MakeContextCurrent(Rendering::RenderHardwareInterface::Context* const Context)
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

		bool MakeContextNotCurrent(Rendering::RenderHardwareInterface::Context* const Context)
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

		GLenum GetVertexInputAttributeDataTypeGLType(const Rendering::RenderHardwareInterface::VertexInputAttributeValueDataType DataType)
		{
			switch (DataType)
			{
			case Rendering::RenderHardwareInterface::VertexInputAttributeValueDataType::Float3: return GL_FLOAT;

			case Rendering::RenderHardwareInterface::VertexInputAttributeValueDataType::MAX:
			default: return GL_NONE;
			}
		}
	}
}

bool Rendering::RenderHardwareInterface::Initialize(void* const InitWindowPlatformHandle)
{
	// Create init context and make it current. A current gl context is needed to load api functions from os/driver
	if (Rendering::RenderHardwareInterface::Context* InitContext = Rendering::RenderHardwareInterface::NewContext(InitWindowPlatformHandle))
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
			glGenVertexArrays(1, &GL_RHI::VAO);
			glBindVertexArray(GL_RHI::VAO);

			// Clear current context, currently set as the init context
			if (!GL_RHI::MakeContextNotCurrent(InitContext))
			{
				return false;
			}

			// Delete init context
			if (!Rendering::RenderHardwareInterface::DeleteContext(InitContext))
			{
				return false;
			}

			return true;
		}
	}

	return false;
}

bool Rendering::RenderHardwareInterface::Shutdown()
{
	// Destroy vertex array object
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &GL_RHI::VAO);

	return true;
}

Rendering::RenderHardwareInterface::Context* Rendering::RenderHardwareInterface::NewContext(void* const WindowPlatformHandle)
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

	return reinterpret_cast<Rendering::RenderHardwareInterface::Context*>(GLContext);
#else
	return nullptr;
#endif // PLATFORM_WINDOWS
}

bool Rendering::RenderHardwareInterface::DeleteContext(Rendering::RenderHardwareInterface::Context* const Context)
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

Rendering::RenderHardwareInterface::Buffer* Rendering::RenderHardwareInterface::NewVertexBuffer(
	Rendering::RenderHardwareInterface::Context* const Context, 
	const void* const VertexDataStart,
	const size_t VertexDataSizeBytes
)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	// Create new gl vertex buffer structure
	GL_RHI::GL_VertexBuffer* const GLVertexBuffer = new GL_RHI::GL_VertexBuffer();

	// Create vertex buffer
	glGenBuffers(1, &GLVertexBuffer->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, GLVertexBuffer->VBO);
	glBufferData(GL_ARRAY_BUFFER, VertexDataSizeBytes, VertexDataStart, GL_STATIC_DRAW);

	return reinterpret_cast<Rendering::RenderHardwareInterface::Buffer*>(GLVertexBuffer);
}

bool Rendering::RenderHardwareInterface::DeleteVertexBuffer(Rendering::RenderHardwareInterface::Context* const Context, Rendering::RenderHardwareInterface::Buffer* const Buffer)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	GL_RHI::GL_VertexBuffer* const GLVertexBuffer = reinterpret_cast<GL_RHI::GL_VertexBuffer* const>(Buffer);

	glDeleteBuffers(1, &GLVertexBuffer->VBO);

	delete GLVertexBuffer;

	return true;
}

Rendering::RenderHardwareInterface::Buffer* Rendering::RenderHardwareInterface::NewIndexBuffer(
	Rendering::RenderHardwareInterface::Context* const Context,
	const uint32_t* const IndexData, const size_t IndexCount
)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	GL_RHI::GL_ElementBuffer* const GLElementBuffer = new GL_RHI::GL_ElementBuffer();

	glGenBuffers(1, &GLElementBuffer->EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLElementBuffer->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(uint32_t), IndexData, GL_STATIC_DRAW);
	GLElementBuffer->ElementCount = IndexCount;

	return reinterpret_cast<Rendering::RenderHardwareInterface::Buffer*>(GLElementBuffer);
}

bool Rendering::RenderHardwareInterface::DeleteIndexBuffer(Rendering::RenderHardwareInterface::Context* const Context, Rendering::RenderHardwareInterface::Buffer* const Buffer)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	GL_RHI::GL_ElementBuffer* const GLElementBuffer = reinterpret_cast<GL_RHI::GL_ElementBuffer* const>(Buffer);

	glDeleteBuffers(1, &GLElementBuffer->EBO);

	delete GLElementBuffer;

	return false;
}

Rendering::RenderHardwareInterface::Pipeline* Rendering::RenderHardwareInterface::NewPipeline(
	Rendering::RenderHardwareInterface::Context* const Context,
	const std::string& VertexShaderSource,
	const std::string& PixelShaderSource
)
{
	// Set context
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	std::array<char, 512> ErrorMessageBuffer = {};

	uint32_t VertexShader;
	if (!GL_RHI::CompileShaderSourceString(GL_VERTEX_SHADER, VertexShaderSource, VertexShader, ErrorMessageBuffer))
	{
		CONSOLE_PRINTF("Vertex shader source compilation error: %s\n", ErrorMessageBuffer.data());
		return nullptr;
	}

	uint32_t FragmentShader;
	if (!GL_RHI::CompileShaderSourceString(GL_FRAGMENT_SHADER, PixelShaderSource, FragmentShader, ErrorMessageBuffer))
	{
		CONSOLE_PRINTF("Pixel shader source compilation error: %s\n", ErrorMessageBuffer.data());
		return nullptr;
	}

	GL_RHI::GL_Pipeline* GLPipeline = new GL_RHI::GL_Pipeline();

	GLPipeline->ShaderProgram = glCreateProgram();
	glAttachShader(GLPipeline->ShaderProgram, VertexShader);
	glAttachShader(GLPipeline->ShaderProgram, FragmentShader);
	glLinkProgram(GLPipeline->ShaderProgram);

	glDeleteShader(VertexShader);
	glDeleteShader(FragmentShader);

	int32_t LinkSuccess;
	glGetProgramiv(GLPipeline->ShaderProgram, GL_LINK_STATUS, &LinkSuccess);
	if (!LinkSuccess)
	{
		CONSOLE_PRINTF("Shader source compilation error: %s\n", ErrorMessageBuffer.data());
		delete GLPipeline;
		return nullptr;
	}

	return reinterpret_cast<Rendering::RenderHardwareInterface::Pipeline*>(GLPipeline);
}

bool Rendering::RenderHardwareInterface::DeletePipeline(Rendering::RenderHardwareInterface::Context* const Context, Rendering::RenderHardwareInterface::Pipeline* const Pipeline)
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

bool Rendering::RenderHardwareInterface::Present(Rendering::RenderHardwareInterface::Context* const Context)
{
#ifdef PLATFORM_WINDOWS
	return SwapBuffers(GetDC(static_cast<HWND>(reinterpret_cast<GL_RHI::GL_Context* const>(Context)->PlatformWindowHandle))) == TRUE;
#else
	return false;
#endif // PLATFORM_WINDOWS
}

bool Rendering::RenderHardwareInterface::BeginFrame(Rendering::RenderHardwareInterface::Context* const Context)
{
	if (!GL_RHI::MakeContextCurrent(Context))
	{
		return false;
	}

	return true;
}

bool Rendering::RenderHardwareInterface::EndFrame(Rendering::RenderHardwareInterface::Context* const Context)
{
	return true;
}

void Rendering::RenderHardwareInterface::SetViewport(const int32_t X, const int32_t Y, const int32_t Width, const int32_t Height)
{
	glViewport(X, Y, Width, Height);
}

void Rendering::RenderHardwareInterface::ClearColorBuffer(const float R, const float G, const float B, const float A)
{
	glClearColor(R, G, B, A);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Rendering::RenderHardwareInterface::SetPipeline(Rendering::RenderHardwareInterface::Pipeline* const Pipeline)
{
	glUseProgram(reinterpret_cast<GL_RHI::GL_Pipeline* const>(Pipeline)->ShaderProgram);
}

void Rendering::RenderHardwareInterface::DrawIndexed(
	Rendering::RenderHardwareInterface::Buffer* const VertexBuffer,
	Rendering::RenderHardwareInterface::Buffer* const IndexBuffer,
	const Rendering::RenderHardwareInterface::VertexInputAttributeLayout& AttributeLayout
)
{
	GL_RHI::GL_VertexBuffer* const GLVertexBuffer = reinterpret_cast<GL_RHI::GL_VertexBuffer* const>(VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, GLVertexBuffer->VBO);

	GL_RHI::GL_ElementBuffer* const GLElementBuffer = reinterpret_cast<GL_RHI::GL_ElementBuffer* const>(IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLElementBuffer->EBO);

	// TODO: Move into pipeline/tie with shaders. Vertex attribute layout
	const size_t AttributeCount = AttributeLayout.AttributeDescriptions.size();
	for (uint32_t AttributeIndex = 0; AttributeIndex < AttributeCount; ++AttributeIndex)
	{
		glEnableVertexAttribArray(AttributeIndex);
		glVertexAttribPointer(
			AttributeLayout.AttributeDescriptions[AttributeIndex].Index,
			AttributeLayout.AttributeDescriptions[AttributeIndex].ValueCount,
			GL_RHI::GetVertexInputAttributeDataTypeGLType(AttributeLayout.AttributeDescriptions[AttributeIndex].ValueType),
			GL_FALSE,
			AttributeLayout.AttributeDescriptions[AttributeIndex].ByteStrideToNextAttribute,
			reinterpret_cast<const void*>(AttributeLayout.AttributeDescriptions[AttributeIndex].ByteOffsetFromVertexStart)
		);
	}

	glDrawElements(GL_TRIANGLES, GLElementBuffer->ElementCount, GL_UNSIGNED_INT, 0);
}
