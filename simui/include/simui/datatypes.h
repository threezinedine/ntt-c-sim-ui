#pragma once
#include "common.h"

#if __cplusplus
extern "C" {
#endif

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

#if __cplusplus
}
#endif