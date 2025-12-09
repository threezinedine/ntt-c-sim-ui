#pragma once
#include "common.h"

#if __cplusplus
extern "C" {
#endif

void siStringFormat(char* buffer, u32 bufferSize, const char* format, ...);
void siRaiseError(const char* message, ...);
void siPrintWarning(const char* message, ...);

#define ERROR_MESSAGE_BUFFER_SIZE 1024

/**
 * Macro for printing an error message and exiting the program with the specified exit code.
 */
#define SI_ERROR_EXIT(message, ...)                                                                                    \
	do                                                                                                                 \
	{                                                                                                                  \
		char errorMessage[ERROR_MESSAGE_BUFFER_SIZE] = {0};                                                            \
		siStringFormat(errorMessage, ERROR_MESSAGE_BUFFER_SIZE, message, ##__VA_ARGS__);                               \
		siRaiseError("[SIMUI] - %s", errorMessage);                                                                    \
	} while (0)

#define SI_STRINGIFY(x)	 _SI_STRINGIFY(x)
#define _SI_STRINGIFY(x) #x

/**
 * Read the contents of a file into a buffer.
 *
 * @param filePath The path to the file to be read.
 * @param buffer The buffer to store the file contents.
 * @param bufferSize The size of the buffer.
 * @return The number of bytes read from the file.
 */
u32 readFile(const char* filePath, char* buffer, u32 bufferSize);

/**
 * Used for working with font files, to determine the endianness of the system.
 */
b8 isBigEndian();

u64 siU64LittleToBigEndian(u64 value);
u32 siU32LittleToBigEndian(u32 value);
u16 siU16LittleToBigEndian(u16 value);

#if __cplusplus
}
#endif