#pragma once

#if __cplusplus
extern "C" {
#endif

#include "common.h"
#include "simui/texture.h"

typedef struct SiFont
{
	const char* file;
	u32			size;
	f32			sizeInPixels;
} SiFont;

void siFontLoad(const char* file, SiFont* pFont, f32 size);

SiSprite siGetFontSprite(SiFont* pFont, i8 character);

void siFontUnload(SiFont* pFont);

#if __cplusplus
}
#endif