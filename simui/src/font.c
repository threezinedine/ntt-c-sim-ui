#include "simui/font.h"
#include "simui/platform.h"
#include "simui/simui.h"
#include "stdio.h"
#include <string.h>

#define MAX_FONT_FILE_SIZE 65536

static void reformatFontTableRecords(SiFontTableRecord* pTableRecord);
static b8	validateTable(u8* buffer, SiFontTableRecord* pRecord);

void siFontReadFromFile(const char* filepath, SiFont* pOutFont)
{
	u8	buffer[MAX_FONT_FILE_SIZE] = {0};
	u32 bytesRead				   = readFile(filepath, (char*)buffer, MAX_FONT_FILE_SIZE);

	if (bytesRead == 0 || bytesRead >= MAX_FONT_FILE_SIZE)
	{
		SI_ERROR_EXIT("Failed to read font file: %s", filepath);
	}

	SiFontOffsetSubTable* pOffsetSubTable = (SiFontOffsetSubTable*)buffer;

	if (!gSiContext.isBigEndian)
	{
		pOffsetSubTable->scalarType	   = siU32LittleToBigEndian(pOffsetSubTable->scalarType);
		pOffsetSubTable->numTables	   = siU16LittleToBigEndian(pOffsetSubTable->numTables);
		pOffsetSubTable->searchRange   = siU16LittleToBigEndian(pOffsetSubTable->searchRange);
		pOffsetSubTable->entrySelector = siU16LittleToBigEndian(pOffsetSubTable->entrySelector);
		pOffsetSubTable->rangeShift	   = siU16LittleToBigEndian(pOffsetSubTable->rangeShift);
	}

	pOffsetSubTable->numTables =
		pOffsetSubTable->numTables > MAX_FONT_TABLE_RECORDS ? MAX_FONT_TABLE_RECORDS : pOffsetSubTable->numTables;

	gSiContext.defaultFont.numFontTables = pOffsetSubTable->numTables;

	u32 currentOffset = sizeof(SiFontOffsetSubTable);

	for (u32 tableIndex = 0; tableIndex < pOffsetSubTable->numTables; ++tableIndex)
	{
		memcpy(&pOutFont->fontTableRecords[tableIndex], buffer + currentOffset, sizeof(SiFontTableRecord));
		reformatFontTableRecords(&pOutFont->fontTableRecords[tableIndex]);
		currentOffset += sizeof(SiFontTableRecord);

#if 0
		if (!validateTable(buffer, &pOutFont->fontTableRecords[tableIndex]))
		{
			siPrintWarning("Font table validation failed for table index %u in file %s", tableIndex, filepath);
		}
#endif
		char tag[5] = {0};
		memcpy(tag, &pOutFont->fontTableRecords[tableIndex].tag, sizeof(u32));

		if (strcmp((const char*)tag, "cmap") == 0)
		{
			u32 tableOffset = pOutFont->fontTableRecords[tableIndex].offset;

			memcpy(&pOutFont->cmap, buffer + tableOffset, sizeof(SiCMapHeader));

			if (!gSiContext.isBigEndian)
			{
				pOutFont->cmap.header.version	   = siU16LittleToBigEndian(pOutFont->cmap.header.version);
				pOutFont->cmap.header.numSubTables = siU16LittleToBigEndian(pOutFont->cmap.header.numSubTables);
			}

			pOutFont->cmap.numSubTables = pOutFont->cmap.header.numSubTables > MAX_CMAP_SUBTABLES
											  ? MAX_CMAP_SUBTABLES
											  : pOutFont->cmap.header.numSubTables;

			memcpy(&pOutFont->cmap.subTableRecords,
				   buffer + tableOffset + sizeof(SiCMapHeader),
				   pOutFont->cmap.numSubTables * sizeof(SiCMapSubTableRecord));

			for (u32 subTableIndex = 0; subTableIndex < pOutFont->cmap.numSubTables; ++subTableIndex)
			{
				SiCMapSubTableRecord* pSubtable = &pOutFont->cmap.subTableRecords[subTableIndex];

				if (!gSiContext.isBigEndian)
				{
					pSubtable->platformID		  = siU16LittleToBigEndian(pSubtable->platformID);
					pSubtable->platformSpecificID = siU16LittleToBigEndian(pSubtable->platformSpecificID);
					pSubtable->offset			  = siU32LittleToBigEndian(pSubtable->offset);
				}

				if (pSubtable->platformID == PLATFORM_ID_UNICODE && pSubtable->platformSpecificID == 3)
				{
					u32			   format4Offset = tableOffset + pSubtable->offset;
					SiCMapFormat4* pFormat4		 = &pOutFont->cmap.format4;

					u32 formatSpecificSize = 5 * sizeof(u16); // up to reversedPad

					memcpy(pFormat4,
						   buffer + format4Offset,
						   sizeof(SiCMapFormatHeader) + formatSpecificSize); // up to reversedPad

					if (!gSiContext.isBigEndian)
					{
						pFormat4->header.format	  = siU16LittleToBigEndian(pFormat4->header.format);
						pFormat4->header.length	  = siU16LittleToBigEndian(pFormat4->header.length);
						pFormat4->header.language = siU16LittleToBigEndian(pFormat4->header.language);

						pFormat4->segCountX2	= siU16LittleToBigEndian(pFormat4->segCountX2);
						pFormat4->searchRange	= siU16LittleToBigEndian(pFormat4->searchRange);
						pFormat4->entrySelector = siU16LittleToBigEndian(pFormat4->entrySelector);
						pFormat4->rangeShift	= siU16LittleToBigEndian(pFormat4->rangeShift);
					}

					u32 endCodeOffset	   = format4Offset + sizeof(SiCMapFormatHeader) + formatSpecificSize;
					u32 startCodeOffset	   = endCodeOffset + (pFormat4->segCountX2 / 2) * sizeof(u16);
					u32 idDeltaOffset	   = startCodeOffset + (pFormat4->segCountX2 / 2) * sizeof(u16);
					u32 idRangeOffset	   = idDeltaOffset + (pFormat4->segCountX2 / 2) * sizeof(u16);
					u32 glyphIdArrayOffset = idRangeOffset + (pFormat4->segCountX2 / 2) * sizeof(u16);

					memcpy(pFormat4->endCode, buffer + endCodeOffset, startCodeOffset - endCodeOffset);
					memcpy(pFormat4->startCode, buffer + startCodeOffset, idDeltaOffset - startCodeOffset);
					memcpy(pFormat4->idDelta, buffer + idDeltaOffset, idRangeOffset - idDeltaOffset);
					memcpy(pFormat4->idRangeOffset, buffer + idRangeOffset, glyphIdArrayOffset - idRangeOffset);

					for (u16 i = 0; i < pFormat4->segCountX2 / 2; ++i)
					{
						if (!gSiContext.isBigEndian)
						{
							pFormat4->endCode[i]	   = siU16LittleToBigEndian(pFormat4->endCode[i]);
							pFormat4->startCode[i]	   = siU16LittleToBigEndian(pFormat4->startCode[i]);
							pFormat4->idDelta[i]	   = siU16LittleToBigEndian(pFormat4->idDelta[i]);
							pFormat4->idRangeOffset[i] = siU16LittleToBigEndian(pFormat4->idRangeOffset[i]);
						}
					}

					i32 remainingBytes = pFormat4->header.length - glyphIdArrayOffset + format4Offset;

					for (i32 i = 0; i < remainingBytes / 2; ++i)
					{
						if (!gSiContext.isBigEndian)
						{
							pFormat4->glyphIdArray[i] = siU16LittleToBigEndian(pFormat4->glyphIdArray[i]);
						}
					}
				}
			}
		}
	}
}

i32 siGetGlyphIndex(SiFont* pFont, u16 characterCode)
{
	if (pFont->cmap.numSubTables == 0)
	{
		SI_ERROR_EXIT("No cmap sub-tables available in the font.");
	}

	if (pFont->cmap.format4.header.format != 4)
	{
		SI_ERROR_EXIT("Unsupported cmap format: %u. Only format 4 is supported.", pFont->cmap.format4.header.format);
	}

	SiCMapFormat4* pFormat4 = &pFont->cmap.format4;
	u32			   segCount = pFormat4->segCountX2 / 2;

	i32 index = -1;

	for (u32 i = 0; i < segCount; ++i)
	{
		if (pFormat4->endCode[i] >= characterCode)
		{
			index = i;
			break;
		}
	}

	if (index == -1 || pFormat4->startCode[index] > characterCode)
	{
		return -1; // character not found
	}

	u16 idRangeOffset = pFormat4->idRangeOffset[index];

	if (idRangeOffset == 0)
	{
		return (i16)characterCode + (i16)pFormat4->idDelta[index];
	}
	else
	{
		u16 glyphIndexAddress = (idRangeOffset / 2) + (characterCode - pFormat4->startCode[index]) - (segCount - index);

		i16 glyphIndex = (i16)pFormat4->glyphIdArray[glyphIndexAddress];

		if (glyphIndex == 0)
		{
			return -1; // missing glyph
		}

		glyphIndex += (i16)pFormat4->idDelta[index];
		return glyphIndex;
	}
}

static void reformatFontTableRecords(SiFontTableRecord* pTableRecord)
{
	if (!gSiContext.isBigEndian)
	{
		pTableRecord->checkSum = siU32LittleToBigEndian(pTableRecord->checkSum);
		pTableRecord->offset   = siU32LittleToBigEndian(pTableRecord->offset);
		pTableRecord->length   = siU32LittleToBigEndian(pTableRecord->length);
	}
}

void siPrintFontTableRecords(const SiFontTableRecord* pRecord)
{
	char tag[5] = {0};
	memcpy(tag, &pRecord->tag, sizeof(u32));

	printf("Font Table Record: Tag: %s, CheckSum: 0x%X, Offset: %u, Length: %u\n",
		   tag,
		   pRecord->checkSum,
		   pRecord->offset,
		   pRecord->length);
}

void siPrintCMap(const SiCMap* pCMap)
{
	printf("CMap Header: Version: %u, NumSubTables: %u\n", pCMap->header.version, pCMap->header.numSubTables);
	for (u32 i = 0; i < pCMap->numSubTables; ++i)
	{
		const SiCMapSubTableRecord* pSubTable = &pCMap->subTableRecords[i];

		char platform[16] = {0};

		switch (pSubTable->platformID)
		{
		case 0:
			strcpy(platform, "Unicode");
			break;
		case 1:
			strcpy(platform, "Macintosh");
			break;
		case 3:
			strcpy(platform, "Windows");
			break;
		default:
			strcpy(platform, "Unknown");
		}

		printf("\tSubTable %u: PlatformID: %s, EncodingID: %u, Offset: %u\n",
			   i,
			   platform,
			   pSubTable->platformSpecificID,
			   pSubTable->offset);
	}
}

static b8 validateTable(u8* buffer, SiFontTableRecord* pRecord)
{
	u32 sum	   = 0;
	u32 nLongs = (pRecord->length + 3) / 4;

	for (u32 i = 0; i < nLongs; ++i)
	{
		sum += *(buffer + (pRecord->offset + i) * sizeof(u32));
	}

	return sum == pRecord->checkSum;
}

void siPrintFontFormat4(const SiCMapFormat4* pFormat4)
{
	printf("CMap Format 4:\n");
	printf("\tFormat: %u\n", pFormat4->header.format);
	printf("\tLength: %u\n", pFormat4->header.length);
	printf("\tLanguage: %u\n", pFormat4->header.language);
	printf("\tSegCountX2: %u\n", pFormat4->segCountX2);
	printf("\tSearchRange: %u\n", pFormat4->searchRange);
	printf("\tEntrySelector: %u\n", pFormat4->entrySelector);
	printf("\tRangeShift: %u\n", pFormat4->rangeShift);

#if 1
	u16 segCount = pFormat4->segCountX2 / 2;

	printf("Segments:\tStartCode\tEndCode\tIDDelta\tIDRangeOffset\n");
	for (u16 i = 0; i < segCount; ++i)
	{
		printf("%8u:\t%9u\t%7u\t%7u\t%13u\n",
			   i,
			   pFormat4->startCode[i],
			   pFormat4->endCode[i],
			   pFormat4->idDelta[i],
			   pFormat4->idRangeOffset[i]);
	}

#endif
}