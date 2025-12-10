#pragma once
#include "common.h"
#include "datatypes.h"

#if __cplusplus
extern "C" {
#endif

#define SI_TEXTURE_NULL ((u32) - 1)
typedef u32 SiTexture;

typedef struct SiSprite
{
	SiTexture texture; ///< The texture associated with the sprite.
	SiVector2 quadMin; ///< The minimum texture coordinates of the sprite.
	SiVector2 quadMax; ///< The maximum texture coordinates of the sprite.
} SiSprite;

typedef enum SiTextureFormat
{
	SI_TEXTURE_FORMAT_RGBA8, ///< 8 bits per channel RGBA format.
	SI_TEXTURE_FORMAT_RGB8,	 ///< 8 bits per channel RGB format.
	SI_TEXTURE_FORMAT_R8,	 ///< 8 bits single channel format.
} SiTextureFormat;

/**
 * Rendering specific function for creating a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should allocate
 * and initialize a texture object based on the provided width, height, format, and data.
 *
 * @param width  The width of the texture to be created.
 * @param height The height of the texture to be created.
 * @param format The format of the texture to be created.
 * @param pData  A pointer to the initial data for the texture. Can not be NULL.
 *
 * @return The created texture object.
 */
SiTexture siCreateTexture(u32 width, u32 height, SiTextureFormat format, const void* pData);

/**
 * Rendering specific function for getting the size of a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should return
 * the width and height of the provided texture object.
 *
 * @param texture The texture whose size is to be retrieved.
 *
 * @return A SiVector2 structure containing the width and height of the texture.
 */
SiVector2 siGetTextureSize(SiTexture texture);

/**
 * Rendering specific function for getting the size of a sprite. User must provide their own implementation
 * matching this signature if they want to use sprites in their UI. This function should return
 * the width and height of the provided sprite object.
 *
 * @param texture The sprite whose size is to be retrieved.
 *
 * @return A SiVector2 structure containing the width and height of the sprite.
 */
SiVector2 siGetSpriteSize(SiSprite sprite);

/**
 * Rendering specific function for getting the format of a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should return
 * the format of the provided texture object.
 *
 * @param texture The texture whose format is to be retrieved.
 *
 * @return The format of the texture.
 */
SiTextureFormat siGetTextureFormat(SiTexture texture);

/**
 * Rendering specific function for destroying a texture. User must provide their own implementation
 * matching this signature if they want to use textures in their UI. This function should free
 * all resources associated with the provided texture object.
 *
 * @param texture The texture to be destroyed.
 */
void siDestroyTexture(SiTexture texture);

#if __cplusplus
}
#endif