#ifndef __ATOM_FONT_RENDERCALLBACK_H
#define __ATOM_FONT_RENDERCALLBACK_H

class ATOM_FontRenderCallback 
{
public:
  virtual ~ATOM_FontRenderCallback (void) {}
  virtual void render (int w, int h, int pitch, const unsigned char *alpha, int x, int y, bool mono, void **userdata) = 0;
};

#endif // __ATOM_FONT_RENDERCALLBACK_H
