#include "stdafx.h"
#include "keymapping.h"

static KeyMapping _S_key_mapping;

KeyMapping::KeyMapping() {
  for ( int i = 0; i < MAXNUMKEYS; ++i)
    _M_key_map[i] = KEY_UNKNOWN;

  for ( int n = 0; n < KEY_COUNT; ++n)
    _M_inv_key_map[n] = NULLKEY;

#if defined(__SDL_WINDOW__)
# define MAP_KEY_TO_SDLK(key) \
_M_key_map[SDLK_##key] = KEY_##key; \
_M_inv_key_map[KEY_##key] = SDLK_##key;

  MAP_KEY_TO_SDLK(BACKSPACE);
  MAP_KEY_TO_SDLK(TAB);
  MAP_KEY_TO_SDLK(CLEAR);
  MAP_KEY_TO_SDLK(RETURN);
  MAP_KEY_TO_SDLK(PAUSE);
  MAP_KEY_TO_SDLK(ESCAPE);
  MAP_KEY_TO_SDLK(SPACE);
  MAP_KEY_TO_SDLK(EXCLAIM);
  MAP_KEY_TO_SDLK(QUOTEDBL);
  MAP_KEY_TO_SDLK(HASH);
  MAP_KEY_TO_SDLK(DOLLAR);
  MAP_KEY_TO_SDLK(AMPERSAND);
  MAP_KEY_TO_SDLK(QUOTE);
  MAP_KEY_TO_SDLK(LEFTPAREN);
  MAP_KEY_TO_SDLK(RIGHTPAREN);
  MAP_KEY_TO_SDLK(ASTERISK);
  MAP_KEY_TO_SDLK(PLUS);
  MAP_KEY_TO_SDLK(COMMA);
  MAP_KEY_TO_SDLK(MINUS);
  MAP_KEY_TO_SDLK(PERIOD);
  MAP_KEY_TO_SDLK(SLASH);
  MAP_KEY_TO_SDLK(0);
  MAP_KEY_TO_SDLK(1);
  MAP_KEY_TO_SDLK(2);
  MAP_KEY_TO_SDLK(3);
  MAP_KEY_TO_SDLK(4);
  MAP_KEY_TO_SDLK(5);
  MAP_KEY_TO_SDLK(6);
  MAP_KEY_TO_SDLK(7);
  MAP_KEY_TO_SDLK(8);
  MAP_KEY_TO_SDLK(9);
  MAP_KEY_TO_SDLK(COLON);
  MAP_KEY_TO_SDLK(SEMICOLON);
  MAP_KEY_TO_SDLK(LESS);
  MAP_KEY_TO_SDLK(EQUALS);
  MAP_KEY_TO_SDLK(GREATER);
  MAP_KEY_TO_SDLK(QUESTION);
  MAP_KEY_TO_SDLK(AT);
  MAP_KEY_TO_SDLK(LEFTBRACKET);
  MAP_KEY_TO_SDLK(RIGHTBRACKET);
  MAP_KEY_TO_SDLK(CARET);
  MAP_KEY_TO_SDLK(UNDERSCORE);
  MAP_KEY_TO_SDLK(BACKQUOTE);
  MAP_KEY_TO_SDLK(a);
  MAP_KEY_TO_SDLK(b);
  MAP_KEY_TO_SDLK(c);
  MAP_KEY_TO_SDLK(d);
  MAP_KEY_TO_SDLK(e);
  MAP_KEY_TO_SDLK(f);
  MAP_KEY_TO_SDLK(g);
  MAP_KEY_TO_SDLK(h);
  MAP_KEY_TO_SDLK(i);
  MAP_KEY_TO_SDLK(j);
  MAP_KEY_TO_SDLK(k);
  MAP_KEY_TO_SDLK(l);
  MAP_KEY_TO_SDLK(m);
  MAP_KEY_TO_SDLK(n);
  MAP_KEY_TO_SDLK(o);
  MAP_KEY_TO_SDLK(p);
  MAP_KEY_TO_SDLK(q);
  MAP_KEY_TO_SDLK(r);
  MAP_KEY_TO_SDLK(s);
  MAP_KEY_TO_SDLK(t);
  MAP_KEY_TO_SDLK(u);
  MAP_KEY_TO_SDLK(v);
  MAP_KEY_TO_SDLK(w);
  MAP_KEY_TO_SDLK(x);
  MAP_KEY_TO_SDLK(y);
  MAP_KEY_TO_SDLK(z);
  MAP_KEY_TO_SDLK(DELETE);
  MAP_KEY_TO_SDLK(KP0);
  MAP_KEY_TO_SDLK(KP1);
  MAP_KEY_TO_SDLK(KP2);
  MAP_KEY_TO_SDLK(KP3);
  MAP_KEY_TO_SDLK(KP4);
  MAP_KEY_TO_SDLK(KP5);
  MAP_KEY_TO_SDLK(KP6);
  MAP_KEY_TO_SDLK(KP7);
  MAP_KEY_TO_SDLK(KP8);
  MAP_KEY_TO_SDLK(KP9);
  MAP_KEY_TO_SDLK(KP_PERIOD);
  MAP_KEY_TO_SDLK(KP_DIVIDE);
  MAP_KEY_TO_SDLK(KP_MULTIPLY);
  MAP_KEY_TO_SDLK(KP_MINUS);
  MAP_KEY_TO_SDLK(KP_PLUS);
  MAP_KEY_TO_SDLK(KP_ENTER);
  MAP_KEY_TO_SDLK(KP_EQUALS);
  MAP_KEY_TO_SDLK(UP);
  MAP_KEY_TO_SDLK(DOWN);
  MAP_KEY_TO_SDLK(LEFT);
  MAP_KEY_TO_SDLK(RIGHT);
  MAP_KEY_TO_SDLK(INSERT);
  MAP_KEY_TO_SDLK(HOME);
  MAP_KEY_TO_SDLK(END);
  MAP_KEY_TO_SDLK(PAGEUP);
  MAP_KEY_TO_SDLK(PAGEDOWN);
  MAP_KEY_TO_SDLK(F1);
  MAP_KEY_TO_SDLK(F2);
  MAP_KEY_TO_SDLK(F3);
  MAP_KEY_TO_SDLK(F4);
  MAP_KEY_TO_SDLK(F5);
  MAP_KEY_TO_SDLK(F6);
  MAP_KEY_TO_SDLK(F7);
  MAP_KEY_TO_SDLK(F8);
  MAP_KEY_TO_SDLK(F9);
  MAP_KEY_TO_SDLK(F10);
  MAP_KEY_TO_SDLK(F11);
  MAP_KEY_TO_SDLK(F12);
  MAP_KEY_TO_SDLK(F13);
  MAP_KEY_TO_SDLK(F14);
  MAP_KEY_TO_SDLK(F15);
  MAP_KEY_TO_SDLK(NUMLOCK);
  MAP_KEY_TO_SDLK(CAPSLOCK);
  MAP_KEY_TO_SDLK(SCROLLOCK);
  MAP_KEY_TO_SDLK(RSHIFT);
  MAP_KEY_TO_SDLK(LSHIFT);
  MAP_KEY_TO_SDLK(RCTRL);
  MAP_KEY_TO_SDLK(LCTRL);
  MAP_KEY_TO_SDLK(RALT);
  MAP_KEY_TO_SDLK(LALT);
  MAP_KEY_TO_SDLK(RMETA);
  MAP_KEY_TO_SDLK(LMETA);
  MAP_KEY_TO_SDLK(LSUPER);
  MAP_KEY_TO_SDLK(RSUPER);
  MAP_KEY_TO_SDLK(MODE);
  MAP_KEY_TO_SDLK(COMPOSE);
  MAP_KEY_TO_SDLK(HELP);
  MAP_KEY_TO_SDLK(PRINT);
  MAP_KEY_TO_SDLK(SYSREQ);
  MAP_KEY_TO_SDLK(BREAK);
  MAP_KEY_TO_SDLK(MENU);
  MAP_KEY_TO_SDLK(POWER);
  MAP_KEY_TO_SDLK(EURO);
  MAP_KEY_TO_SDLK(UNDO);
# else
  _M_key_map[VK_LCONTROL] = KEY_LCTRL;
  _M_inv_key_map[KEY_LCTRL] = VK_LCONTROL;
  _M_key_map[VK_RCONTROL] = KEY_RCTRL;
  _M_inv_key_map[KEY_RCTRL] = VK_RCONTROL;
  _M_key_map[VK_LSHIFT] = KEY_LSHIFT;
  _M_inv_key_map[KEY_LSHIFT] = VK_LSHIFT;
  _M_key_map[VK_RSHIFT] = KEY_RSHIFT;
  _M_inv_key_map[KEY_RSHIFT] = VK_RSHIFT;
  _M_key_map[VK_LMENU] = KEY_LALT;
  _M_inv_key_map[KEY_LALT] = VK_LMENU;
  _M_key_map[VK_RMENU] = KEY_RALT;
  _M_inv_key_map[KEY_RALT] = VK_RMENU;
  _M_key_map[VK_BACK] = KEY_BACKSPACE;
  _M_inv_key_map[KEY_BACKSPACE] = VK_BACK;
  _M_key_map[VK_TAB] = KEY_TAB;
  _M_inv_key_map[KEY_TAB] = VK_TAB;
  _M_key_map[VK_CLEAR] = KEY_CLEAR;
  _M_inv_key_map[KEY_CLEAR] = VK_CLEAR;
  _M_key_map[VK_RETURN] = KEY_RETURN;
  _M_inv_key_map[KEY_RETURN] = VK_RETURN;
  _M_key_map[VK_PAUSE] = KEY_PAUSE;
  _M_inv_key_map[KEY_PAUSE] = VK_PAUSE;
  _M_key_map[VK_ESCAPE] = KEY_ESCAPE;
  _M_inv_key_map[KEY_ESCAPE] = VK_ESCAPE;
  _M_key_map[VK_SPACE] = KEY_SPACE;
  _M_inv_key_map[KEY_SPACE] = VK_SPACE;
  _M_key_map[VK_APOSTROPHE] = KEY_QUOTE;
  _M_inv_key_map[KEY_QUOTE] = VK_APOSTROPHE;
  _M_key_map[VK_COMMA] = KEY_COMMA;
  _M_inv_key_map[KEY_COMMA] = VK_COMMA;
  _M_key_map[VK_MINUS] = KEY_MINUS;
  _M_inv_key_map[KEY_MINUS] = VK_MINUS;
  _M_key_map[VK_PERIOD] = KEY_PERIOD;
  _M_inv_key_map[KEY_PERIOD] = VK_PERIOD;
  _M_key_map[VK_SLASH] = KEY_SLASH;
  _M_inv_key_map[KEY_SLASH] = VK_SLASH;
  _M_key_map[VK_0] = KEY_0;
  _M_inv_key_map[KEY_0] = VK_0;
  _M_key_map[VK_1] = KEY_1;
  _M_inv_key_map[KEY_1] = VK_1;
  _M_key_map[VK_2] = KEY_2;
  _M_inv_key_map[KEY_2] = VK_2;
  _M_key_map[VK_3] = KEY_3;
  _M_inv_key_map[KEY_3] = VK_3;
  _M_key_map[VK_4] = KEY_4;
  _M_inv_key_map[KEY_4] = VK_4;
  _M_key_map[VK_5] = KEY_5;
  _M_inv_key_map[KEY_5] = VK_5;
  _M_key_map[VK_6] = KEY_6;
  _M_inv_key_map[KEY_6] = VK_6;
  _M_key_map[VK_7] = KEY_7;
  _M_inv_key_map[KEY_7] = VK_7;
  _M_key_map[VK_8] = KEY_8;
  _M_inv_key_map[KEY_8] = VK_8;
  _M_key_map[VK_9] = KEY_9;
  _M_inv_key_map[KEY_9] = VK_9;
  _M_key_map[VK_SEMICOLON] = KEY_SEMICOLON;
  _M_inv_key_map[KEY_SEMICOLON] = VK_SEMICOLON;
  _M_key_map[VK_EQUALS] = KEY_EQUALS;
  _M_inv_key_map[KEY_EQUALS] = VK_EQUALS;
  _M_key_map[VK_LBRACKET] = KEY_LEFTBRACKET;
  _M_inv_key_map[KEY_LEFTBRACKET] = VK_LBRACKET;
  _M_key_map[VK_RBRACKET] = KEY_RIGHTBRACKET;
  _M_inv_key_map[KEY_RIGHTBRACKET] = VK_RBRACKET;
  _M_key_map[VK_GRAVE] = KEY_BACKQUOTE;
  _M_inv_key_map[KEY_BACKQUOTE] = VK_GRAVE;
  _M_key_map[VK_A] = KEY_a;
  _M_inv_key_map[KEY_a] = VK_A;
  _M_key_map[VK_B] = KEY_b;
  _M_inv_key_map[KEY_b] = VK_B;
  _M_key_map[VK_C] = KEY_c;
  _M_inv_key_map[KEY_c] = VK_C;
  _M_key_map[VK_D] = KEY_d;
  _M_inv_key_map[KEY_d] = VK_D;
  _M_key_map[VK_E] = KEY_e;
  _M_inv_key_map[KEY_e] = VK_E;
  _M_key_map[VK_F] = KEY_f;
  _M_inv_key_map[KEY_f] = VK_F;
  _M_key_map[VK_G] = KEY_g;
  _M_inv_key_map[KEY_g] = VK_G;
  _M_key_map[VK_H] = KEY_h;
  _M_inv_key_map[KEY_h] = VK_H;
  _M_key_map[VK_I] = KEY_i;
  _M_inv_key_map[KEY_i] = VK_I;
  _M_key_map[VK_J] = KEY_j;
  _M_inv_key_map[KEY_j] = VK_J;
  _M_key_map[VK_K] = KEY_k;
  _M_inv_key_map[KEY_k] = VK_K;
  _M_key_map[VK_L] = KEY_l;
  _M_inv_key_map[KEY_l] = VK_L;
  _M_key_map[VK_M] = KEY_m;
  _M_inv_key_map[KEY_m] = VK_M;
  _M_key_map[VK_N] = KEY_n;
  _M_inv_key_map[KEY_n] = VK_N;
  _M_key_map[VK_O] = KEY_o;
  _M_inv_key_map[KEY_o] = VK_O;
  _M_key_map[VK_P] = KEY_p;
  _M_inv_key_map[KEY_p] = VK_P;
  _M_key_map[VK_Q] = KEY_q;
  _M_inv_key_map[KEY_q] = VK_Q;
  _M_key_map[VK_R] = KEY_r;
  _M_inv_key_map[KEY_r] = VK_R;
  _M_key_map[VK_S] = KEY_s;
  _M_inv_key_map[KEY_s] = VK_S;
  _M_key_map[VK_T] = KEY_t;
  _M_inv_key_map[KEY_t] = VK_T;
  _M_key_map[VK_U] = KEY_u;
  _M_inv_key_map[KEY_u] = VK_U;
  _M_key_map[VK_V] = KEY_v;
  _M_inv_key_map[KEY_v] = VK_V;
  _M_key_map[VK_W] = KEY_w;
  _M_inv_key_map[KEY_w] = VK_W;
  _M_key_map[VK_X] = KEY_x;
  _M_inv_key_map[KEY_x] = VK_X;
  _M_key_map[VK_Y] = KEY_y;
  _M_inv_key_map[KEY_y] = VK_Y;
  _M_key_map[VK_Z] = KEY_z;
  _M_inv_key_map[KEY_z] = VK_Z;
  _M_key_map[VK_DELETE] = KEY_DELETE;
  _M_inv_key_map[KEY_DELETE] = VK_DELETE;
  _M_key_map[VK_NUMPAD0] = KEY_KP0;
  _M_inv_key_map[KEY_KP0] = VK_NUMPAD0;
  _M_key_map[VK_NUMPAD1] = KEY_KP1;
  _M_inv_key_map[KEY_KP1] = VK_NUMPAD1;
  _M_key_map[VK_NUMPAD2] = KEY_KP2;
  _M_inv_key_map[KEY_KP2] = VK_NUMPAD2;
  _M_key_map[VK_NUMPAD3] = KEY_KP3;
  _M_inv_key_map[KEY_KP3] = VK_NUMPAD3;
  _M_key_map[VK_NUMPAD4] = KEY_KP4;
  _M_inv_key_map[KEY_KP4] = VK_NUMPAD4;
  _M_key_map[VK_NUMPAD5] = KEY_KP5;
  _M_inv_key_map[KEY_KP5] = VK_NUMPAD5;
  _M_key_map[VK_NUMPAD6] = KEY_KP6;
  _M_inv_key_map[KEY_KP6] = VK_NUMPAD6;
  _M_key_map[VK_NUMPAD7] = KEY_KP7;
  _M_inv_key_map[KEY_KP7] = VK_NUMPAD7;
  _M_key_map[VK_NUMPAD8] = KEY_KP8;
  _M_inv_key_map[KEY_KP8] = VK_NUMPAD8;
  _M_key_map[VK_NUMPAD9] = KEY_KP9;
  _M_inv_key_map[KEY_KP9] = VK_NUMPAD9;
  _M_key_map[VK_DECIMAL] = KEY_KP_PERIOD;
  _M_inv_key_map[KEY_KP_PERIOD] = VK_DECIMAL;
  _M_key_map[VK_DIVIDE] = KEY_KP_DIVIDE;
  _M_inv_key_map[KEY_KP_DIVIDE] = VK_DIVIDE;
  _M_key_map[VK_MULTIPLY] = KEY_KP_MULTIPLY;
  _M_inv_key_map[KEY_KP_MULTIPLY] = VK_MULTIPLY;
  _M_key_map[VK_SUBTRACT] = KEY_KP_MINUS;
  _M_inv_key_map[KEY_KP_MINUS] = VK_SUBTRACT;
  _M_key_map[VK_ADD] = KEY_KP_PLUS;
  _M_inv_key_map[KEY_KP_PLUS] = VK_ADD;
  _M_key_map[VK_UP] = KEY_UP;
  _M_inv_key_map[KEY_UP] = VK_UP;
  _M_key_map[VK_DOWN] = KEY_DOWN;
  _M_inv_key_map[KEY_DOWN] = VK_DOWN;
  _M_key_map[VK_LEFT] = KEY_LEFT;
  _M_inv_key_map[KEY_LEFT] = VK_LEFT;
  _M_key_map[VK_RIGHT] = KEY_RIGHT;
  _M_inv_key_map[KEY_RIGHT] = VK_RIGHT;
  _M_key_map[VK_INSERT] = KEY_INSERT;
  _M_inv_key_map[KEY_INSERT] = VK_INSERT;
  _M_key_map[VK_HOME] = KEY_HOME;
  _M_inv_key_map[KEY_HOME] = VK_HOME;
  _M_key_map[VK_END] = KEY_END;
  _M_inv_key_map[KEY_END] = VK_END;
  _M_key_map[VK_PRIOR] = KEY_PAGEUP;
  _M_inv_key_map[KEY_PAGEUP] = VK_PRIOR;
  _M_key_map[VK_NEXT] = KEY_PAGEDOWN;
  _M_inv_key_map[KEY_PAGEDOWN] = VK_NEXT;
  _M_key_map[VK_F1] = KEY_F1;
  _M_inv_key_map[KEY_F1] = VK_F1;
  _M_key_map[VK_F2] = KEY_F2;
  _M_inv_key_map[KEY_F2] = VK_F2;
  _M_key_map[VK_F3] = KEY_F3;
  _M_inv_key_map[KEY_F3] = VK_F3;
  _M_key_map[VK_F4] = KEY_F4;
  _M_inv_key_map[KEY_F4] = VK_F4;
  _M_key_map[VK_F5] = KEY_F5;
  _M_inv_key_map[KEY_F5] = VK_F5;
  _M_key_map[VK_F6] = KEY_F6;
  _M_inv_key_map[KEY_F6] = VK_F6;
  _M_key_map[VK_F7] = KEY_F7;
  _M_inv_key_map[KEY_F7] = VK_F7;
  _M_key_map[VK_F8] = KEY_F8;
  _M_inv_key_map[KEY_F8] = VK_F8;
  _M_key_map[VK_F9] = KEY_F9;
  _M_inv_key_map[KEY_F9] = VK_F9;
  _M_key_map[VK_F10] = KEY_F10;
  _M_inv_key_map[KEY_F10] = VK_F10;
  _M_key_map[VK_F11] = KEY_F11;
  _M_inv_key_map[KEY_F11] = VK_F11;
  _M_key_map[VK_F12] = KEY_F12;
  _M_inv_key_map[KEY_F12] = VK_F12;
  _M_key_map[VK_F13] = KEY_F13;
  _M_inv_key_map[KEY_F13] = VK_F13;
  _M_key_map[VK_F14] = KEY_F14;
  _M_inv_key_map[KEY_F14] = VK_F14;
  _M_key_map[VK_F15] = KEY_F15;
  _M_inv_key_map[KEY_F15] = VK_F15;
  _M_key_map[VK_NUMLOCK] = KEY_NUMLOCK;
  _M_inv_key_map[KEY_NUMLOCK] = VK_NUMLOCK;
  _M_key_map[VK_CAPITAL] = KEY_CAPSLOCK;
  _M_inv_key_map[KEY_CAPSLOCK] = VK_CAPITAL;
  _M_key_map[VK_SCROLL] = KEY_SCROLLOCK;
  _M_inv_key_map[KEY_SCROLLOCK] = VK_SCROLL;
  _M_key_map[VK_RSHIFT] = KEY_RSHIFT;
  _M_inv_key_map[KEY_RSHIFT] = VK_RSHIFT;
  _M_key_map[VK_LSHIFT] = KEY_LSHIFT;
  _M_inv_key_map[KEY_LSHIFT] = VK_LSHIFT;
  _M_key_map[VK_RCONTROL] = KEY_RCTRL;
  _M_inv_key_map[KEY_RCTRL] = VK_RCONTROL;
  _M_key_map[VK_LCONTROL] = KEY_LCTRL;
  _M_inv_key_map[KEY_LCTRL] = VK_LCONTROL;
  _M_key_map[VK_RMENU] = KEY_RALT;
  _M_inv_key_map[KEY_RALT] = VK_RMENU;
  _M_key_map[VK_LMENU] = KEY_LALT;
  _M_inv_key_map[KEY_LALT] = VK_LMENU;
  _M_key_map[VK_LWIN] = KEY_LSUPER;
  _M_inv_key_map[KEY_LSUPER] = VK_LWIN;
  _M_key_map[VK_RWIN] = KEY_RSUPER;
  _M_inv_key_map[KEY_RSUPER] = VK_RWIN;
  _M_key_map[VK_HELP] = KEY_HELP;
  _M_inv_key_map[KEY_HELP] = VK_HELP;
  _M_key_map[VK_SNAPSHOT] = KEY_PRINT;
  _M_inv_key_map[KEY_PRINT] = VK_SNAPSHOT;
  _M_key_map[VK_CANCEL] = KEY_BREAK;
  _M_inv_key_map[KEY_BREAK] = VK_CANCEL;
  _M_key_map[VK_APPS] = KEY_MENU;
  _M_inv_key_map[KEY_MENU] = VK_APPS;
#endif
}

ATOM_Key TranslateKey(KEY from) {
  return _S_key_mapping.TranslateKey(from);
}

KEY InvTranslateKey(ATOM_Key from) {
  return _S_key_mapping.InvTranslateKey(from);
}

#define KEYNAME(name) case name: return #name;

const char *GetKeyDesc(ATOM_Key key)
{
	switch (key)
	{
		KEYNAME(KEY_UNKNOWN)
		KEYNAME(KEY_BACKSPACE)
		KEYNAME(KEY_TAB)
		KEYNAME(KEY_CLEAR)
		KEYNAME(KEY_RETURN)
		KEYNAME(KEY_PAUSE)
		KEYNAME(KEY_ESCAPE)
		KEYNAME(KEY_SPACE)
		KEYNAME(KEY_EXCLAIM)
		KEYNAME(KEY_QUOTEDBL)
		KEYNAME(KEY_HASH)
		KEYNAME(KEY_DOLLAR)
		KEYNAME(KEY_AMPERSAND)
		KEYNAME(KEY_QUOTE)
		KEYNAME(KEY_LEFTPAREN)
		KEYNAME(KEY_RIGHTPAREN)
		KEYNAME(KEY_ASTERISK)
		KEYNAME(KEY_PLUS)
		KEYNAME(KEY_COMMA)
		KEYNAME(KEY_MINUS)
		KEYNAME(KEY_PERIOD)
		KEYNAME(KEY_SLASH)
		KEYNAME(KEY_0)
		KEYNAME(KEY_1)
		KEYNAME(KEY_2)
		KEYNAME(KEY_3)
		KEYNAME(KEY_4)
		KEYNAME(KEY_5)
		KEYNAME(KEY_6)
		KEYNAME(KEY_7)
		KEYNAME(KEY_8)
		KEYNAME(KEY_9)
		KEYNAME(KEY_COLON)
		KEYNAME(KEY_SEMICOLON)
		KEYNAME(KEY_LESS)
		KEYNAME(KEY_EQUALS)
		KEYNAME(KEY_GREATER)
		KEYNAME(KEY_QUESTION)
		KEYNAME(KEY_AT)
		KEYNAME(KEY_LEFTBRACKET)
		KEYNAME(KEY_RIGHTBRACKET)
		KEYNAME(KEY_CARET)
		KEYNAME(KEY_UNDERSCORE)
		KEYNAME(KEY_BACKQUOTE)
		KEYNAME(KEY_a)
		KEYNAME(KEY_b)
		KEYNAME(KEY_c)
		KEYNAME(KEY_d)
		KEYNAME(KEY_e)
		KEYNAME(KEY_f)
		KEYNAME(KEY_g)
		KEYNAME(KEY_h)
		KEYNAME(KEY_i)
		KEYNAME(KEY_j)
		KEYNAME(KEY_k)
		KEYNAME(KEY_l)
		KEYNAME(KEY_m)
		KEYNAME(KEY_n)
		KEYNAME(KEY_o)
		KEYNAME(KEY_p)
		KEYNAME(KEY_q)
		KEYNAME(KEY_r)
		KEYNAME(KEY_s)
		KEYNAME(KEY_t)
		KEYNAME(KEY_u)
		KEYNAME(KEY_v)
		KEYNAME(KEY_w)
		KEYNAME(KEY_x)
		KEYNAME(KEY_y)
		KEYNAME(KEY_z)
		KEYNAME(KEY_DELETE)
		KEYNAME(KEY_KP0)
		KEYNAME(KEY_KP1)
		KEYNAME(KEY_KP2)
		KEYNAME(KEY_KP3)
		KEYNAME(KEY_KP4)
		KEYNAME(KEY_KP5)
		KEYNAME(KEY_KP6)
		KEYNAME(KEY_KP7)
		KEYNAME(KEY_KP8)
		KEYNAME(KEY_KP9)
		KEYNAME(KEY_KP_PERIOD)
		KEYNAME(KEY_KP_DIVIDE)
		KEYNAME(KEY_KP_MULTIPLY)
		KEYNAME(KEY_KP_MINUS)
		KEYNAME(KEY_KP_PLUS)
		KEYNAME(KEY_KP_ENTER)
		KEYNAME(KEY_KP_EQUALS)
		KEYNAME(KEY_UP)
		KEYNAME(KEY_DOWN)
		KEYNAME(KEY_LEFT)
		KEYNAME(KEY_RIGHT)
		KEYNAME(KEY_INSERT)
		KEYNAME(KEY_HOME)
		KEYNAME(KEY_END)
		KEYNAME(KEY_PAGEUP)
		KEYNAME(KEY_PAGEDOWN)
		KEYNAME(KEY_F1)
		KEYNAME(KEY_F2)
		KEYNAME(KEY_F3)
		KEYNAME(KEY_F4)
		KEYNAME(KEY_F5)
		KEYNAME(KEY_F6)
		KEYNAME(KEY_F7)
		KEYNAME(KEY_F8)
		KEYNAME(KEY_F9)
		KEYNAME(KEY_F10)
		KEYNAME(KEY_F11)
		KEYNAME(KEY_F12)
		KEYNAME(KEY_F13)
		KEYNAME(KEY_F14)
		KEYNAME(KEY_F15)
		KEYNAME(KEY_NUMLOCK)
		KEYNAME(KEY_CAPSLOCK)
		KEYNAME(KEY_SCROLLOCK)
		KEYNAME(KEY_RSHIFT)
		KEYNAME(KEY_LSHIFT)
		KEYNAME(KEY_RCTRL)
		KEYNAME(KEY_LCTRL)
		KEYNAME(KEY_RALT)
		KEYNAME(KEY_LALT)
		KEYNAME(KEY_RMETA)
		KEYNAME(KEY_LMETA)
		KEYNAME(KEY_LSUPER)
		KEYNAME(KEY_RSUPER)
		KEYNAME(KEY_MODE)
		KEYNAME(KEY_COMPOSE)
		KEYNAME(KEY_HELP)
		KEYNAME(KEY_PRINT)
		KEYNAME(KEY_SYSREQ)
		KEYNAME(KEY_BREAK)
		KEYNAME(KEY_MENU)
		KEYNAME(KEY_POWER)
		KEYNAME(KEY_EURO)
		KEYNAME(KEY_UNDO)
		KEYNAME(KEY_NULL)
	default: return "Unknown";
	}
}
