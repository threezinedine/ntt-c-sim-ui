#include "simui/simui.h"
#include <stdio.h>

int main(void)
{
	siConfigureCallbacks();
	siInitialize();

	SiTexture texture = readImageFile(SI_STRINGIFY(EXAMPLE_SOURCE_PATH) "/meed-logo.png");
	SiVector2 texSize = siGetTextureSize(texture);

	while (siRunning())
	{
		siPollEvents();

		siDrawRectangle(600, 600, 300, 300, SI_COLOR_WHITE, texture);

		siDrawRectangle(100, 50, 200, 100, SI_COLOR_WHITE, SI_TEXTURE_NULL);

		siDrawRectangle(110, 100, 200, 100, SI_COLOR_RED, SI_TEXTURE_NULL);

		siRender();
	}

	siShutdown();
	return 0;
}