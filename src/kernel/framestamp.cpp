#include "StdAfx.h"
#include "framestamp.h"

ATOM_FrameStamp::ATOM_FrameStamp (void)
{
	reset ();
}

void ATOM_FrameStamp::reset (float scale)
{
	frameStamp = 0;
	currentTick = 0;
	elapsedTick = 0;
	FPS = 0;

	renderTick = 0;
	oldTick = 0;
	lastFPSTimeMark = 0;
	lastFPSCounter = 0;

	timeScale = scale;
}

void ATOM_FrameStamp::update (void)
{
	currentTick = ATOM_GetTick();
	if ( !renderTick)
	{
		renderTick = currentTick;
		oldTick = currentTick;
	}

	currentTick -= renderTick;
	currentTick *= timeScale;
	elapsedTick = currentTick - oldTick;
	oldTick = currentTick;

	++frameStamp;

	++lastFPSCounter;

	if (currentTick >= lastFPSTimeMark + 1000)
	{
		FPS = (lastFPSCounter * 1000 / (currentTick - lastFPSTimeMark)) * timeScale;
		lastFPSCounter = 0;
		lastFPSTimeMark = currentTick;
	}
}

