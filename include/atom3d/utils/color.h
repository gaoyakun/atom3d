#ifndef __ATOM_UTILS_COLOR_H
#define __ATOM_UTILS_COLOR_H

#include "../ATOM_math.h"

#if _MSC_VER > 1000
# pragma once
#endif

class ATOM_ColorARGB
{
  friend bool operator == (const ATOM_ColorARGB &, const ATOM_ColorARGB &);
  friend bool operator != (const ATOM_ColorARGB &, const ATOM_ColorARGB &);
  friend bool operator < (const ATOM_ColorARGB &, const ATOM_ColorARGB &);
  friend bool operator <= (const ATOM_ColorARGB &, const ATOM_ColorARGB &);
  friend bool operator > (const ATOM_ColorARGB &, const ATOM_ColorARGB &);
  friend bool operator >= (const ATOM_ColorARGB &, const ATOM_ColorARGB &);

public:
  ATOM_ColorARGB (void);
  ATOM_ColorARGB (unsigned c);
  ATOM_ColorARGB (float r, float g, float b, float a);

  void setRaw (unsigned c);
  unsigned getRaw (void) const;
  void setFloats (float r, float g, float b, float a);
  void getFloats (float *r, float *g, float *b, float *a) const;
  float getFloatR (void) const;
  float getFloatG (void) const;
  float getFloatB (void) const;
  float getFloatA (void) const;
  void setBytes (unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  void getBytes (unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) const;
  unsigned char getByteR (void) const;
  unsigned char getByteG (void) const;
  unsigned char getByteB (void) const;
  unsigned char getByteA (void) const;

  operator unsigned int (void) const;

private:
  unsigned _color;
};

inline
ATOM_ColorARGB::ATOM_ColorARGB (void)
{
}

inline
ATOM_ColorARGB::ATOM_ColorARGB (unsigned c): _color(c)
{
}

inline
ATOM_ColorARGB::ATOM_ColorARGB (float r, float g, float b, float a)
{
  setFloats (r, g, b, a);
}

inline
ATOM_ColorARGB::operator unsigned int (void) const
{
  return _color;
}

inline
void ATOM_ColorARGB::setFloats (float r, float g, float b, float a)
{
	unsigned char br = ATOM_ftol (r * 255);
	unsigned char bg = ATOM_ftol (g * 255);
	unsigned char bb = ATOM_ftol (b * 255);
	unsigned char ba = ATOM_ftol (a * 255);
	setBytes (br, bg, bb, ba);
}

inline
void ATOM_ColorARGB::getFloats (float *r, float *g, float *b, float *a) const
{
  *r = getFloatR ();
  *g = getFloatG ();
  *b = getFloatB ();
  *a = getFloatA ();
}

inline
float ATOM_ColorARGB::getFloatR (void) const
{
  return getByteR () / 255.f;
}

inline
float ATOM_ColorARGB::getFloatG (void) const
{
  return getByteG () / 255.f;
}

inline
float ATOM_ColorARGB::getFloatB (void) const
{
  return getByteB () / 255.f;
}

inline
float ATOM_ColorARGB::getFloatA (void) const
{
  return getByteA () / 255.f;
}

inline
void ATOM_ColorARGB::setBytes (unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  _color = (((unsigned)a) << 24) | (((unsigned)r) << 16) | (((unsigned)g) << 8) | ((unsigned)b);
}

inline
void ATOM_ColorARGB::getBytes (unsigned char *r, unsigned char *g, unsigned char *b, unsigned char *a) const
{
  *r = getByteR ();
  *g = getByteG ();
  *b = getByteB ();
  *a = getByteA ();
}

inline
unsigned char ATOM_ColorARGB::getByteR (void) const
{
  return (_color >> 16) & 0xFF;
}

inline
unsigned char ATOM_ColorARGB::getByteG (void) const
{
  return (_color >> 8) & 0xFF;
}

inline
unsigned char ATOM_ColorARGB::getByteB (void) const
{
  return _color & 0xFF;
}

inline
unsigned char ATOM_ColorARGB::getByteA (void) const
{
  return (_color >> 24) & 0xFF;
}

inline 
void ATOM_ColorARGB::setRaw (unsigned c)
{
  _color = c;
}

inline
unsigned ATOM_ColorARGB::getRaw (void) const
{
  return _color;
}

inline
bool operator == (const ATOM_ColorARGB &c1, const ATOM_ColorARGB &c2)
{
  return c1._color == c2._color;
}

inline
bool operator != (const ATOM_ColorARGB &c1, const ATOM_ColorARGB &c2)
{
  return c1._color != c2._color;
}

inline
bool operator < (const ATOM_ColorARGB &c1, const ATOM_ColorARGB &c2)
{
  return c1._color < c2._color;
}

inline
bool operator <= (const ATOM_ColorARGB &c1, const ATOM_ColorARGB &c2)
{
  return c1._color <= c2._color;
}

inline
bool operator > (const ATOM_ColorARGB &c1, const ATOM_ColorARGB &c2)
{
  return c1._color > c2._color;
}

inline
bool operator >= (const ATOM_ColorARGB &c1, const ATOM_ColorARGB &c2)
{
  return c1._color >= c2._color;
}

#endif // __ATOM_UTILS_COLOR_H
