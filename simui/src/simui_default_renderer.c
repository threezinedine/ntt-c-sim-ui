#if SIMUI_USE_DEFAULT_RENDERER
#include "simui/simui.h"

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stdlib.h>

/**
 * Data structure to hold default renderer specific data.
 */
typedef struct DefaultRendererData
{
	GLFWwindow* pWindow; ///< Pointer to the GLFW window.
} DefaultRendererData;

static DefaultRendererData gDefaultRendererData = {0};

static void siInitialize_DefaultRenderer();
static void siPollEvents_DefaultRenderer();
static void siRender_DefaultRenderer(f32 deltaTime);
static void siShutdown_DefaultRenderer();

void siConfigureCallbacks()
{
	SiCallbackHub* hub = &gSiCallbackHub;

	hub->initializeFunction	 = siInitialize_DefaultRenderer;
	hub->pollEventsFunction	 = siPollEvents_DefaultRenderer;
	hub->executeLoopFunction = siRender_DefaultRenderer;
	hub->shutdownFunction	 = siShutdown_DefaultRenderer;
}

static void siInitialize_DefaultRenderer()
{
	if (!glfwInit())
	{
		exit(SI_EXIT_INIT_FAILURE);
	}

	gDefaultRendererData.pWindow = glfwCreateWindow(800, 600, "SimUI Default Renderer", NULL, NULL);

	if (!gDefaultRendererData.pWindow)
	{
		glfwTerminate();
		exit(SI_EXIT_INIT_FAILURE);
	}

	glfwMakeContextCurrent(gDefaultRendererData.pWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwDestroyWindow(gDefaultRendererData.pWindow);
		glfwTerminate();
		exit(SI_EXIT_INIT_FAILURE);
	}
}

void siPollEvents_DefaultRenderer()
{
	glfwPollEvents();

	if (glfwWindowShouldClose(gDefaultRendererData.pWindow))
	{
		gSiContext.isRunning = SI_FALSE;
	}
}

void siRender_DefaultRenderer(f32 deltaTime)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(gDefaultRendererData.pWindow);
}

void siShutdown_DefaultRenderer()
{
	glfwDestroyWindow(gDefaultRendererData.pWindow);
	glfwTerminate();
}

#endif // SIMUI_USE_DEFAULT_RENDERER
