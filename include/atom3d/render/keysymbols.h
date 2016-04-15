/**	\file animatedtexturechannel.h
 *	键盘键码和鼠标相关常量的声明.
 *
 *	\author 高雅昆
 *	\addtogroup render
 *	@{
 */

#ifndef __ATOM_GLRENDER_KEYSYMBOLS_H
#define __ATOM_GLRENDER_KEYSYMBOLS_H

#if _MSC_VER > 1000
# pragma once
#endif

enum ATOM_Key 
{
  KEY_UNKNOWN = 0,
  KEY_BACKSPACE,
  KEY_TAB,
  KEY_CLEAR,
  KEY_RETURN,
  KEY_PAUSE,
  KEY_ESCAPE,
  KEY_SPACE,
  KEY_EXCLAIM,
  KEY_QUOTEDBL,
  KEY_HASH,
  KEY_DOLLAR,
  KEY_AMPERSAND,
  KEY_QUOTE,
  KEY_LEFTPAREN,
  KEY_RIGHTPAREN,
  KEY_ASTERISK,
  KEY_PLUS,
  KEY_COMMA,
  KEY_MINUS,
  KEY_PERIOD,
  KEY_SLASH,
  KEY_0,
  KEY_1,
  KEY_2,
  KEY_3,
  KEY_4,
  KEY_5,
  KEY_6,
  KEY_7,
  KEY_8,
  KEY_9,
  KEY_COLON,
  KEY_SEMICOLON,
  KEY_LESS,
  KEY_EQUALS,
  KEY_GREATER,
  KEY_QUESTION,
  KEY_AT,
  /*
        Skip uppercase letters
  */
  KEY_LEFTBRACKET,
  KEY_RIGHTBRACKET,
  KEY_CARET,
  KEY_UNDERSCORE,
  KEY_BACKQUOTE,
  KEY_a,
  KEY_b,
  KEY_c,
  KEY_d,
  KEY_e,
  KEY_f,
  KEY_g,
  KEY_h,
  KEY_i,
  KEY_j,
  KEY_k,
  KEY_l,
  KEY_m,
  KEY_n,
  KEY_o,
  KEY_p,
  KEY_q,
  KEY_r,
  KEY_s,
  KEY_t,
  KEY_u,
  KEY_v,
  KEY_w,
  KEY_x,
  KEY_y,
  KEY_z,
  KEY_DELETE,
  /* End of ascii mapped keysyms */

  /* Numeric keypad */
  KEY_KP0,
  KEY_KP1,
  KEY_KP2,
  KEY_KP3,
  KEY_KP4,
  KEY_KP5,
  KEY_KP6,
  KEY_KP7,
  KEY_KP8,
  KEY_KP9,
  KEY_KP_PERIOD,
  KEY_KP_DIVIDE,
  KEY_KP_MULTIPLY,
  KEY_KP_MINUS,
  KEY_KP_PLUS,
  KEY_KP_ENTER,
  KEY_KP_EQUALS,
  /* Arrows + Home/End Pad */
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_INSERT,
  KEY_HOME,
  KEY_END,
  KEY_PAGEUP,
  KEY_PAGEDOWN,
  /* Function keys */
  KEY_F1,
  KEY_F2,
  KEY_F3,
  KEY_F4,
  KEY_F5,
  KEY_F6,
  KEY_F7,
  KEY_F8,
  KEY_F9,
  KEY_F10,
  KEY_F11,
  KEY_F12,
  KEY_F13,
  KEY_F14,
  KEY_F15,
  /* Key state modifier keys */
  KEY_NUMLOCK,
  KEY_CAPSLOCK,
  KEY_SCROLLOCK,
  KEY_RSHIFT,
  KEY_LSHIFT,
  KEY_RCTRL,
  KEY_LCTRL,
  KEY_RALT,
  KEY_LALT,
  KEY_RMETA,
  KEY_LMETA,
  KEY_LSUPER,
  KEY_RSUPER,
  KEY_MODE,
  KEY_COMPOSE,
  /* Miscellaneous function keys */
  KEY_HELP,
  KEY_PRINT,
  KEY_SYSREQ,
  KEY_BREAK,
  KEY_MENU,
  KEY_POWER,
  KEY_EURO,
  KEY_UNDO,

  KEY_NULL,
  KEY_COUNT,
};

enum {
  KEYMOD_NONE = 0,
  KEYMOD_LSHIFT = (1 << 0),
  KEYMOD_RSHIFT = (1 << 1),
  KEYMOD_LCTRL = (1 << 2),
  KEYMOD_RCTRL = (1 << 3),
  KEYMOD_LALT = (1 << 4),
  KEYMOD_RALT = (1 << 5),
  KEYMOD_LMETA = (1 << 6),
  KEYMOD_RMETA = (1 << 7),
  KEYMOD_NUM = (1 << 8),
  KEYMOD_CAPS = (1 << 9),
  KEYMOD_MODE = (1 << 10),
};

#define KEYMOD_CTRL (KEYMOD_LCTRL | KEYMOD_RCTRL)
#define KEYMOD_SHIFT (KEYMOD_LSHIFT | KEYMOD_RSHIFT)
#define KEYMOD_ALT (KEYMOD_LALT | KEYMOD_RALT)
#define KEYMOD_META (KEYMOD_LMETA | KEYMOD_RMETA)

enum ATOM_MouseButton 
{
  BUTTON_UNKNOWN = -1,
  BUTTON_LEFT = 0,
  BUTTON_MIDDLE,
  BUTTON_RIGHT,
  BUTTON_COUNT
};

enum ATOM_KeyState 
{
  KEYSTATE_RELEASED = 0,
  KEYSTATE_PRESSED = 1,
};

#endif // __ATOM_GLRENDER_KEYSYMBOLS_H
/*! @} */
