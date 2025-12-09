#include "simui/simui.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void siStringFormat(char* buffer, u32 bufferSize, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, bufferSize, format, args);
	va_end(args);
}

void siRaiseError(const char* message, ...)
{
	// change console color to red
	printf("\033[1;31m");

	char finalMessage[ERROR_MESSAGE_BUFFER_SIZE] = {0};

	va_list args;
	va_start(args, message);
	vsnprintf(finalMessage, ERROR_MESSAGE_BUFFER_SIZE, message, args);
	printf("%s\n", finalMessage);
	va_end(args);

	// reset console color
	printf("\033[0m");

	exit(SI_EXIT_FAILURE);
}

void siPrintWarning(const char* message, ...)
{
	// change console color to yellow
	printf("\033[1;33m");

	char finalMessage[ERROR_MESSAGE_BUFFER_SIZE] = {0};

	va_list args;
	va_start(args, message);
	vsnprintf(finalMessage, ERROR_MESSAGE_BUFFER_SIZE, message, args);
	printf("Warning: %s\n", finalMessage);
	va_end(args);

	// reset console color
	printf("\033[0m");
}

u32 readFile(const char* filePath, char* buffer, u32 bufferSize)
{
	FILE* file = fopen(filePath, "r");
	if (!file)
	{
		SI_ERROR_EXIT("Failed to open shader file: %s", filePath);
	}

	memset(buffer, 0, bufferSize);

	u32 fileLength = fseek(file, 0, SEEK_END);
	rewind(file);

	if (fileLength >= bufferSize)
	{
		SI_ERROR_EXIT("Shader file too large: %s", filePath);
	}

	size_t bytesRead  = fread(buffer, 1, bufferSize - 1, file);
	buffer[bytesRead] = '\0';

	fclose(file);

	return (u32)bytesRead;
}

b8 isBigEndian()
{
	u16 testValue = 0x1;
	u8* pBytes	  = (u8*)&testValue;
	return pBytes[0] == 0 ? SI_TRUE : SI_FALSE;
}

u64 siU64LittleToBigEndian(u64 value)
{
	return ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) |
		   ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8) |
		   ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
		   ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56);
}

u32 siU32LittleToBigEndian(u32 value)
{
	return ((value & 0x000000FFU) << 24) | ((value & 0x0000FF00U) << 8) | ((value & 0x00FF0000U) >> 8) |
		   ((value & 0xFF000000U) >> 24);
}

u16 siU16LittleToBigEndian(u16 value)
{
	return (u16)(((value & 0x00FFU) << 8) | ((value & 0xFF00U) >> 8));
}