#include "simui/simui.h"
#include <stdio.h>

int main(void)
{
	siConfigureCallbacks();
	siInitialize();

	while (siRunning())
	{
		siPollEvents();

		siDrawRectangle(100, 50, 200, 100);

		siDrawRectangle(110, 100, 200, 100);

		siRender(0.016f); // Assuming a fixed delta time of ~16ms for simplicity
	}

	siShutdown();
	return 0;
}