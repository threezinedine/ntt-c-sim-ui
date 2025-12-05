#include "simui/simui.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

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