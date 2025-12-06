#pragma once

#if __cplusplus
extern "C" {
#endif

typedef long long i64;
typedef int		  i32;
typedef short	  i16;
typedef char	  i8;

typedef unsigned long long u64;
typedef unsigned int	   u32;
typedef unsigned short	   u16;
typedef unsigned char	   u8;

typedef float  f32;
typedef double f64;

#define SI_NULL 0

typedef u8 b8;

#define SI_TRUE	 ((b8)1)
#define SI_FALSE ((b8)0)

// Exit codes
#define SI_EXIT_SUCCESS 0
#define SI_EXIT_FAILURE 1

// =========================== Macros ===========================
void siStringFormat(char* buffer, u32 bufferSize, const char* format, ...);
void siRaiseError(const char* message, ...);
void siPrintWarning(const char* message, ...);

#define ERROR_MESSAGE_BUFFER_SIZE 1024

/**
 * Macro for printing an error message and exiting the program with the specified exit code.
 */
#define SI_ERROR_EXIT(message, ...)                                                                                    \
	do                                                                                                                 \
	{                                                                                                                  \
		char errorMessage[ERROR_MESSAGE_BUFFER_SIZE] = {0};                                                            \
		siStringFormat(errorMessage, ERROR_MESSAGE_BUFFER_SIZE, message, ##__VA_ARGS__);                               \
		siRaiseError("[SIMUI] - %s", errorMessage);                                                                    \
	} while (0)

#define SI_STRINGIFY(x)	 _SI_STRINGIFY(x)
#define _SI_STRINGIFY(x) #x

// =========================== Common data types ===========================
/**
 * The structure representing a color with RGBA formats.
 */
typedef struct SiColor
{
	u8 r; ///< The red channel of the color.
	u8 g; ///< The green channel of the color.
	u8 b; ///< The blue channel of the color.
	u8 a; ///< The alpha channel of the color.
} SiColor;

#define SI_COLOR_WHITE                                                                                                 \
	(SiColor)                                                                                                          \
	{                                                                                                                  \
		255, 255, 255, 255                                                                                             \
	}
#define SI_COLOR_BLACK                                                                                                 \
	(SiColor)                                                                                                          \
	{                                                                                                                  \
		0, 0, 0, 255                                                                                                   \
	}

#define SI_COLOR_RED                                                                                                   \
	(SiColor)                                                                                                          \
	{                                                                                                                  \
		255, 0, 0, 255                                                                                                 \
	}

#define SI_COLOR_GREEN                                                                                                 \
	(SiColor)                                                                                                          \
	{                                                                                                                  \
		0, 255, 0, 255                                                                                                 \
	}

#define SI_COLOR_BLUE                                                                                                  \
	(SiColor)                                                                                                          \
	{                                                                                                                  \
		0, 0, 255, 255                                                                                                 \
	}

/**
 * For representing the position, size or texture coordinates in 2D space.
 */
typedef struct SiVector2
{
	f32 x; ///< The x component of the vector.
	f32 y; ///< The y component of the vector.
} SiVector2;

typedef struct SiVector4
{
	f32 x; ///< The x component of the vector.
	f32 y; ///< The y component of the vector.
	f32 z; ///< The z component of the vector.
	f32 w; ///< The w component of the vector.
} SiVector4;

// =========================== C Function pointers ===========================

/**
 * Function pointer type for the simulation initialization function. If user wants to
 * override the default initialization, they can provide a function matching this signature. Be called
 * at with the siInitialize function.
 */
typedef void (*FPN_SiInitialize)(void);

/**
 * Function pointer type for the simulation poll events function. If user wants to
 * override the default event polling, they can provide a function matching this signature. Be called
 * at with the siPollEvents function.
 */
typedef void (*FPN_SiPollEvents)(void);

/**
 * Function pointer type for the simulation execute loop function. If user wants to
 * override the default rendering loop, they can provide a function matching this signature. Be called
 * at with the siRender function.
 */
typedef void (*FPN_SiBeginFrame)();

/**
 * Function pointer type for the simulation execute loop function. If user wants to
 * override the default rendering loop, they can provide a function matching this signature. Be called
 * at with the siRender function.
 */
typedef void (*FPN_SiEndFrame)();

/**
 * Function pointer type for the simulation shutdown function. If user wants to
 * override the default shutdown, they can provide a function matching this signature. Be called
 * at with the siShutdown function.
 */
typedef void (*FPN_SiShutdown)(void);

/**
 * Function pointer type for getting the window size. If user wants to
 * override the default window size retrieval, they can provide a function matching this signature.
 * Be called when the window size is needed.
 *
 * @param void* A pointer to the rendering backend specific data.
 */
typedef SiVector2 (*FPN_GetWindowSize)(void*);

#define SI_TEXTURE_NULL ((u32) - 1)
typedef u32 SiTexture;

typedef enum SiTextureFormat
{
	SI_TEXTURE_FORMAT_RGBA8, ///< 8 bits per channel RGBA format.
	SI_TEXTURE_FORMAT_RGB8,	 ///< 8 bits per channel RGB format.
} SiTextureFormat;

/**
 * Rendering specific function for creating a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should allocate
 * and initialize a texture object based on the provided width, height, format, and data.
 *
 * @param width  The width of the texture to be created.
 * @param height The height of the texture to be created.
 * @param format The format of the texture to be created.
 * @param pData  A pointer to the initial data for the texture. Can not be NULL.
 *
 * @return The created texture object.
 */
SiTexture siCreateTexture(u32 width, u32 height, SiTextureFormat format, const void* pData);

/**
 * Rendering specific function for getting the size of a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should return
 * the width and height of the provided texture object.
 *
 * @param texture The texture whose size is to be retrieved.
 *
 * @return A SiVector2 structure containing the width and height of the texture.
 */
SiVector2 siGetTextureSize(SiTexture texture);

/**
 * Rendering specific function for getting the format of a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should return
 * the format of the provided texture object.
 *
 * @param texture The texture whose format is to be retrieved.
 *
 * @return The format of the texture.
 */
SiTextureFormat siGetTextureFormat(SiTexture texture);

/**
 * Rendering specific function for destroying a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should free
 * all resources associated with the provided texture object.
 *
 * @param texture The texture to be destroyed.
 */
void siDestroyTexture(SiTexture texture);

// =========================== Event System ===========================

/**
 * All of drawing events from the libraries which the rendering backend should be aware of.
 * If user wants to override the default event handling, they can provide their own implementation
 * matching this enum, if not, the events will be ignored.
 */
typedef enum SiUIEventType
{
	SI_UI_EVENT_TYPE_DRAW_RECTANGLE, ///< Draw rectangle event with the receive `DrawRectangleParameter`.
} SiUIEventType;

/**
 * The parameter structure for the rectangle drawing event.
 * It contains all necessary information required to draw a rectangle on the screen.
 */
typedef struct DrawRectangleParameter
{
	f32		  x;	   ///< The x position of the rectangle.
	f32		  y;	   ///< The y position of the rectangle.
	f32		  width;   ///< The width of the rectangle.
	f32		  height;  ///< The height of the rectangle.
	SiColor	  color;   ///< The color of the rectangle.
	SiTexture texture; ///< The texture to be used for the rectangle.
} DrawRectangleParameter;

/**
 * The structure representing a UI event in the SimUI library.
 * It contains the type of the event and a union of parameters specific to each event type.
 * The siRender function will dispatch events to the rendering backend based on this structure.
 */
typedef struct SiUIEvent
{
	SiUIEventType type; ///< The type of the event.

	union {
		DrawRectangleParameter drawRectangleParams; ///< Parameters for the draw rectangle event.
	};
} SiUIEvent;

/**
 * The template method for handling the rectangle drawing event. If user wants to override the default rectangle drawing
 * behavior, they can provide their own implementation matching this signature, if the override method is not provided,
 * the event will be ignored.
 */
typedef void (*FPN_SiDrawRectangle)(DrawRectangleParameter params);

// =========================== Context ===========================
/**
 * Store all the customable callback functions provided by the user or the default implementations. With
 * `SIM_UI_USE_DEFAULT_RENDERER` defined, all the function pointers inside this struct will point to the default
 * implementations, otherwise user must provide their own implementations.
 */
typedef struct SiCallbackHub
{
	FPN_SiInitialize  initializeFunction;	 ///< Pointer to the user-defined initialization function.
	FPN_SiPollEvents  pollEventsFunction;	 ///< Pointer to the user-defined poll events function.
	FPN_SiBeginFrame  beginFrameFunction;	 ///< Pointer to the user-defined begin frame function.
	FPN_SiEndFrame	  endFrameFunction;		 ///< Pointer to the user-defined end frame function.
	FPN_SiShutdown	  shutdownFunction;		 ///< Pointer to the user-defined shutdown function.
	FPN_GetWindowSize getWindowSizeFunction; ///< Pointer to the user-defined get window size function.

	/**
	 * Pointer to the user-defined event handling function. If not provided, all events will be ignored.
	 */
	FPN_SiDrawRectangle drawRectangleFunction;
} SiCallbackHub;

typedef struct SiContext
{
	b8		  isRunning;  ///< Flag indicating whether the application is still running.
	SiVector2 windowSize; ///< The current size of the window.

	/**
	 * @brief Pointer to user-defined data associated with the context. Used for storing rendering
	 * backend specific data.
	 */
	void* pRenderingData;
} SiContext;

// =========================== Main API Functions ===========================
/**
 * Configure the callback hub with user-defined or default implementations. This function should be called
 * before `siInitialize` to ensure that the SimUI library uses the correct callbacks.
 */
void siConfigureCallbacks();

/**
 * Be called at the top of the `main` function to starting the SimUI library. If the flag `SIMUI_USE_DEFAULT_RENDERER`
 * is defined, the default rendering backend will be used, otherwise user must provide their own rendering backend.
 */
void siInitialize();

/**
 * Be called in the main loop to check whether the application is still running. If the flag
 * `SIMUI_USE_DEFAULT_RENDERER` is defined, the default rendering backend will be used, otherwise user must handle their
 * own rendering backend.
 *
 * @return A boolean value indicating whether the application is still running (`SI_TRUE`) or should exit (`SI_FALSE`).
 */
b8 siRunning();

/**
 * Be called in the main loop to check whether the application is still running. If the flag
 * `SIMUI_USE_DEFAULT_RENDERER` is defined, the default rendering backend will be used, otherwise user must handle their
 * own rendering backend.
 */
void siPollEvents();

/**
 * Be called every frame to render the SimUI library. This function should be called within the main loop of the
 * application. The `deltaTime` parameter represents the time elapsed since the last frame, in seconds.
 * If the flag `SIMUI_USE_DEFAULT_RENDERER` is defined, the default rendering backend will be used, otherwise user
 * must handle their own rendering backend (`FPN_SiExecuteLoop`).
 */
void siRender();

/**
 * Be called at the bottom of the `main` function to shutdown the SimUI library and free all allocated resources.
 * After this function is called, no SimUI functions should be used unless `siInitialize` is called again. If the flag
 * `SIMUI_USE_DEFAULT_RENDERER` is defined, the default rendering backend will be shutdown, otherwise user must handle
 * the shutdown of their own rendering backend.
 */
void siShutdown();

/**
 * @example usage
 *
 * ```c `main.c`
 * #define SIMUI_IMPLEMENTATION
 * #include "simui/simui.h"
 *
 * int main()
 * {
 * 	   siConfigureCallbacks();
 *     siInitialize();
 *
 * 	   while (siRunning())
 * 	   {
 *          siPollEvents();
 * 	   		siRender(deltaTime);
 * 	   }
 *
 *     siShutdown();
 *     return 0;
 * }
 * ```
 */

// =========================== Drawing API (but used internally) ===========================
void siDrawRectangle(f32 x, f32 y, f32 width, f32 height, SiColor color, SiTexture texture);

#ifdef SIMUI_USE_STB
// =========================== Utils ===========================
SiTexture readImageFile(const char* filePath);
#endif // SIMUI_USE_STB

// =========================== Global Variables ==========================
extern SiCallbackHub gSiCallbackHub;
extern SiContext	 gSiContext;

#if __cplusplus
}
#endif