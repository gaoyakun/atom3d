// Copyright (c) 2007 Ernest Szoka
#ifndef IMAGE_H
#define IMAGE_H

#define RED 0X000000FF
#define GREEN 0X0000FF00
#define BLUE 0X00FF0000
#define ALPHA 0XFF000000



enum colorType
{
  COLOR_GRAYSCALE,
  COLOR_RGB,
  COLOR_RGBA
};

//all image data is stored upside down (to conform to DDS standard alignment)
// so bmps,tgas are flipped vertically
class ES_API Image
{
public:
  BYTE *    m_pData;	          //Imaga Data
	int	      m_width;
	int   	  m_height;
  int       m_bytesPerPixel;
  colorType m_colorType;        //Gray, RGB, RGBA
	char*		  m_sName;				    //Image Name
  GLenum    m_compression;      //Image Compression Property (DDS only)
  int       m_numMipMaps;       //mipmaps (DDS only)
  bool      m_bForce8BitDepth;  // force image to use 16bit(and not downsample, usefull for heightmaps)


	Image();
	Image(Image& in_image); //copy constructor
  Image(BYTE* in_pImgData, int in_width, int in_height, int in_bytesPerPixel,char* in_sName = NULL);
	virtual ~Image();
  virtual void Set(BYTE* in_pImgData, int in_width, int in_height, int in_bytesPerPixel, char* in_sName = NULL);
	virtual void Unload(); //delete Image data

  virtual bool Load(LPCTSTR in_sName); //Loads TGA,BMP, DDS, PNG
  bool Save(); //Saves TGA, PNG only, set m_sName with appropriate extention: .png .tga

  void Force8BitDepth(bool in_bEnable){m_bForce8BitDepth = in_bEnable;}
  void FlipVertical();
  int  GetBitDepthPerChannel();
  BYTE* GetPixel(int x, int y, float scale);
  BYTE* GetPixelf(float x, float y, float scale); //0-1


  void AddProgress(float amount); //adds image loading/saving to progress bar (if present)

private:
  void SetColorType();

  bool LoadTGA();
  bool LoadBMP();
  bool LoadDDS();
  bool LoadPNG();

  bool SaveTGA();
  bool SavePNG();

  static float m_progressAmount;
};

#endif