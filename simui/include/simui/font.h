#pragma once

#if __cplusplus
extern "C" {
#endif

#include "common.h"
#define MAX_FONT_TABLE_RECORDS 64
#define MAX_CMAP_SUBTABLES	   16
#define MAX_SEGMENT_COUNT	   256
#define MAX_GLYPHS_PER_FONT	   1024

typedef struct SiCMapHeader
{
	u16 version;	  ///< dont know
	u16 numSubTables; ///< Number of sub-tables in the cmap table
} SiCMapHeader;

#define PLATFORM_ID_UNICODE	  0
#define PLATFORM_ID_MACINTOSH 1
#define PLATFORM_ID_RESERVED  2
#define PLATFORM_ID_WINDOWS	  3

typedef struct SiCMapSubTableRecord
{
	u16 platformID;			///< 0 = Unicode, 1 = Macintosh, 3 = Windows
	u16 platformSpecificID; ///< If platformID == 0 -> 0, 1, 2: ...
	u32 offset;				///< Offset from beginning of cmap table in bytes to the sub-table
} SiCMapSubTableRecord;

typedef struct SiCMapFormatHeader
{
	u16 format;
	u16 length;
	u16 language;
} SiCMapFormatHeader;

typedef struct SiCMapFormat4
{
	SiCMapFormatHeader header;
	u16				   segCountX2;
	u16				   searchRange;
	u16				   entrySelector;
	u16				   rangeShift;
	u16				   reversedPad;
	u16				   endCode[MAX_SEGMENT_COUNT];
	u16				   startCode[MAX_SEGMENT_COUNT];
	u16				   idDelta[MAX_SEGMENT_COUNT];
	u16				   idRangeOffset[MAX_SEGMENT_COUNT];
	u16				   glyphIdArray[MAX_GLYPHS_PER_FONT]; ///< be extended to the end of the table
} SiCMapFormat4;

typedef struct SiCMap
{
	SiCMapHeader		 header;
	SiCMapSubTableRecord subTableRecords[MAX_CMAP_SUBTABLES];
	u32					 numSubTables;

	SiCMapFormat4 format4; ///< Currently only support format 4
} SiCMap;

typedef struct SiFontOffsetSubTable
{
	u32 scalarType;	   ///< 0x00010000 for version 1.0, dont care
	u16 numTables;	   ///< Number of tables
	u16 searchRange;   ///< dont care
	u16 entrySelector; ///< dont care
	u16 rangeShift;	   ///< dont care
} SiFontOffsetSubTable;

typedef union SiFontTag {
	char tagChars[4];
	u32	 tagValue;
} SiFontTag;

typedef struct SiFontTableRecord
{
	SiFontTag tag;		///< 4-character identifier
	u32		  checkSum; ///< Used for validation
	u32		  offset;	///< Offset from beginning of file in bytes
	u32		  length;	///< Length of the table in bytes
} SiFontTableRecord;

/**
 * The needed information for a font in SimUI.
 */
typedef struct SiFont
{
	SiFontTableRecord fontTableRecords[MAX_FONT_TABLE_RECORDS];
	u32				  numFontTables;

	SiCMap cmap;
} SiFont;

void siFontReadFromFile(const char* filepath, SiFont* pOutFont);

i32 siGetGlyphIndex(SiFont* pFont, u16 characterCode);

// ========================== Debug ===========================
void siPrintFontTableRecords(const SiFontTableRecord* pRecord);
void siPrintCMap(const SiCMap* pCMap);
void siPrintFontFormat4(const SiCMapFormat4* pFormat4);

#if __cplusplus
}
#endif