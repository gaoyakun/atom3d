#ifndef COLOR_H
#define COLOR_H

#define COLOR_RED     0xFFFF0000
#define COLOR_GREEN   0xFF00FF00
#define COLOR_BLUE    0xFF0000FF
#define COLOR_BLACK   0xFF000000
#define COLOR_WHITE   0xFFFFFFFF
#define COLOR_YELLOW  0xFFFFFF00
#define COLOR_CYAN    0xFF00FFFF
#define COLOR_MAGENTA 0xFFFF00FF
#define COLOR_GRAY    0xFF666666
#define COLOR_LGRAY   0xFFCCCCCC


#define INVERSE_255 1.0f/255.0f

class Color
{
public:
  float r;
  float g;
  float b;
  float a;       
   
   

  Color(){*((DWORD*)(this)) = 0xFFFFFFFF;}
  Color(DWORD in_color)
  {    
    r = (in_color& 0x00FF0000)>>16;r*=INVERSE_255;
    g = (in_color& 0x0000FF00)>>8;g*=INVERSE_255;
    b = in_color& 0xFF; b*=INVERSE_255;
    a = in_color>>24;a*=INVERSE_255;
  }
  Color(BYTE in_r, BYTE in_g, BYTE in_b, BYTE in_a = 255)
       {r = in_r*INVERSE_255; g = in_g*INVERSE_255; b = in_b*INVERSE_255; a = in_a*INVERSE_255;}

  Color(float in_r, float in_g, float in_b, float in_a = 1.0f)
       {r = in_r; g = in_g; b = in_b; a = in_a;}



  void Set(BYTE in_r, BYTE in_g, BYTE in_b, BYTE in_a = 255)
       {r = in_r*INVERSE_255; g = in_g*INVERSE_255; b = in_b*INVERSE_255; a = in_a*INVERSE_255;}

   void Set(float in_r, float in_g, float in_b, float in_a = 1.0f)
       {r = in_r; g = in_g; b = in_b; a = in_a;}

  void Set(float g)
  {
    Set(g,g,g,1);
  }

  void SetFloat(float* in_floats)
  {
    r = in_floats[0]; g = in_floats[1]; b = in_floats[2]; a = in_floats[3];
  }

  void GetFloat(float* out_floats)
  {
    out_floats[0] = r; out_floats[1] = g; out_floats[2] = b; out_floats[3] = a;
  }

  int GetInt()
  {
    int red = (int)(r*255)<<16;
    int green = (int)(g*255)<<8;
    int blue = (int)(b*255); 
    int alpha = (int)(a*255)<<24;

    return (red+green+blue+alpha);
  }

 

  inline void operator = (DWORD in_color) 
  {    
    r = ((in_color& 0x00FF0000)>>16)/255.0f;
    g = ((in_color& 0x0000FF00)>>8)/255.0f;
    b = (in_color& 0xFF)/255.0f;
    a = (in_color>>24)/255.0f;
  }

  void GetRGB3B(BYTE& in_r, BYTE& in_g, BYTE& in_b)
  {
    in_r = r*255;
    in_g = r*255;
    in_b = b*255;
  }

  void GetRGB3BV(BYTE* pRGB)
  {
    pRGB[0] = r*255;
    pRGB[1] = g*255;
    pRGB[2] = b*255;
  }

  void operator *= (DWORD in_color)
  {
    a *= (in_color>>24)/255.0f;
    r *= ((in_color& 0x00FF0000)>>16)/255.0f;;
    g *= ((in_color& 0x0000FF00)>>8)/255.0f;
    b *= (in_color& 0xFF)/255.0f;
  }
  void operator *= (Color& in_color)
  {
    a *= in_color.a;
    r *= in_color.r;
    g *= in_color.g;
    b *= in_color.b;    
  }

  Color& operator * (Color& in_color)
  {
    a *= in_color.a;
    r *= in_color.r;
    g *= in_color.g;
    b *= in_color.b;   
    return *this;
  }

   Color& operator + (Color& in_color)
  {
    a += in_color.a;
    r += in_color.r;
    g += in_color.g;
    b += in_color.b;   
    return *this;
  }

  bool operator == (Color& in_color)
  {
    return (r == in_color.r && g == in_color.g && b == in_color.b && a == in_color.a);
  }

  bool operator != (Color& in_color)
  {
    return (r != in_color.r || g != in_color.g || b != in_color.b || a != in_color.a);
  }

  Color Lerp(Color& in_color, float in_lerp)
  {
    float invLerp = 1.0f - in_lerp;
    Color out;
    out.a = a*in_lerp + in_color.a*invLerp;
    out.r = r*in_lerp + in_color.r*invLerp;
    out.g = g*in_lerp + in_color.g*invLerp;
    out.b = b*in_lerp + in_color.b*invLerp;

    return out;
  }

  void Normalize()
  {
    if(a > 1.0f)
      a = 1.0f;
    else if(a < 0.0f)
      a = 0.0f;
    if(r > 1.0f)
      r = 1.0f;
    else if(r < 0.0f)
      r = 0.0f;
    if(g > 1.0f)
      g = 1.0f;
    else if(g < 0.0f)
      g = 0.0f;
    if(b > 1.0f)
      b = 1.0f;
    else if(b < 0.0f)
      b = 0.0f;
  }
};

inline void glColor3(Color &color) 
{
	glColor3fv((float*)&color);
}

inline void glColor4(Color &color) 
{
	glColor4fv((float*)&color);
}

inline void glColor(Color &color) 
{
	glColor4(color);
}


#endif