#ifndef __ATOM3DX_TW_WRAPPER_H
#define __ATOM3DX_TW_WRAPPER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

ATOMX_API bool ATOM_CALL ATOMX_LoadTweakBarLib (void);
ATOMX_API void ATOM_CALL ATOMX_UnloadTweakBarLib (void);

typedef struct CTwBar TwBar; // structure CTwBar is not exposed.

#define TW_CALL __cdecl

typedef TwBar* (TW_CALL* Fn_TwNewBar)(const char* barName);
typedef int (TW_CALL* Fn_TwDeleteBar)(TwBar* bar);
typedef int (TW_CALL* Fn_TwDeleteAllBars)();
typedef int (TW_CALL* Fn_TwSetTopBar)(const TwBar* bar);
typedef TwBar* (TW_CALL* Fn_TwGetTopBar)();
typedef int (TW_CALL* Fn_TwSetBottomBar)(const TwBar* bar);
typedef TwBar* (TW_CALL* Fn_TwGetBottomBar)();
typedef const char* (TW_CALL* Fn_TwGetBarName)(TwBar* bar);
typedef int (TW_CALL* Fn_TwGetBarCount)();
typedef TwBar* (TW_CALL* Fn_TwGetBarByIndex)(int barIndex);
typedef TwBar* (TW_CALL* Fn_TwGetBarByName)(const char* barName);
typedef int (TW_CALL* Fn_TwRefreshBar)(TwBar* bar);

// ----------------------------------------------------------------------------
//  Var functions and definitions
// ----------------------------------------------------------------------------

typedef enum ETwType
{
	TW_TYPE_UNDEF = 0,
#ifdef __cplusplus
	TW_TYPE_BOOLCPP = 1,
#endif // __cplusplus
	TW_TYPE_BOOL8 = 2,
	TW_TYPE_BOOL16,
	TW_TYPE_BOOL32,
	TW_TYPE_CHAR,
	TW_TYPE_INT8,
	TW_TYPE_UINT8,
	TW_TYPE_INT16,
	TW_TYPE_UINT16,
	TW_TYPE_INT32,
	TW_TYPE_UINT32,
	TW_TYPE_FLOAT,
	TW_TYPE_DOUBLE,
	TW_TYPE_COLOR32,    // 32 bits color. Order is RGBA if API is OpenGL or Direct3D10, and inversed if API is Direct3D9 (can be modified by defining 'colorOrder=...', see doc)
	TW_TYPE_COLOR3F,    // 3 floats color. Order is RGB.
	TW_TYPE_COLOR4F,    // 4 floats color. Order is RGBA.
	TW_TYPE_CDSTRING,   // Null-terminated C Dynamic String (pointer to an array of char dynamically allocated with malloc/realloc/strdup)
#ifdef __cplusplus
	TW_TYPE_STDSTRING = (0x2fff0000 + sizeof(std::string)),  // C++ STL string (std::string)
#endif // __cplusplus
	TW_TYPE_QUAT4F = TW_TYPE_CDSTRING + 2, // 4 floats encoding a quaternion {qx,qy,qz,qs}
	TW_TYPE_QUAT4D,     // 4 doubles encoding a quaternion {qx,qy,qz,qs}
	TW_TYPE_DIR3F,      // direction vector represented by 3 floats
	TW_TYPE_DIR3D       // direction vector represented by 3 doubles
} TwType;
#define TW_TYPE_CSSTRING(n) ((TwType)(0x30000000+((n)&0xfffffff))) // Null-terminated C Static String of size n (defined as char[n], with n<2^28)

typedef void (TW_CALL* TwSetVarCallback)(const void* value, void* clientData);
typedef void (TW_CALL* TwGetVarCallback)(void* value, void* clientData);
typedef void (TW_CALL* TwButtonCallback)(void* clientData);

typedef int (TW_CALL* Fn_TwAddVarRW)(TwBar* bar, const char* name, TwType type, void* var, const char* def);
typedef int (TW_CALL* Fn_TwAddVarRO)(TwBar* bar, const char* name, TwType type, const void* var, const char* def);
typedef int (TW_CALL* Fn_TwAddVarCB)(TwBar* bar, const char* name, TwType type, TwSetVarCallback setCallback, TwGetVarCallback getCallback, void* clientData, const char* def);
typedef int (TW_CALL* Fn_TwAddButton)(TwBar* bar, const char* name, TwButtonCallback callback, void* clientData, const char* def);
typedef int (TW_CALL* Fn_TwAddSeparator)(TwBar* bar, const char* name, const char* def);
typedef int (TW_CALL* Fn_TwRemoveVar)(TwBar* bar, const char* name);
typedef int (TW_CALL* Fn_TwRemoveAllVars)(TwBar* bar);

typedef struct CTwEnumVal
{
	int           Value;
	const char* Label;
} TwEnumVal;
typedef struct CTwStructMember
{
	const char* Name;
	TwType        Type;
	size_t        Offset;
	const char* DefString;
} TwStructMember;
typedef void (TW_CALL* TwSummaryCallback)(char* summaryString, size_t summaryMaxLength, const void* value, void* clientData);

typedef int (TW_CALL* Fn_TwDefine)(const char* def);
typedef TwType (TW_CALL* Fn_TwDefineEnum)(const char* name, const TwEnumVal* enumValues, unsigned int nbValues);
typedef TwType (TW_CALL* Fn_TwDefineStruct)(const char* name, const TwStructMember* structMembers, unsigned int nbMembers, size_t structSize, TwSummaryCallback summaryCallback, void* summaryClientData);

typedef void (TW_CALL* TwCopyCDStringToClient)(char** destinationClientStringPtr, const char* sourceString);
typedef void (TW_CALL* Fn_TwCopyCDStringToClientFunc)(TwCopyCDStringToClient copyCDStringFunc);
typedef void (TW_CALL* Fn_TwCopyCDStringToLibrary)(char** destinationLibraryStringPtr, const char* sourceClientString);

typedef void (TW_CALL* TwCopyStdStringToClient)(std::string& destinationClientString, const std::string& sourceString);
typedef void (TW_CALL* Fn_TwCopyStdStringToClientFunc)(TwCopyStdStringToClient copyStdStringToClientFunc);
typedef void (TW_CALL* Fn_TwCopyStdStringToLibrary)(std::string& destinationLibraryString, const std::string& sourceClientString);

typedef void (TW_CALL* TwSaveCursorCallback)(void);
typedef void (TW_CALL* TwSetCursorCallback)(HCURSOR);
typedef void (TW_CALL* TwRestoreCursorCallback)(void);

typedef void (TW_CALL* Fn_TwCursorAccessFunc)(TwSaveCursorCallback saveFunc, TwSetCursorCallback setFunc, TwRestoreCursorCallback restoreFunc);

typedef enum ETwParamValueType
{
	TW_PARAM_INT32,
	TW_PARAM_FLOAT,
	TW_PARAM_DOUBLE,
	TW_PARAM_CSTRING // Null-terminated array of char (ie, c-string)
} TwParamValueType;
typedef int (TW_CALL* Fn_TwGetParam)(TwBar* bar, const char* varName, const char* paramName, TwParamValueType paramValueType, unsigned int outValueMaxCount, void* outValues);
typedef int (TW_CALL* Fn_TwSetParam)(TwBar* bar, const char* varName, const char* paramName, TwParamValueType paramValueType, unsigned int inValueCount, const void* inValues);


// ----------------------------------------------------------------------------
//  Managment functions and definitions
// ----------------------------------------------------------------------------

typedef enum ETwGraphAPI
{
	TW_OPENGL = 1,
	TW_DIRECT3D9 = 2,
	TW_DIRECT3D10 = 3
} TwGraphAPI;

typedef void* (TW_CALL* TwGraphCreateCallback)(TwGraphAPI graphAPI, void* clientData);

typedef int (TW_CALL* Fn_TwInit)(TwGraphAPI graphAPI, void* device);
typedef int (TW_CALL* Fn_TwTerminate)();
typedef void (TW_CALL* Fn_TwGraphCreateFunc)(TwGraphCreateCallback callback, void* clientData);

typedef int (TW_CALL* Fn_TwDraw)();
typedef int (TW_CALL* Fn_TwWindowSize)(int width, int height);

typedef enum ETwKeyModifier
{
	TW_KMOD_NONE = 0x0000,   // same codes as SDL keysym.mod
	TW_KMOD_SHIFT = 0x0003,
	TW_KMOD_CTRL = 0x00c0,
	TW_KMOD_ALT = 0x0100,
	TW_KMOD_META = 0x0c00
} TwKeyModifier;
typedef enum EKeySpecial
{
	TW_KEY_BACKSPACE = '\b',
	TW_KEY_TAB = '\t',
	TW_KEY_CLEAR = 0x0c,
	TW_KEY_RETURN = '\r',
	TW_KEY_PAUSE = 0x13,
	TW_KEY_ESCAPE = 0x1b,
	TW_KEY_SPACE = ' ',
	TW_KEY_DELETE = 0x7f,
	TW_KEY_UP = 273,      // same codes and order as SDL keysym.sym
	TW_KEY_DOWN,
	TW_KEY_RIGHT,
	TW_KEY_LEFT,
	TW_KEY_INSERT,
	TW_KEY_HOME,
	TW_KEY_END,
	TW_KEY_PAGE_UP,
	TW_KEY_PAGE_DOWN,
	TW_KEY_F1,
	TW_KEY_F2,
	TW_KEY_F3,
	TW_KEY_F4,
	TW_KEY_F5,
	TW_KEY_F6,
	TW_KEY_F7,
	TW_KEY_F8,
	TW_KEY_F9,
	TW_KEY_F10,
	TW_KEY_F11,
	TW_KEY_F12,
	TW_KEY_F13,
	TW_KEY_F14,
	TW_KEY_F15,
	TW_KEY_LAST
} TwKeySpecial;

typedef int (TW_CALL* Fn_TwKeyPressed)(int key, int modifiers);
typedef int (TW_CALL* Fn_TwCharEntered)(unsigned short mbcc);

typedef enum ETwMouseAction
{
	TW_MOUSE_RELEASED,
	TW_MOUSE_PRESSED
} TwMouseAction;
typedef enum ETwMouseButtonID
{
	TW_MOUSE_LEFT = 1,    // same code as SDL_BUTTON_LEFT
	TW_MOUSE_MIDDLE = 2,    // same code as SDL_BUTTON_MIDDLE
	TW_MOUSE_RIGHT = 3     // same code as SDL_BUTTON_RIGHT
} TwMouseButtonID;

typedef int (TW_CALL* Fn_TwMouseButton)(TwMouseAction action, TwMouseButtonID button);
typedef int (TW_CALL* Fn_TwMouseMotion)(int mouseX, int mouseY);
typedef int (TW_CALL* Fn_TwMouseWheel)(int pos);

typedef const char* (TW_CALL* Fn_TwGetLastError)();
typedef void (TW_CALL* TwErrorHandler)(const char* errorMessage);
typedef void (TW_CALL* Fn_TwHandleErrors)(TwErrorHandler errorHandler);


// ----------------------------------------------------------------------------
//  Helper functions to translate events from some common window management
//  frameworks to AntTweakBar.
//  They call TwKeyPressed, TwMouse* and TwWindowSize for you (implemented in
//  files TwEventWin.c TwEventSDL.c TwEventGLFW.c TwEventGLUT.c)
// ----------------------------------------------------------------------------

//  For Windows message proc
#ifndef _W64    // Microsoft specific (detection of 64 bits portability problems)
#   define _W64
#endif  // _W64
typedef int (TW_CALL* Fn_TwEventWin)(void* wnd, unsigned int msg, unsigned int _W64 wParam, int _W64 lParam);
#define TwEventWin32    TwEventWin // For compatibility with AntTweakBar versions prior to 1.11

//  For libSDL event loop
typedef int (TW_CALL* Fn_TwEventSDL)(const void* sdlEvent);

//  For GLFW event callbacks
typedef int (TW_CALL* Fn_TwEventMouseButtonGLFW)(int glfwButton, int glfwAction);
typedef int (TW_CALL* Fn_TwEventKeyGLFW)(int glfwKey, int glfwAction);
typedef int (TW_CALL* Fn_TwEventCharGLFW)(int glfwChar, int glfwAction);
#define TwEventMousePosGLFW     TwMouseMotion
#define TwEventMouseWheelGLFW   TwMouseWheel

//  For GLUT event callbacks (Windows calling convention for GLUT callbacks is cdecl)
#if defined(_WIN32) || defined(_WIN64)
#   define TW_GLUT_CALL __cdecl
#else
#   define TW_GLUT_CALL
#endif
typedef int (TW_CALL* Fn_TwEventMouseButtonGLUT)(int glutButton, int glutState, int mouseX, int mouseY);
typedef int (TW_CALL* Fn_TwEventMouseMotionGLUT)(int mouseX, int mouseY);
typedef int (TW_CALL* Fn_TwEventKeyboardGLUT)(unsigned char glutKey, int mouseX, int mouseY);
typedef int (TW_CALL* Fn_TwEventSpecialGLUT)(int glutKey, int mouseX, int mouseY);
typedef int (TW_CALL* Fn_TwGLUTModifiersFunc)(int (TW_CALL* glutGetModifiersFunc)(void));
typedef void (TW_GLUT_CALL* GLUTmousebuttonfun)(int glutButton, int glutState, int mouseX, int mouseY);
typedef void (TW_GLUT_CALL* GLUTmousemotionfun)(int mouseX, int mouseY);
typedef void (TW_GLUT_CALL* GLUTkeyboardfun)(unsigned char glutKey, int mouseX, int mouseY);
typedef void (TW_GLUT_CALL* GLUTspecialfun)(int glutKey, int mouseX, int mouseY);


// ----------------------------------------------------------------------------
//  Make sure the types have the right sizes
// ----------------------------------------------------------------------------

#define TW_COMPILE_TIME_ASSERT(name, x) typedef int TW_DUMMY_ ## name[(x) * 2 - 1]

TW_COMPILE_TIME_ASSERT(CHAR, sizeof(char) == 1);
TW_COMPILE_TIME_ASSERT(SHORT, sizeof(short) == 2);
TW_COMPILE_TIME_ASSERT(INT, sizeof(int) == 4);
TW_COMPILE_TIME_ASSERT(FLOAT, sizeof(float) == 4);
TW_COMPILE_TIME_ASSERT(DOUBLE, sizeof(double) == 8);

extern Fn_TwNewBar TwNewBar;
extern Fn_TwDeleteBar TwDeleteBar;
extern Fn_TwDeleteAllBars TwDeleteAllBars;
extern Fn_TwSetTopBar TwSetTopBar;
extern Fn_TwGetTopBar TwGetTopBar;
extern Fn_TwSetBottomBar TwSetBottomBar;
extern Fn_TwGetBottomBar TwGetBottomBar;
extern Fn_TwGetBarName TwGetBarName;
extern Fn_TwGetBarCount TwGetBarCount;
extern Fn_TwGetBarByIndex TwGetBarByIndex;
extern Fn_TwGetBarByName TwGetBarByName;
extern Fn_TwRefreshBar TwRefreshBar;
extern Fn_TwAddVarRW TwAddVarRW;
extern Fn_TwAddVarRO TwAddVarRO;
extern Fn_TwAddVarCB TwAddVarCB;
extern Fn_TwAddButton TwAddButton;
extern Fn_TwAddSeparator TwAddSeparator;
extern Fn_TwRemoveVar TwRemoveVar;
extern Fn_TwRemoveAllVars TwRemoveAllVars;
extern Fn_TwDefine TwDefine;
extern Fn_TwDefineEnum TwDefineEnum;
extern Fn_TwDefineStruct TwDefineStruct;
extern Fn_TwCopyCDStringToClientFunc TwCopyCDStringToClientFunc;
extern Fn_TwCopyCDStringToLibrary TwCopyCDStringToLibrary;
extern Fn_TwCopyStdStringToClientFunc TwCopyStdStringToClientFunc;
extern Fn_TwCopyStdStringToLibrary TwCopyStdStringToLibrary;
extern Fn_TwCursorAccessFunc TwCursorAccessFunc;
extern Fn_TwGetParam TwGetParam;
extern Fn_TwSetParam TwSetParam;
extern Fn_TwInit TwInit;
extern Fn_TwTerminate TwTerminate;
extern Fn_TwGraphCreateFunc TwGraphCreateFunc;
extern Fn_TwDraw TwDraw;
extern Fn_TwWindowSize TwWindowSize;
extern Fn_TwKeyPressed TwKeyPressed;
extern Fn_TwCharEntered TwCharEntered;
extern Fn_TwMouseButton TwMouseButton;
extern Fn_TwMouseMotion TwMouseMotion;
extern Fn_TwMouseWheel TwMouseWheel;
extern Fn_TwGetLastError TwGetLastError;
extern Fn_TwHandleErrors TwHandleErrors;
extern Fn_TwEventWin TwEventWin;

/* TwColors */
typedef unsigned int color32;


const color32 COLOR32_BLACK = 0xff000000;   // Black 
const color32 COLOR32_WHITE = 0xffffffff;   // White 
const color32 COLOR32_ZERO = 0x00000000;   // Zero 
const color32 COLOR32_RED = 0xffff0000;   // Red 
const color32 COLOR32_GREEN = 0xff00ff00;   // Green 
const color32 COLOR32_BLUE = 0xff0000ff;   // Blue 


template <typename _T> inline const _T& TClamp(const _T& _X, const _T& _Limit1, const _T& _Limit2)
{
	if (_Limit1 < _Limit2)
		return (_X <= _Limit1) ? _Limit1 : ((_X >= _Limit2) ? _Limit2 : _X);
	else
		return (_X <= _Limit2) ? _Limit2 : ((_X >= _Limit1) ? _Limit1 : _X);
}

inline color32 Color32FromARGBi(int _A, int _R, int _G, int _B)
{
	return (((color32)TClamp(_A, 0, 255)) << 24) | (((color32)TClamp(_R, 0, 255)) << 16) | (((color32)TClamp(_G, 0, 255)) << 8) | ((color32)TClamp(_B, 0, 255));
}

inline color32 Color32FromARGBf(float _A, float _R, float _G, float _B)
{
	return (((color32)TClamp(_A * 256.0f, 0.0f, 255.0f)) << 24) | (((color32)TClamp(_R * 256.0f, 0.0f, 255.0f)) << 16) | (((color32)TClamp(_G * 256.0f, 0.0f, 255.0f)) << 8) | ((color32)TClamp(_B * 256.0f, 0.0f, 255.0f));
}

inline void Color32ToARGBi(color32 _Color, int* _A, int* _R, int* _G, int* _B)
{
	if (_A) *_A = (_Color >> 24) & 0xff;
	if (_R) *_R = (_Color >> 16) & 0xff;
	if (_G) *_G = (_Color >> 8) & 0xff;
	if (_B) *_B = _Color & 0xff;
}

inline void Color32ToARGBf(color32 _Color, float* _A, float* _R, float* _G, float* _B)
{
	if (_A) *_A = (1.0f / 255.0f) * float((_Color >> 24) & 0xff);
	if (_R) *_R = (1.0f / 255.0f) * float((_Color >> 16) & 0xff);
	if (_G) *_G = (1.0f / 255.0f) * float((_Color >> 8) & 0xff);
	if (_B) *_B = (1.0f / 255.0f) * float(_Color & 0xff);
}

/* TwFonts */

struct CTexFont
{
	unsigned char* m_TexBytes;
	int             m_TexWidth;     // power of 2
	int             m_TexHeight;    // power of 2
	float           m_CharU0[256];
	float           m_CharV0[256];
	float           m_CharU1[256];
	float           m_CharV1[256];
	int             m_CharWidth[256];
	int             m_CharHeight;
	int             m_NbCharRead;
};

/* TwGraph */

class ITwGraph
{
public:
	virtual int         Init() = 0;
	virtual int         Shut() = 0;
	virtual void        BeginDraw(int _WndWidth, int _WndHeight) = 0;
	virtual void        EndDraw() = 0;
	virtual bool        IsDrawing() = 0;
	virtual void        Restore() = 0;

	virtual void        DrawLine(int _X0, int _Y0, int _X1, int _Y1, color32 _Color0, color32 _Color1, bool _AntiAliased = false) = 0;
	virtual void        DrawLine(int _X0, int _Y0, int _X1, int _Y1, color32 _Color, bool _AntiAliased = false) = 0;
	virtual void        DrawRect(int _X0, int _Y0, int _X1, int _Y1, color32 _Color00, color32 _Color10, color32 _Color01, color32 _Color11) = 0;
	virtual void        DrawRect(int _X0, int _Y0, int _X1, int _Y1, color32 _Color) = 0;
	enum Cull { CULL_NONE, CULL_CW, CULL_CCW };
	virtual void        DrawTriangles(int _NumTriangles, int* _Vertices, color32* _Colors, Cull _CullMode) = 0;

	virtual void* NewTextObj() = 0;
	virtual void        DeleteTextObj(void* _TextObj) = 0;
	virtual void        BuildText(void* _TextObj, const char** _TextLines, color32* _LineColors, color32* _LineBgColors, int _NbLines, const CTexFont* _Font, int _Sep, int _BgWidth) = 0;
	virtual void        DrawText(void* _TextObj, int _X, int _Y, color32 _Color, color32 _BgColor) = 0;

	virtual void        ChangeViewport(int _X0, int _Y0, int _Width, int _Height, int _OffsetX, int _OffsetY) = 0;
	virtual void        RestoreViewport() = 0;
	virtual void		DeleteThis() = 0;

	virtual             ~ITwGraph() {}  // required by gcc
};

#endif // __ATOM3DX_TW_WRAPPER_H
