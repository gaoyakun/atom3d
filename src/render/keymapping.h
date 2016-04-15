#ifndef __ATOM_GLRENDER_KEYMAPPING_H
#define __ATOM_GLRENDER_KEYMAPPING_H

#if _MSC_VER > 1000
# pragma once
#endif

#if defined(__SDL_WINDOW__)
# include <SDL.h>
# define KEY SDLKey
# define NULLKEY  SDLK_UNKNOWN
# define MAXNUMKEYS SDLK_LAST
#elif defined(WIN32)
# include <windows.h>
# define KEY int
# define NULLKEY  0
# define MAXNUMKEYS 256
# ifndef VK_0
#   define VK_0 '0'
#   define VK_1 '1'
#   define VK_2 '2'
#   define VK_3 '3'
#   define VK_4 '4'
#   define VK_5 '5'
#   define VK_6 '6'
#   define VK_7 '7'
#   define VK_8 '8'
#   define VK_9 '9'
#   define VK_A 'A'
#   define VK_B 'B'
#   define VK_C 'C'
#   define VK_D 'D'
#   define VK_E 'E'
#   define VK_F 'F'
#   define VK_G 'G'
#   define VK_H 'H'
#   define VK_I 'I'
#   define VK_J 'J'
#   define VK_K 'K'
#   define VK_L 'L'
#   define VK_M 'M'
#   define VK_N 'N'
#   define VK_O 'O'
#   define VK_P 'P'
#   define VK_Q 'Q'
#   define VK_R 'R'
#   define VK_S 'S'
#   define VK_T 'T'
#   define VK_U 'U'
#   define VK_V 'V'
#   define VK_W 'W'
#   define VK_X 'X'
#   define VK_Y 'Y'
#   define VK_Z 'Z'
# endif /* VK_0 */
# define VK_SEMICOLON   0xBA
# define VK_EQUALS      0xBB
# define VK_COMMA       0xBC
# define VK_MINUS       0xBD
# define VK_PERIOD      0xBE
# define VK_SLASH       0xBF
# define VK_GRAVE       0xC0
# define VK_LBRACKET    0xDB
# define VK_BACKSLASH   0xDC
# define VK_RBRACKET    0xDD
# define VK_APOSTROPHE  0xDE
# define VK_BACKTICK    0xDF
#endif // __SDL_WINDOW__

#include "keysymbols.h"

extern ATOM_Key TranslateKey(KEY from);
extern KEY InvTranslateKey(ATOM_Key from);
extern const char *GetKeyDesc(ATOM_Key key);

class KeyMapping
  {
  public:
    KeyMapping();
    ATOM_Key TranslateKey(KEY from)  const {
      return (from >= 0 && from < MAXNUMKEYS) ? _M_key_map[from] : KEY_UNKNOWN;
    }
    KEY InvTranslateKey(ATOM_Key from) const {
      return _M_inv_key_map[from];
    }
  private:
    ATOM_Key _M_key_map[MAXNUMKEYS];
    KEY _M_inv_key_map[KEY_COUNT];
  };

#endif // __ATOM_GLRENDER_KEYMAPPING_H
