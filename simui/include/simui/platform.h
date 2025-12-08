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

#if __cplusplus
}
#endif