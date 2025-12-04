#include "simui/simui.h"
#include <stdio.h>

int main(void)
{
	siConfigureCallbacks();
	siInitialize();

	while (siRunning())
	{
		siPollEvents();
		siRender(0.016f); // Assuming a fixed delta time of ~16ms for simplicity
	}

	siShutdown();
	return 0;
}