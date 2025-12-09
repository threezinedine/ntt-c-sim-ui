#include "simui/simui.h"

#ifdef SIMUI_USE_STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif // SIMUI_USE_STB

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

void siInitialize(SiConfig config)
{
	gSiContext.isRunning   = SI_TRUE;
	gSiContext.isBigEndian = isBigEndian();

	siFontReadFromFile(config.fontFile, &gSiContext.defaultFont);

	for (u32 tableIndex = 0u; tableIndex < gSiContext.defaultFont.numFontTables; ++tableIndex)
	{
		SiFontTableRecord* pRecord = &gSiContext.defaultFont.fontTableRecords[tableIndex];
		siPrintFontTableRecords(pRecord);
	}

	siPrintCMap(&gSiContext.defaultFont.cmap);
	siPrintFontFormat4(&gSiContext.defaultFont.cmap.format4);

	printf("\n");

	const char* testChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	for (const char* pChar = testChars; *pChar != '\0'; ++pChar)
	{
		printf("Map: '%c': %d\n", *pChar, siGetGlyphIndex(&gSiContext.defaultFont, (u16)(*pChar)));
	}

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

void siRender()
{
	if (gSiCallbackHub.beginFrameFunction)
	{
		gSiCallbackHub.beginFrameFunction();
	}

	gSiContext.windowSize = gSiCallbackHub.getWindowSizeFunction(gSiContext.pRenderingData);

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

void siDrawRectangle(f32 x, f32 y, f32 width, f32 height, SiColor color, SiTexture texture)
{
	CHECK_DRAWING_EVENT_BUFFER_CAPACITY();

	SiUIEvent* pEvent					= &gDrawingEvents[gDrawingEventsCount++];
	pEvent->type						= SI_UI_EVENT_TYPE_DRAW_RECTANGLE;
	pEvent->drawRectangleParams.x		= x;
	pEvent->drawRectangleParams.y		= y;
	pEvent->drawRectangleParams.width	= width;
	pEvent->drawRectangleParams.height	= height;
	pEvent->drawRectangleParams.color.r = color.r;
	pEvent->drawRectangleParams.color.g = color.g;
	pEvent->drawRectangleParams.color.b = color.b;
	pEvent->drawRectangleParams.color.a = color.a;
	pEvent->drawRectangleParams.texture = texture;
}

#ifdef SIMUI_USE_STB
// =========================== Utils ===========================
SiTexture readImageFile(const char* filePath)
{
	int width, height, channels;
	u8* data = stbi_load(filePath, &width, &height, &channels, 0);
	if (!data)
	{
		SI_ERROR_EXIT("Failed to load image file: %s", filePath);
	}

	SiTextureFormat format;
	switch (channels)
	{
	case 4:
		format = SI_TEXTURE_FORMAT_RGBA8;
		break;
	case 3:
		format = SI_TEXTURE_FORMAT_RGB8;
		break;
	default:
		stbi_image_free(data);
		SI_ERROR_EXIT("Unsupported image format: %s", filePath);
		break;
	}

	SiTexture texture = siCreateTexture((u32)width, (u32)height, format, data);
	stbi_image_free(data);
	return texture;
}
#endif // SIMUI_USE_STB