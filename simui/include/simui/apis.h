#pragma once
#include "common.h"
#include "datatypes.h"
#include "event.h"

#if __cplusplus
extern "C" {
#endif

// =========================== Main APIs ===========================

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

// =========================== Drawing API (but used internally) ===========================
void siDrawRectangle(f32 x, f32 y, f32 width, f32 height, SiColor color, SiTexture texture);

#ifdef SIMUI_USE_STB
// =========================== Utils ===========================
SiTexture readImageFile(const char* filePath);
#endif // SIMUI_USE_STB

#if __cplusplus
}
#endif