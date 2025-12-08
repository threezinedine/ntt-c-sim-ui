#pragma once
#include "common.h"
#include "datatypes.h"
#include "texture.h"

#if __cplusplus
extern "C" {
#endif

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

#if __cplusplus
}
#endif