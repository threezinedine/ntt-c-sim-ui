#include "simui/simui.h"

SiCallbackHub gSiCallbackHub = {0};
SiContext	  gSiContext	 = {0};

void siInitialize()
{
	gSiContext.isRunning = SI_TRUE;

	if (gSiCallbackHub.initializeFunction)
	{
		gSiCallbackHub.initializeFunction();
	}
}

void siPollEvents()
{
	if (gSiCallbackHub.pollEventsFunction)
	{
		gSiCallbackHub.pollEventsFunction();
	}
}

b8 siRunning()
{
	return gSiContext.isRunning;
}

void siRender(f32 deltaTime)
{
	if (gSiCallbackHub.executeLoopFunction)
	{
		gSiCallbackHub.executeLoopFunction(deltaTime);
	}
}

void siShutdown()
{
	if (gSiCallbackHub.shutdownFunction)
	{
		gSiCallbackHub.shutdownFunction();
	}
}