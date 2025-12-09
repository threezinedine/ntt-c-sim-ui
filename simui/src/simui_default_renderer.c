#if SIMUI_USE_DEFAULT_RENDERER
#include "simui/simui.h"
#include <string.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stdio.h>

#define SHADER_SOURCE_BUFFER_SIZE  2048
#define MAX_UNIFORMS_PER_DRAW_CALL 16

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

#define TEXTURE_VALIDATE(texture)                                                                                      \
	do                                                                                                                 \
	{                                                                                                                  \
		if (texture >= MAX_TEXTURES)                                                                                   \
		{                                                                                                              \
			SI_ERROR_EXIT("Invalid texture handle.");                                                                  \
		}                                                                                                              \
                                                                                                                       \
		if (!gTexturesHub[texture].isUsed)                                                                             \
		{                                                                                                              \
			SI_ERROR_EXIT("Texture handle not in use.");                                                               \
		}                                                                                                              \
	} while (0)

/**
 * Uniform types for shaders.
 */
typedef enum UniformType
{
	UNIFORM_TYPE_FLOAT,
	UNIFORM_TYPE_VEC2,
	UNIFORM_TYPE_VEC3,
	UNIFORM_TYPE_VEC4,
	UNIFORM_TYPE_INT,
	UNIFORM_TYPE_IVEC2,
	UNIFORM_TYPE_IVEC3,
	UNIFORM_TYPE_IVEC4,
	UNIFORM_TYPE_SAMPLER2D,
} UniformType;

typedef struct Sampler2D
{
	u32 textureUnit;
	u32 textureId;
} Sampler2D;

/**
 * Union to hold different types of uniform values.
 */
typedef union UniformValue {
	f32		  floatValue;
	f32		  vec2Value[2];
	f32		  vec3Value[3];
	f32		  vec4Value[4];
	i32		  intValue;
	i32		  ivec2Value[2];
	i32		  ivec3Value[3];
	i32		  ivec4Value[4];
	Sampler2D sampler2DValue;
} UniformValue;

/**
 * Structure to represent a uniform in a shader.
 */
typedef struct Uniform
{
	UniformType	 type;
	UniformValue value;
	i32			 location;
} Uniform;

/**
 * Structure to hold draw call information.
 */
typedef struct DrawCall
{
	Uniform uniforms[MAX_UNIFORMS_PER_DRAW_CALL]; ///< Array of uniforms for this draw call.
	u32		uniformCount;						  ///< Number of uniforms.
	u32		shader;								  ///< The shader program to use for this draw call.
	u32		indexOffset;						  ///< The offset in the index buffer.
} DrawCall;

#define MAX_TEXTURES   128
#define MAX_RECTANGLES 1024
#define MAX_VERTICES   (MAX_RECTANGLES * 4)
#define MAX_INDICES	   (MAX_RECTANGLES * 6)

typedef struct SiTextureData
{
	u32				width;
	u32				height;
	SiTextureFormat format;
	u32				textureId;

	b8 isUsed; ///< Flag to indicate if the texture slot is used.
} SiTextureData;

static SiTextureData gTexturesHub[MAX_TEXTURES];

/**
 * Data structure to hold default renderer specific data.
 */
typedef struct DefaultRendererData
{
	GLFWwindow* pWindow; ///< Pointer to the GLFW window.

	u32 vao; ///< Vertex Array Object.
	u32 vbo; ///< Vertex Buffer Object.
	u32 ebo; ///< Element Buffer Object.

	u32 simpleShader;  ///< Shader program.
	u32 textureShader; ///< Texture shader program.

	DrawCall drawCalls[MAX_RECTANGLES]; ///< Array of draw calls.
	u32		 drawCallCount;				///< Number of draw calls.
	u32		 currentDrawCallIndex;		///< Current draw call index.

	u32 bufferOffset; ///< Current buffer offset for dynamic vertex data.
	u32 indexOffset;  ///< Current index offset for dynamic index data.
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
	f32 textureCoordinates[2];
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

static u32 createShaderFromSource(const char* vertexSourceFile, const char* fragmentSourceFile);

static void siInitialize_DefaultRenderer()
{
	memset(&gDefaultRendererData, 0, sizeof(gDefaultRendererData));
	memset(gTexturesHub, 0, sizeof(gTexturesHub));

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gDefaultRendererData.simpleShader = createShaderFromSource(SI_STRINGIFY(SOURCE_PATH) "/shaders/sim.vert",
															   SI_STRINGIFY(SOURCE_PATH) "/shaders/sim.frag");

	gDefaultRendererData.textureShader = createShaderFromSource(SI_STRINGIFY(SOURCE_PATH) "/shaders/texture.vert",
																SI_STRINGIFY(SOURCE_PATH) "/shaders/texture.frag");

	GL_ASSERT(glGenBuffers(1, &gDefaultRendererData.vbo));
	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, gDefaultRendererData.vbo));
	GL_ASSERT(glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVertex) * MAX_VERTICES, NULL, GL_DYNAMIC_DRAW));

	GL_ASSERT(glGenBuffers(1, &gDefaultRendererData.ebo));
	GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gDefaultRendererData.ebo));
	GL_ASSERT(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * MAX_INDICES, NULL, GL_DYNAMIC_DRAW));

	GL_ASSERT(glGenVertexArrays(1, &gDefaultRendererData.vao));
	GL_ASSERT(glBindVertexArray(gDefaultRendererData.vao));
	GL_ASSERT(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)0));
	GL_ASSERT(glEnableVertexAttribArray(0));
	GL_ASSERT(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)(2 * sizeof(f32))));
	GL_ASSERT(glEnableVertexAttribArray(1));

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GL_ASSERT(glBindVertexArray(0));
	gSiContext.pRenderingData = &gDefaultRendererData;
}

static u32 createShaderFromSource(const char* vertexSourceFile, const char* fragmentSourceFile)
{
	char vertexShaderSource[SHADER_SOURCE_BUFFER_SIZE] = {0};
	readFile(vertexSourceFile, vertexShaderSource, SHADER_SOURCE_BUFFER_SIZE);
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
	readFile(fragmentSourceFile, fragShaderSource, SHADER_SOURCE_BUFFER_SIZE);
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

	u32 shaderProgram = glCreateProgram();
	GL_ASSERT(glAttachShader(shaderProgram, vertexShader));
	GL_ASSERT(glAttachShader(shaderProgram, fragmentShader));
	GL_ASSERT(glLinkProgram(shaderProgram));

	GL_ASSERT(glGetProgramiv(shaderProgram, GL_LINK_STATUS, (i32*)&success));
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		SI_ERROR_EXIT("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
	}

	GL_ASSERT(glDeleteShader(vertexShader));
	GL_ASSERT(glDeleteShader(fragmentShader));

	return shaderProgram;
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
		GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gDefaultRendererData.ebo));

		// Set uniforms
		for (u32 uniformIndex = 0u; uniformIndex < pDrawCall->uniformCount; ++uniformIndex)
		{
			Uniform* pUniform = &pDrawCall->uniforms[uniformIndex];
			switch (pUniform->type)
			{
			case UNIFORM_TYPE_FLOAT:
				GL_ASSERT(glUniform1f(pUniform->location, pUniform->value.floatValue));
				break;
			case UNIFORM_TYPE_VEC2:
				GL_ASSERT(glUniform2fv(pUniform->location, 1, pUniform->value.vec2Value));
				break;
			case UNIFORM_TYPE_VEC3:
				GL_ASSERT(glUniform3fv(pUniform->location, 1, pUniform->value.vec3Value));
				break;
			case UNIFORM_TYPE_VEC4:
				GL_ASSERT(glUniform4fv(pUniform->location, 1, pUniform->value.vec4Value));
				break;
			case UNIFORM_TYPE_INT:
				GL_ASSERT(glUniform1i(pUniform->location, pUniform->value.intValue));
				break;
			case UNIFORM_TYPE_IVEC2:
				GL_ASSERT(glUniform2iv(pUniform->location, 1, pUniform->value.ivec2Value));
				break;
			case UNIFORM_TYPE_IVEC3:
				GL_ASSERT(glUniform3iv(pUniform->location, 1, pUniform->value.ivec3Value));
				break;
			case UNIFORM_TYPE_IVEC4:
				GL_ASSERT(glUniform4iv(pUniform->location, 1, pUniform->value.ivec4Value));
				break;
			case UNIFORM_TYPE_SAMPLER2D:
				GL_ASSERT(glActiveTexture(GL_TEXTURE0 + pUniform->value.sampler2DValue.textureUnit));
				GL_ASSERT(glBindTexture(GL_TEXTURE_2D, pUniform->value.sampler2DValue.textureId));
				GL_ASSERT(glUniform1i(pUniform->location, pUniform->value.sampler2DValue.textureUnit));
				break;
			default:
				SI_ERROR_EXIT("Unknown uniform type.");
			}
		}

		GL_ASSERT(
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(uintptr_t)(pDrawCall->indexOffset * sizeof(u32))));
	}

	GL_ASSERT(glfwSwapBuffers(gDefaultRendererData.pWindow));

	// Reset for next frame
	{
		gDefaultRendererData.drawCallCount		  = 0u;
		gDefaultRendererData.currentDrawCallIndex = 0u;
		gDefaultRendererData.bufferOffset		  = 0u;
		gDefaultRendererData.indexOffset		  = 0u;
	}
}

static void siShutdown_DefaultRenderer()
{
	for (u32 textureIndex = 0u; textureIndex < MAX_TEXTURES; ++textureIndex)
	{
		if (gTexturesHub[textureIndex].isUsed)
		{
			siDestroyTexture((SiTexture)textureIndex);
		}
	}

	GL_ASSERT(glDeleteBuffers(1, &gDefaultRendererData.vbo));
	GL_ASSERT(glDeleteBuffers(1, &gDefaultRendererData.ebo));
	GL_ASSERT(glDeleteVertexArrays(1, &gDefaultRendererData.vao));
	GL_ASSERT(glDeleteProgram(gDefaultRendererData.simpleShader));
	GL_ASSERT(glDeleteProgram(gDefaultRendererData.textureShader));

	GL_ASSERT(glfwDestroyWindow(gDefaultRendererData.pWindow));
	glfwTerminate();
}

static void addVec2Uniform(DrawCall* pDrawCall, const char* name, SiVector2 vec);
static void addVec4Uniform(DrawCall* pDrawCall, const char* name, SiVector4 vec);
static void addSamplerUniform(DrawCall* pDrawCall, const char* name, u32 textureUnit, u32 texture);

static void siDrawRectangle_DefaultRenderer(DrawRectangleParameter params)
{
	DrawCall* pDrawCall = &gDefaultRendererData.drawCalls[gDefaultRendererData.drawCallCount++];
	memset(pDrawCall, 0, sizeof(DrawCall));
	pDrawCall->indexOffset = gDefaultRendererData.indexOffset;

	if (params.texture != SI_TEXTURE_NULL)
	{
		pDrawCall->shader = gDefaultRendererData.textureShader;
	}
	else
	{
		pDrawCall->shader = gDefaultRendererData.simpleShader;
	}

	GL_ASSERT(glUseProgram(pDrawCall->shader));

	addVec2Uniform(pDrawCall, "windowSize", siGetWindowSize_DefaultRenderer(&gDefaultRendererData));
	addVec4Uniform(
		pDrawCall,
		"uColor",
		(SiVector4){
			params.color.r / 255.0f, params.color.g / 255.0f, params.color.b / 255.0f, params.color.a / 255.0f});

	if (params.texture != SI_TEXTURE_NULL)
	{
		addSamplerUniform(pDrawCall, "uTexture", 0, params.texture);
	}

	GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, gDefaultRendererData.vbo));

	f32 x	   = params.x;
	f32 y	   = params.y;
	f32 width  = params.width;
	f32 height = params.height;

	// clang-format off
	RenderVertex vertices[] = {
		{{x - width / 2.0f, y - height / 2.0f}, {0.0f, 1.0f}}, // Bottom-left
		{{x + width / 2.0f, y - height / 2.0f}, {1.0f, 1.0f}}, // Bottom-right
		{{x + width / 2.0f, y + height / 2.0f}, {1.0f, 0.0f}}, // Top-right
		{{x - width / 2.0f, y + height / 2.0f}, {0.0f, 0.0f}}, // Top-left
	};
	// clang-format on

	u32 verticiesCount = sizeof(vertices) / sizeof(RenderVertex);

	GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER,
							  gDefaultRendererData.bufferOffset * sizeof(RenderVertex),
							  sizeof(RenderVertex) * verticiesCount,
							  vertices));

	u32 indices[] = {
		gDefaultRendererData.bufferOffset + 0,
		gDefaultRendererData.bufferOffset + 1,
		gDefaultRendererData.bufferOffset + 2,
		gDefaultRendererData.bufferOffset + 2,
		gDefaultRendererData.bufferOffset + 3,
		gDefaultRendererData.bufferOffset + 0,
	};

	u32 indicesCount = sizeof(indices) / sizeof(u32);

	GL_ASSERT(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gDefaultRendererData.ebo));
	GL_ASSERT(glBufferSubData(
		GL_ELEMENT_ARRAY_BUFFER, gDefaultRendererData.indexOffset * sizeof(u32), sizeof(u32) * indicesCount, indices));

	gDefaultRendererData.bufferOffset += verticiesCount;
	gDefaultRendererData.indexOffset += indicesCount;
}

static void addVec2Uniform(DrawCall* pDrawCall, const char* name, SiVector2 vec)
{
	pDrawCall->uniformCount++;
	Uniform* pUniform  = &pDrawCall->uniforms[pDrawCall->uniformCount - 1];
	pUniform->type	   = UNIFORM_TYPE_VEC2;
	pUniform->location = glGetUniformLocation(pDrawCall->shader, name);

	memcpy(pUniform->value.vec2Value, &vec, sizeof(pUniform->value.vec2Value));

	if (pUniform->location == -1)
	{
		siPrintWarning("SIMUI: Failed to get uniform location for '%s'.", name);
		pDrawCall->uniformCount--;
	}
}

static void addVec4Uniform(DrawCall* pDrawCall, const char* name, SiVector4 vec)
{
	pDrawCall->uniformCount++;
	Uniform* pUniform  = &pDrawCall->uniforms[pDrawCall->uniformCount - 1];
	pUniform->type	   = UNIFORM_TYPE_VEC4;
	pUniform->location = glGetUniformLocation(pDrawCall->shader, name);

	memcpy(pUniform->value.vec4Value, &vec, sizeof(pUniform->value.vec4Value));

	if (pUniform->location == -1)
	{
		siPrintWarning("SIMUI: Failed to get uniform location for '%s'.", name);
		pDrawCall->uniformCount--;
	}
}

static void addSamplerUniform(DrawCall* pDrawCall, const char* name, u32 textureUnit, u32 texture)
{
	TEXTURE_VALIDATE(texture);
	SiTextureData* pTexture = &gTexturesHub[texture];
	pDrawCall->uniformCount++;
	Uniform* pUniform  = &pDrawCall->uniforms[pDrawCall->uniformCount - 1];
	pUniform->type	   = UNIFORM_TYPE_SAMPLER2D;
	pUniform->location = glGetUniformLocation(pDrawCall->shader, name);

	if (pUniform->location == -1)
	{
		siPrintWarning("SIMUI: Failed to get uniform location for '%s'.", name);
		pDrawCall->uniformCount--;
	}

	pUniform->value.sampler2DValue.textureUnit = textureUnit; // Texture unit 0
	pUniform->value.sampler2DValue.textureId   = pTexture->textureId;
}

static SiVector2 siGetWindowSize_DefaultRenderer(void* pRenderingData)
{
	DefaultRendererData* pData = (DefaultRendererData*)pRenderingData;
	i32					 width, height;
	GL_ASSERT(glfwGetFramebufferSize(pData->pWindow, &width, &height));
	SiVector2 size = {(f32)width, (f32)height};
	return size;
}

SiTexture siCreateTexture(u32 width, u32 height, SiTextureFormat format, const void* pData)
{
	// Find an unused texture slot
	u32 textureIndex = 0;
	for (textureIndex = 0; textureIndex < MAX_TEXTURES; ++textureIndex)
	{
		if (!gTexturesHub[textureIndex].isUsed)
		{
			break;
		}
	}

	SiTextureData* pTexture = &gTexturesHub[textureIndex];
	memset(pTexture, 0, sizeof(SiTextureData));

	pTexture->width	 = width;
	pTexture->height = height;
	pTexture->format = format;
	pTexture->isUsed = SI_TRUE;
	GL_ASSERT(glGenTextures(1, &pTexture->textureId));
	GL_ASSERT(glBindTexture(GL_TEXTURE_2D, pTexture->textureId));
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	switch (format)
	{
	case SI_TEXTURE_FORMAT_RGBA8:
		GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData));
		break;
	case SI_TEXTURE_FORMAT_RGB8:
		GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pData));
		break;
	default:
		SI_ERROR_EXIT("Unsupported texture format.");
		break;
	}

	return (u32)textureIndex;
}

SiVector2 siGetTextureSize(SiTexture texture)
{
	TEXTURE_VALIDATE(texture);
	SiTextureData* pTexture = &gTexturesHub[texture];
	SiVector2	   size		= {(f32)pTexture->width, (f32)pTexture->height};
	return size;
}

SiTextureFormat siGetTextureFormat(SiTexture texture)
{
	TEXTURE_VALIDATE(texture);

	SiTextureData* pTexture = &gTexturesHub[texture];
	return pTexture->format;
}

void siDestroyTexture(SiTexture texture)
{
	TEXTURE_VALIDATE(texture);

	SiTextureData* pTexture = &gTexturesHub[texture];
	GL_ASSERT(glDeleteTextures(1, &pTexture->textureId));
	pTexture->isUsed = SI_FALSE;
}

#endif // SIMUI_USE_DEFAULT_RENDERER
