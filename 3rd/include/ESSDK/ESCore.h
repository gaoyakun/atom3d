// Copyright (c) 2007 Ernest Szoka
#ifndef PRECOMPILED_H
#define PRECOMPILED_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#pragma warning (disable: 4244 )

// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef ESAPI_EXPORTS
#define ES_API __declspec(dllexport)
#else
#define ES_API __declspec(dllimport)
#endif

#ifdef ES_PLUGIN_EXPORTS
#define ES_PLUGIN __declspec(dllexport)
#else
#define ES_PLUGIN __declspec(dllimport)
#endif

#define _CRT_SECURE_NO_DEPRECATE 




#include <windows.h>
#include <gl\gl.h>								// Header File For The OpenGL32 Library
#include <gl\glu.h>								// Header File For The GLu32 Library
//#include <gl\glaux.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Image.h"
#include "Color.h"
#include "ESPlugin.h"
#include "ESAPI.h"


#endif
