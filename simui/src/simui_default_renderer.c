#if SIMUI_USE_DEFAULT_RENDERER
#include "simui/simui.h"
#include <string.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stdio.h>

#define SHADER_SOURCE_BUFFER_SIZE 2048

#define GL_ASSERT(call)                                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		while (glGetError() != GL_NO_ERROR);                                                                           \
		call;                                                                                                          \
		GLenum err = glGetError();                                                                                     \
		if (err != GL_NO_ERROR)                                                                                        \
		{                                                                                                              \
			SI_ERROR_EXIT("OpenGL error 0x%X at %s:%d", err, __FILE__, __LINE__);                                      \
		}                                                                                                              \
	} while (0);

/**
 * Structure to hold draw call information.
 */
typedef struct DrawCall
{
	SiVector2 windowSize;	///< The size of the window for this draw call.
	u32		  shader;		///< The shader program to use for this draw call.
	u32		  vertexOffset; ///< The offset in the vertex buffer.
} DrawCall;

#define MAX_TRIANGLES 1024
#define MAX_VERTICES  (MAX_TRIANGLES * 3)

/**
 * Data structure to hold default renderer specific data.
 */
typedef struct DefaultRendererData
{
	GLFWwindow* pWindow; ///< Pointer to the GLFW window.

	u32 vao; ///< Vertex Array Object.
	u32 vbo; ///< Vertex Buffer Object.

	u32 shader; ///< Shader program.

	DrawCall drawCalls[MAX_TRIANGLES]; ///< Array of draw calls.
	u32		 drawCallCount;			   ///< Number of draw calls.
	u32		 currentDrawCallIndex;	   ///< Current draw call index.

	u32 bufferOffset; ///< Current buffer offset for dynamic vertex data.
} DefaultRendererData;

static DefaultRendererData gDefaultRendererData = {0};

static void		 siInitialize_DefaultRenderer();
static void		 siPollEvents_DefaultRenderer();
static void		 siBeginFrame_DefaultRenderer();
static void		 siEndFrame_DefaultRenderer();
static void		 siShutdown_DefaultRenderer();
static void		 siDrawRectangle_DefaultRenderer(DrawRectangleParameter params);
static SiVector2 siGetWindowSize_DefaultRenderer(void* pRenderingData);

/**
 * Vertex structure for rendering.
 */
typedef struct RenderVertex
{
	f32 position[2];
} RenderVertex;

void siConfigureCallbacks()
{
	SiCallbackHub* hub = &gSiCallbackHub;

	hub->initializeFunction	   = siInitialize_DefaultRenderer;
	hub->pollEventsFunction	   = siPollEvents_DefaultRenderer;
	hub->beginFrameFunction	   = siBeginFrame_DefaultRenderer;
	hub->endFrameFunction	   = siEndFrame_DefaultRenderer;
	hub->shutdownFunction	   = siShutdown_DefaultRenderer;
	hub->getWindowSizeFunction = siGetWindowSize_DefaultRenderer;

	hub->drawRectangleFunction = siDrawRectangle_DefaultRenderer;
}

static void readShaderSource(const char* filePath, char* buffer, u32 bufferSize)
{
	FILE* file = fopen(filePath, "r");
	if (!file)
	{
		SI_ERROR_EXIT("Failed to open shader file: %s", filePath);
	}

	memset(buffer, 0, bufferSize);

	u32 fileLength = fseek(file, 0, SEEK_END);
	rewind(file);

	if (fileLength >= bufferSize)
	{
		SI_ERROR_EXIT("Shader file too large: %s", filePath);
	}

	size_t bytesRead  = fread(buffer, 1, bufferSize - 1, file);
	buffer[bytesRead] = '\0';

	fclose(file);
}

static void siInitialize_DefaultRenderer()
{
	memset(&gDefaultRendererData, 0, sizeof(gDefaultRendererData));

	if (!glfwInit())
	{
		SI_ERROR_EXIT("Failed to initialize GLFW.");
	}

	gDefaultRendererData.pWindow = glfwCreateWindow(800, 600, "SimUI Default Renderer", NULL, NULL);

	if (!gDefaultRendererData.pWindow)
	{
		glfwTerminate();
		SI_ERROR_EXIT("Failed to create GLFW window.");
	}

	glfwMakeContextCurrent(gDefaultRendererData.pWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(gDefaultRendererData.pWindow);
		glfwTerminate();
		SI_ERROR_EXIT("Failed to initialize GLAD.");
	}

	char vertexShaderSource[SHADER_SOURCE_BUFFER_SIZE] = {0};
	readShaderSource(SI_STRINGIFY(SOURCE_PATH) "/shaders/sim.vert", vertexShaderSource, SHADER_SOURCE_BUFFER_SIZE);
	const char* vertexShaderSourcePtr = vertexShaderSource;

	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GL_ASSERT(glShaderSource(vertexShader, 1, &vertexShaderSourcePtr, NULL));
	GL_ASSERT(glCompileShader(vertexShader));

	GLuint success;
	GL_ASSERT(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, (i32*)&success));
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		SI_ERROR_EXIT("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}

	char fragShaderSource[SHADER_SOURCE_BUFFER_SIZE] = {0};
	readShaderSource(SI_STRINGIFY(SOURCE_PATH) "/shaders/sim.frag", fragShaderSource, SHADER_SOURCE_BUFFER_SIZE);
	const char* fragShaderSourcePtr = fragShaderSource;

	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	GL_ASSERT(glShaderSource(fragmentShader, 1, &fragShaderSourcePtr, NULL));
	GL_ASSERT(glCompileShader(fragmentShader));

	GL_ASSERT(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, (i32*)&success));
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		SI_ERROR_EXIT("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	gDefaultRendererData.shader = glCreateProgram();
	GL_ASSERT(glAttachShader(gDefaultRendererData.shader, vertexShader));
	GL_ASSERT(glAttachShader(gDefaultRendererData.shader, fragmentShader));
	GL_ASSERT(glLinkProgram(gDefaultRendererData.shader));

	GL_ASSERT(glGetProgramiv(gDefaultRendererData.shader, GL_LINK_STATUS, (i32*)&success));
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(gDefaultRendererData.shader, 512, NULL, infoLog);
		SI_ERROR_EXIT("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	GL_ASSERT(glDeleteShader(vertexShader));
	GL_ASSERT(glDeleteShader(fragmentShader));

	GL_ASSERT(glGenBuffers(1, &gDefaultRendererData.vbo));
	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, gDefaultRendererData.vbo));
	GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVertex) * MAX_VERTICES, NULL, GL_DYNAMIC_DRAW));

	GL_ASSERT(glGenVertexArrays(1, &gDefaultRendererData.vao));
	GL_ASSERT(glBindVertexArray(gDefaultRendererData.vao));
	GL_ASSERT(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)0));
	GL_ASSERT(glEnableVertexAttribArray(0));

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_ASSERT(glBindVertexArray(0));
	gSiContext.pRenderingData = &gDefaultRendererData;
}

static void siPollEvents_DefaultRenderer()
{
	glfwPollEvents();

	if (glfwWindowShouldClose(gDefaultRendererData.pWindow))
	{
		gSiContext.isRunning = SI_FALSE;
	}
}

static void siBeginFrame_DefaultRenderer()
{
	GL_ASSERT(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GL_ASSERT(glClear(GL_COLOR_BUFFER_BIT));
}

static void siEndFrame_DefaultRenderer()
{
	for (u32 drawCallIndex = 0u; drawCallIndex < gDefaultRendererData.drawCallCount; ++drawCallIndex)
	{
		DrawCall* pDrawCall = &gDefaultRendererData.drawCalls[drawCallIndex];
		GL_ASSERT(glUseProgram(pDrawCall->shader));
		GL_ASSERT(glBindVertexArray(gDefaultRendererData.vao));
		i32 windowSizeLocation = glGetUniformLocation(pDrawCall->shader, "uWindowSize");
		if (windowSizeLocation == -1)
		{
			SI_ERROR_EXIT("Failed to get uniform location for uWindowSize.");
		}

		GL_ASSERT(glUniform2f(windowSizeLocation, (f32)pDrawCall->windowSize.x, (f32)pDrawCall->windowSize.y));
		GL_ASSERT(glDrawArrays(GL_TRIANGLES, pDrawCall->vertexOffset, 6));
	}

	GL_ASSERT(glfwSwapBuffers(gDefaultRendererData.pWindow));

	// Reset for next frame
	{
		gDefaultRendererData.drawCallCount		  = 0u;
		gDefaultRendererData.bufferOffset		  = 0u;
		gDefaultRendererData.currentDrawCallIndex = 0u;
	}
}

static void siShutdown_DefaultRenderer()
{
	GL_ASSERT(glfwDestroyWindow(gDefaultRendererData.pWindow));
	glfwTerminate();
}

static void siDrawRectangle_DefaultRenderer(DrawRectangleParameter params)
{
	DrawCall* pDrawCall		= &gDefaultRendererData.drawCalls[gDefaultRendererData.drawCallCount++];
	pDrawCall->windowSize	= gSiContext.windowSize;
	pDrawCall->shader		= gDefaultRendererData.shader;
	pDrawCall->vertexOffset = gDefaultRendererData.bufferOffset;

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, gDefaultRendererData.vbo));

	RenderVertex vertices[6];
	f32			 x		= params.x;
	f32			 y		= params.y;
	f32			 width	= params.width;
	f32			 height = params.height;

	// clang-format off
	vertices[0] = (RenderVertex){{x - width / 2.0f, y - height / 2.0f}}; // Bottom-left
	vertices[1] = (RenderVertex){{x + width / 2.0f, y - height / 2.0f}}; // Bottom-right
	vertices[2] = (RenderVertex){{x + width / 2.0f, y + height / 2.0f}}; // Top-right
	vertices[3] = (RenderVertex){{x + width / 2.0f, y + height / 2.0f}}; // Top-right
	vertices[4] = (RenderVertex){{x - width / 2.0f, y + height / 2.0f}}; // Top-left
	vertices[5] = (RenderVertex){{x - width / 2.0f, y - height / 2.0f}}; // Bottom-left
	// clang-format on

	GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER,
							  gDefaultRendererData.bufferOffset * sizeof(RenderVertex),
							  sizeof(RenderVertex) * 6,
							  vertices););

	gDefaultRendererData.bufferOffset += 6; // 6 vertices for 2 triangles
}

static SiVector2 siGetWindowSize_DefaultRenderer(void* pRenderingData)
{
	DefaultRendererData* pData = (DefaultRendererData*)pRenderingData;
	i32					 width, height;
	GL_ASSERT(glfwGetFramebufferSize(pData->pWindow, &width, &height));
	SiVector2 size = {(f32)width, (f32)height};
	return size;
}

#endif // SIMUI_USE_DEFAULT_RENDERER
