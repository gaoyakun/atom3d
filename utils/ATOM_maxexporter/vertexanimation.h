#ifndef __VERTEX_ANIMATION_H
#define __VERTEX_ANIMATION_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <string>
#include <map>
#include <windows.h>
#include "propedit.h"

struct VertexAnimationAction
{
	bool useCustomKeyFrames;
	int frameInterval;
	std::vector<int> keyFrames;
	std::string actionName;
	std::vector<int> frameList;
	PropertyEditor properties;
};

struct AnimationRangeInfo
{
	double msStart;
	double msEnd;
	double msInc;
	int frameStart;
	int frameEnd;
};

struct VertexAnimationInfo
{
	std::map<std::string, VertexAnimationAction> actionSet;
	AnimationRangeInfo rangeInfo;
};

bool editVertexAnimation (HMODULE module, HWND parentWnd, VertexAnimationInfo *info);
bool validateAction (HWND dlgWnd, const VertexAnimationAction &action);
void getAnimationRange (AnimationRangeInfo *info);

#endif // __VERTEX_ANIMATION_H
