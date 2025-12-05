#if SIMUI_USE_DEFAULT_RENDERER
#include "simui/simui.h"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include "default_fragment_shader.h.in"

#include "default_vertex_shader.h.in"

/**
 * Data structure to hold default renderer specific data.
 */
typedef struct DefaultRendererData
{
	GLFWwindow* pWindow; ///< Pointer to the GLFW window.

	u32 vao; ///< Vertex Array Object.
	u32 vbo; ///< Vertex Buffer Object.

	u32 shader; ///< Shader program.
} DefaultRendererData;

static DefaultRendererData gDefaultRendererData = {0};

static void siInitialize_DefaultRenderer();
static void siPollEvents_DefaultRenderer();
static void siBeginFrame_DefaultRenderer();
static void siEndFrame_DefaultRenderer();
static void siShutdown_DefaultRenderer();
static void siDrawRectangle_DefaultRenderer(DrawRectangleParameter params);

/**
 * Vertex structure for rendering.
 */
typedef struct RenderVertex
{
	f32 position[2];
} RenderVertex;

RenderVertex vertices[] = {
	{{-0.5f, -0.5f}}, // Bottom-left
	{{0.5f, 0.5f}},	  // Top-right
	{{-0.5f, 0.5f}},  // Top-left

	{{-0.5f, -0.5f}}, // Bottom-left
	{{0.5f, -0.5f}},  // Bottom-right
	{{0.5f, 0.5f}},	  // Top-right
};

void siConfigureCallbacks()
{
	SiCallbackHub* hub = &gSiCallbackHub;

	hub->initializeFunction = siInitialize_DefaultRenderer;
	hub->pollEventsFunction = siPollEvents_DefaultRenderer;
	hub->beginFrameFunction = siBeginFrame_DefaultRenderer;
	hub->endFrameFunction	= siEndFrame_DefaultRenderer;
	hub->shutdownFunction	= siShutdown_DefaultRenderer;

	hub->drawRectangleFunction = siDrawRectangle_DefaultRenderer;
}

static void siInitialize_DefaultRenderer()
{
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

	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &defaultVertexShader, NULL);
	glCompileShader(vertexShader);

	b8 success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, (i32*)&success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		SI_ERROR_EXIT("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
	}

	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &defaultFragmentShader, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, (i32*)&success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		SI_ERROR_EXIT("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
	}

	gDefaultRendererData.shader = glCreateProgram();
	glAttachShader(gDefaultRendererData.shader, vertexShader);
	glAttachShader(gDefaultRendererData.shader, fragmentShader);
	glLinkProgram(gDefaultRendererData.shader);

	glGetProgramiv(gDefaultRendererData.shader, GL_LINK_STATUS, (i32*)&success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(gDefaultRendererData.shader, 512, NULL, infoLog);
		SI_ERROR_EXIT("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenBuffers(1, &gDefaultRendererData.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, gDefaultRendererData.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &gDefaultRendererData.vao);
	glBindVertexArray(gDefaultRendererData.vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
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
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void siEndFrame_DefaultRenderer()
{
	glfwSwapBuffers(gDefaultRendererData.pWindow);
}

static void siShutdown_DefaultRenderer()
{
	glfwDestroyWindow(gDefaultRendererData.pWindow);
	glfwTerminate();
}

static void siDrawRectangle_DefaultRenderer(DrawRectangleParameter params)
{
	glUseProgram(gDefaultRendererData.shader);
	glBindVertexArray(gDefaultRendererData.vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

#endif // SIMUI_USE_DEFAULT_RENDERER
