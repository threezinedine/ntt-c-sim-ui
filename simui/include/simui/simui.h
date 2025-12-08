#pragma once

#if __cplusplus
extern "C" {
#endif

#include "common.h"
#include "datatypes.h"
#include "event.h"
#include "functions.h"
#include "platform.h"
#include "texture.h"

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

#include "apis.h"

// =========================== Global Variables ==========================
extern SiCallbackHub gSiCallbackHub;
extern SiContext	 gSiContext;

#if __cplusplus
}
#endif