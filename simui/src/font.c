#include "simui/font.h"
#include "simui/platform.h"
#include "simui/simui.h"
#include "simui/texture.h"
#include "stdio.h"
#include <string.h>

#define FONT_BUFFER_SIZE	65536
#define FONT_TEXTURE_WIDTH	2048
#define FONT_TEXTURE_HEIGHT 2048

#define START_OFST 32
#define END_OFST   127

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

static stbtt_fontinfo g_fontInfo;
static SiTexture	  g_fontTexture = SI_TEXTURE_NULL;

static stbtt_packedchar glyphs[END_OFST - START_OFST]					 = {0};
static u8				pixels[FONT_TEXTURE_WIDTH * FONT_TEXTURE_HEIGHT] = {0}; // big texture
static f32				scale											 = 1.0f;
static u8				buffer[FONT_BUFFER_SIZE]						 = {0};

void siFontLoad(const char* file, SiFont* pFont, f32 size)
{
	pFont->file = file;
	pFont->size = readFile(file, buffer, sizeof(buffer));

	if (pFont->size == 0)
	{
		SI_ERROR_EXIT("Failed to read font file: %s", file);
	}

	if (!stbtt_InitFont(&g_fontInfo, buffer, stbtt_GetFontOffsetForIndex(buffer, 0)))
	{
		SI_ERROR_EXIT("Failed to initialize font: %s", file);
	}

	scale				= stbtt_ScaleForPixelHeight(&g_fontInfo, size);
	pFont->sizeInPixels = size;

	stbtt_pack_context pc;
	stbtt_PackBegin(&pc, pixels, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT, 0, 1, NULL);
	stbtt_PackSetOversampling(&pc, 2, 2);
	stbtt_PackFontRange(&pc, buffer, 0, pFont->sizeInPixels, START_OFST, END_OFST - START_OFST, glyphs);
	stbtt_PackEnd(&pc);

	g_fontTexture = siCreateTexture(FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT, SI_TEXTURE_FORMAT_R8, pixels);
}

SiSprite siGetFontSprite(SiFont* pFont, i8 character)
{
	stbtt_aligned_quad quad;

	f32 xpos = 0.0f;
	f32 ypos = 0.0f;

	stbtt_GetPackedQuad(
		glyphs, FONT_TEXTURE_WIDTH, FONT_TEXTURE_HEIGHT, character - START_OFST, &xpos, &ypos, &quad, 0);

	SiSprite sprite = {};
	sprite.texture	= g_fontTexture;
	sprite.quadMin	= (SiVector2){quad.s0, quad.t0};
	sprite.quadMax	= (SiVector2){quad.s1, quad.t1};

	return sprite;
}

void siFontUnload(SiFont* pFont)
{
	if (g_fontTexture != SI_TEXTURE_NULL)
	{
		siDestroyTexture(g_fontTexture);
		g_fontTexture = SI_TEXTURE_NULL;
	}
}