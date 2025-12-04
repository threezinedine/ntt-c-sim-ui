#include "simui/simui.h"
#include <stdio.h>

int main(void)
{
	siConfigureCallbacks();
	siInitialize();

	while (siRunning())
	{
		siPollEvents();

		siDrawRectangle(-0.5f, -0.5f, 1.0f, 1.0f);

		siRender(0.016f); // Assuming a fixed delta time of ~16ms for simplicity
	}

	siShutdown();
	return 0;
}