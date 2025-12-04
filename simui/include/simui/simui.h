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
#define SI_EXIT_SUCCESS		 0
#define SI_EXIT_INIT_FAILURE 1

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
 * The structure representing a color with red, green, blue, and alpha channels.
 */
typedef struct SiColor
{
	u8 r; ///< The red channel of the color.
	u8 g; ///< The green channel of the color.
	u8 b; ///< The blue channel of the color.
	u8 a; ///< The alpha channel of the color.
} SiColor;

/**
 * The parameter structure for the rectangle drawing event.
 * It contains all necessary information required to draw a rectangle on the screen.
 */
typedef struct DrawRectangleParameter
{
	f32			   x;	   ///< The x position of the rectangle.
	f32			   y;	   ///< The y position of the rectangle.
	f32			   width;  ///< The width of the rectangle.
	f32			   height; ///< The height of the rectangle.
	struct SiColor color;  ///< The color of the rectangle.
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
	FPN_SiInitialize initializeFunction; ///< Pointer to the user-defined initialization function.
	FPN_SiPollEvents pollEventsFunction; ///< Pointer to the user-defined poll events function.
	FPN_SiBeginFrame beginFrameFunction; ///< Pointer to the user-defined begin frame function.
	FPN_SiEndFrame	 endFrameFunction;	 ///< Pointer to the user-defined end frame function.
	FPN_SiShutdown	 shutdownFunction;	 ///< Pointer to the user-defined shutdown function.

	/**
	 * Pointer to the user-defined event handling function. If not provided, all events will be ignored.
	 */
	FPN_SiDrawRectangle drawRectangleFunction;
} SiCallbackHub;

typedef struct SiContext
{
	b8 isInitialized; ///< Flag indicating whether the SimUI library has been initialized.
	b8 isRunning;	  ///< Flag indicating whether the application is still running.
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
 *
 * @param deltaTime The time elapsed since the last frame, in seconds.
 */
void siRender(f32 deltaTime);

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

// =========================== Drawing API ===========================
void siDrawRectangle(f32 x, f32 y, f32 width, f32 height);

// =========================== Global Variables ==========================
extern SiCallbackHub gSiCallbackHub;
extern SiContext	 gSiContext;

#if __cplusplus
}
#endif