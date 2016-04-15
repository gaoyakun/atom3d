#ifndef __ATOM3D_ENGINE_IMAGEPACKER_H
#define __ATOM3D_ENGINE_IMAGEPACKER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <ATOM_math.h>
#include <ATOM_utils.h>



class ImagePackerNode
{
public:
    ImagePackerNode (ImagePackerNode *owner, int x, int y, int w, int h);
    ImagePackerNode (const ImagePackerNode &rhs);
    ~ImagePackerNode ();
    ImagePackerNode & operator = (const ImagePackerNode &rhs);
    void swap (ImagePackerNode &rhs);

public:
    ImagePackerNode *getLeft () const;
    void setLeft (ImagePackerNode *p);
    ImagePackerNode *getRight () const;
    void setRight (ImagePackerNode *p);
    ImagePackerNode *getParent () const;
    void setParent (ImagePackerNode *owner);
    void setEmpty (bool b);
    bool isHorizonSplit () const;
    bool isVerticalSplit () const; 
    bool isLeaf () const;
    bool isEmptyLeaf () const;
    bool isSolidLeaf () const;
    int getWidth () const;
    int getHeight () const;
    int getX () const;
    void setX (int x);
    int getY () const;
    void setY (int y);

public:
    ImagePackerNode *split (int w, int h);
    bool verify () const;

private:
    ImagePackerNode *left;
    ImagePackerNode *right;
    ImagePackerNode *parent;
    bool hsplit;
    bool empty;
    int width;
    int height;
    int positionx;
    int positiony;
};

class ImagePacker
{
public:
	struct Rect
	{
	  int x, y, w, h;

	  Rect(void):x(0),y(0),w(0),h(0) {}
	  Rect(int _x, int _y, int _w, int _h):x(_x),y(_y),w(_w),h(_h) {}
	};

public:
    ImagePacker (int w, int h);
    ~ImagePacker (void);
public:
    int getWidth (void) const { return imagewidth; }
    int getHeight (void) const { return imageheight; }
    ImagePackerNode *getRootNode (void) const { return rootnode; }
    unsigned char *getImage (void) const { return image; }
  private:
    ImagePackerNode *put (int width, int height, unsigned char *data);
    ImagePackerNode *put (int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
  public:
    bool grow (int new_width, int new_height);
    bool putImage (unsigned id, int width, int height, unsigned char *data);
    bool putImage (unsigned id, int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	const ImagePacker::Rect *getImage (int id) const;

  private:
    int imagewidth;
    int imageheight;
    ImagePackerNode *rootnode;
    unsigned char *image;
    ATOM_HASHMAP<unsigned, Rect> fragments;
};

inline ImagePackerNode::ImagePackerNode (ImagePackerNode *owner, int x, int y, int w, int h) {
  parent = owner;
  left = 0;
  right = 0;
  hsplit = false;
  empty = true;
  width = w;
  height = h;
  positionx = x;
  positiony = y;
}

inline void ImagePackerNode::setEmpty (bool b) {
  empty = b;
}

inline ImagePackerNode *ImagePackerNode::getLeft () const { 
  return left; 
}

inline void ImagePackerNode::setLeft (ImagePackerNode *p) { 
  if (p != left)
  {
    ATOM_DELETE(left);
    left = p; 
    if (left)
      left->setParent (this);
  }
}

inline ImagePackerNode *ImagePackerNode::getRight () const { 
  return right; 
}

inline void ImagePackerNode::setRight (ImagePackerNode *p) { 
  if (right != p)
  {
    ATOM_DELETE(right);
    right = p; 
    if (right)
      right->setParent (this);
  }
}

inline ImagePackerNode *ImagePackerNode::getParent () const { 
  return parent; 
}

inline void ImagePackerNode::setParent (ImagePackerNode *owner) { 
  parent = owner; 
}

inline bool ImagePackerNode::isHorizonSplit () const { 
  return hsplit && !isLeaf(); 
}

inline bool ImagePackerNode::isVerticalSplit () const { 
  return !hsplit && !isLeaf(); 
}

inline bool ImagePackerNode::isLeaf () const { 
  return !left && !right; 
}

inline bool ImagePackerNode::isEmptyLeaf () const { 
  return isLeaf() && empty; 
}

inline bool ImagePackerNode::isSolidLeaf () const { 
  return isLeaf() && !empty; 
}

inline int ImagePackerNode::getWidth () const { 
  return width; 
}

inline int ImagePackerNode::getHeight () const { 
  return height; 
}

inline int ImagePackerNode::getX () const { 
  return positionx; 
}

inline void ImagePackerNode::setX (int x) {
  if (x != positionx)
  {
    if (left)
      left->setX (left->getX() + (x - positionx));

    if (right)
      right->setX (right->getX() + (x - positionx));

    positionx = x;
  }
}

inline int ImagePackerNode::getY () const { 
  return positiony; 
}

inline void ImagePackerNode::setY (int y) {
  if (y != positiony)
  {
    if (left)
      left->setY (left->getY() + (y - positiony));

    if (right)
      right->setY (right->getY() + (y - positiony));

    positiony = y;
  }
}


#endif // __ATOM3D_ENGINE_IMAGEPACKER_H
