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
		};

		struct GL_Pipeline
		{
			struct InputVertexAttributeDesc
			{
				GLuint Index = 0;
				GLint Size = 0;
				GLenum Type = GL_NONE;
				GLboolean Normalized = GL_FALSE;
				GLsizei Stride = 0;
				const void* Offset = 0;
			};

			uint32_t ShaderProgram = 0;
			std::vector<GL_Pipeline::InputVertexAttributeDesc> InputVertexAttributeLayout = {};
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

		GLenum GetInputVertexAttributeDataTypeGLType(const Rendering::RenderHardwareInterface::InputVertexAttributeValueDataType DataType)
		{
			switch (DataType)
			{
			case Rendering::RenderHardwareInterface::InputVertexAttributeValueDataType::Float: return GL_FLOAT;

			case Rendering::RenderHardwareInterface::InputVertexAttributeValueDataType::MAX:
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

bool Rendering::RenderHardwareInterface::DeleteVertexBuffer(
	Rendering::RenderHardwareInterface::Context* const Context,
	Rendering::RenderHardwareInterface::Buffer* const Buffer
)
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
	const uint32_t* const IndexData, 
	const size_t IndexCount
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

	return reinterpret_cast<Rendering::RenderHardwareInterface::Buffer*>(GLElementBuffer);
}

bool Rendering::RenderHardwareInterface::DeleteIndexBuffer(
	Rendering::RenderHardwareInterface::Context* const Context, 
	Rendering::RenderHardwareInterface::Buffer* const Buffer
)
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
	const Rendering::RenderHardwareInterface::InputVertexAttributeLayout& InputVertexAttributeLayout,
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

	// Link shader program
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

	// Translate input vertex attribute layout to gl input vertex attribute layout. Used when setting pipeline to use for drawing
	const size_t AttributeCount = InputVertexAttributeLayout.AttributeDescriptions.size();
	GLPipeline->InputVertexAttributeLayout.reserve(AttributeCount);

	for (uint32_t AttributeDescIndex = 0; AttributeDescIndex < AttributeCount; ++AttributeDescIndex)
	{
		GLPipeline->InputVertexAttributeLayout.emplace_back(GL_RHI::GL_Pipeline::InputVertexAttributeDesc
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

void Rendering::RenderHardwareInterface::DrawIndexed(
	Rendering::RenderHardwareInterface::Buffer* const VertexBuffer,
	Rendering::RenderHardwareInterface::Buffer* const IndexBuffer,
	const size_t IndexCount
)
{
	GL_RHI::GL_VertexBuffer* const GLVertexBuffer = reinterpret_cast<GL_RHI::GL_VertexBuffer* const>(VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, GLVertexBuffer->VBO);

	GL_RHI::GL_ElementBuffer* const GLElementBuffer = reinterpret_cast<GL_RHI::GL_ElementBuffer* const>(IndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLElementBuffer->EBO);

	glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);
}
