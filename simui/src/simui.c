#include "simui/simui.h"

#define DRAWING_EVENT_BUFFER_SIZE 1024
#define CHECK_DRAWING_EVENT_BUFFER_CAPACITY()                                                                          \
	if (gDrawingEventsCount >= DRAWING_EVENT_BUFFER_SIZE)                                                              \
	{                                                                                                                  \
		return;                                                                                                        \
	}

SiCallbackHub gSiCallbackHub = {0};
SiContext	  gSiContext	 = {0};

static SiUIEvent gDrawingEvents[DRAWING_EVENT_BUFFER_SIZE];
static u32		 gDrawingEventsCount	   = 0u;
static u32		 gCurrentDrawingEventIndex = 0u;

void siInitialize()
{
	gSiContext.isRunning = SI_TRUE;

	if (gSiCallbackHub.initializeFunction)
	{
		gSiCallbackHub.initializeFunction();
	}

	if (gSiCallbackHub.getWindowSizeFunction == SI_NULL)
	{
		SI_ERROR_EXIT("Window size function is not set.");
	}
}

void siPollEvents()
{
	if (gSiCallbackHub.pollEventsFunction)
	{
		gSiCallbackHub.pollEventsFunction();
	}
}

b8 siRunning()
{
	return gSiContext.isRunning;
}

void siRender(f32 deltaTime)
{
	if (gSiCallbackHub.beginFrameFunction)
	{
		gSiCallbackHub.beginFrameFunction();
	}

	for (u32 eventIndex = 0u; eventIndex < gDrawingEventsCount; ++eventIndex)
	{
		SiUIEvent* pEvent = &gDrawingEvents[eventIndex];

		switch (pEvent->type)
		{
		case SI_UI_EVENT_TYPE_DRAW_RECTANGLE:
			if (gSiCallbackHub.drawRectangleFunction)
			{
				gSiCallbackHub.drawRectangleFunction(pEvent->drawRectangleParams);
			}
			break;
		default:
			break;
		};
	}

	if (gSiCallbackHub.endFrameFunction)
	{
		gSiCallbackHub.endFrameFunction();
	}

	gDrawingEventsCount		  = 0u;
	gCurrentDrawingEventIndex = 0u;
}

void siShutdown()
{
	if (gSiCallbackHub.shutdownFunction)
	{
		gSiCallbackHub.shutdownFunction();
	}
}

void siDrawRectangle(f32 x, f32 y, f32 width, f32 height)
{
	CHECK_DRAWING_EVENT_BUFFER_CAPACITY();

	SiUIEvent* pEvent					= &gDrawingEvents[gDrawingEventsCount++];
	pEvent->type						= SI_UI_EVENT_TYPE_DRAW_RECTANGLE;
	pEvent->drawRectangleParams.x		= x;
	pEvent->drawRectangleParams.y		= y;
	pEvent->drawRectangleParams.width	= width;
	pEvent->drawRectangleParams.height	= height;
	pEvent->drawRectangleParams.color.r = 255;
	pEvent->drawRectangleParams.color.g = 255;
	pEvent->drawRectangleParams.color.b = 255;
	pEvent->drawRectangleParams.color.a = 255;
}