#pragma once
#include "common.h"
#include "datatypes.h"

#if __cplusplus
extern "C" {
#endif

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

#if __cplusplus
}
#endif