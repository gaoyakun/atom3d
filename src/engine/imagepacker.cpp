#include "stdafx.h"
#include "imagepacker.h"

ImagePackerNode::~ImagePackerNode () {
  ATOM_DELETE(left);
  ATOM_DELETE(right);
}

ImagePackerNode::ImagePackerNode (const ImagePackerNode &rhs) {
  parent = rhs.parent;
  left = rhs.left ? ATOM_NEW(ImagePackerNode, *rhs.left) : 0;
  right = rhs.right ? ATOM_NEW(ImagePackerNode, *rhs.right) : 0;
  if (left) left->parent = this;
  if (right) right->parent = this;
  hsplit = rhs.hsplit;
  empty = rhs.empty;
  width = rhs.width;
  height = rhs.height;
  positionx = rhs.positionx;
  positiony = rhs.positiony;
}

ImagePackerNode &ImagePackerNode::operator = (const ImagePackerNode &rhs) {
  if (&rhs != this)
  {
	ImagePackerNode temp (rhs);
	swap (temp);
  }
  return *this;
}

void ImagePackerNode::swap (ImagePackerNode &rhs) {
  std::swap (parent, rhs.parent);
  std::swap (left, rhs.left);
  std::swap (right, rhs.right);
  std::swap (hsplit, rhs.hsplit);
  std::swap (empty, rhs.empty);
  std::swap (width, rhs.width);
  std::swap (height, rhs.height);
  std::swap (positionx, rhs.positionx);
  std::swap (positiony, rhs.positiony);
}

bool ImagePackerNode::verify () const {
#if 0
  if (left  && this != left->getParent ())
    return false;

  if (right && this != right->getParent ())
    return false;

  ATOM_LOGGER::Log ("left = 0x%08X, right = 0x%08X\n", left, right);

  if (left && !left->verify())
    return false;

  if (right && !right->verify())
    return false;
#endif
  return true;
}

ImagePackerNode *ImagePackerNode::split (int w, int h) {
  if (!isLeaf ())
  {
    ImagePackerNode *ret = left->split (w, h);
    if (!ret)
      ret = right->split (w, h);
    return ret;
  }
  else if (isSolidLeaf ())
  {
    return 0;
  }
  else
  {
    if (w > width || h > height)
      return 0;

    if (w == width && h == height)
    {
      return this;
    }
    else if (w == width)
    {
      hsplit = false;
      left = ATOM_NEW(ImagePackerNode, this, positionx, positiony, w, h);
      right = ATOM_NEW(ImagePackerNode, this, positionx, positiony + h, w, height - h);
      return left;
    }
    else if (h == height)
    {
      hsplit = true;
      left = ATOM_NEW(ImagePackerNode, this, positionx, positiony, w, h);
      right = ATOM_NEW(ImagePackerNode, this, positionx + w, positiony, width - w, h);
      return left;
    }
    else if (width - w < height - h)
    {
      hsplit = false;
      left = ATOM_NEW(ImagePackerNode, this, positionx, positiony, width, h);
      right = ATOM_NEW(ImagePackerNode, this, positionx, positiony + h, width, height - h);
      return left->split (w, h);
    }
    else
    {
      hsplit = true;
      left = ATOM_NEW(ImagePackerNode, this, positionx, positiony, w, height);
      right = ATOM_NEW(ImagePackerNode, this, positionx + w, positiony, width - w, height);
      return left->split (w, h);
    }
  }
}

ImagePacker::ImagePacker (int w, int h) {
  ATOM_ASSERT (w > 0 && h > 0);
  imagewidth = w;
  imageheight = h;
  rootnode = ATOM_NEW(ImagePackerNode, (ImagePackerNode*)0, 0, 0, w, h);
  image = ATOM_NEW_ARRAY(unsigned char, w * h * 4);
  memset (image, 0, w * h * 4);
}

ImagePacker::~ImagePacker () {
  ATOM_DELETE(rootnode);
  ATOM_DELETE_ARRAY(image);
}

bool ImagePacker::grow (int new_width, int new_height) {
  ATOM_ASSERT (rootnode->verify ());
  ATOM_ASSERT (new_width >= imagewidth && new_height >= imageheight);

  if (new_width == imagewidth && new_height == imageheight)
    return true;

  ImagePackerNode *newroot = ATOM_NEW(ImagePackerNode, (ImagePackerNode*)0, 0, 0, new_width, new_height);
  if (new_width == imagewidth || new_height == imageheight)
  {
    newroot->split (imagewidth, imageheight);
    newroot->setLeft (rootnode);
  }
  else
  {
    newroot->split (new_width, imageheight)->split (imagewidth, imageheight);
    newroot->getLeft()->setLeft (rootnode);
  }
  rootnode = newroot;

  try
  {
    unsigned char *image2 = ATOM_NEW_ARRAY(unsigned char, new_width * new_height * 4);
    memset (image2, 0, new_width * new_height * 4);
    for (unsigned i = 0; i < imageheight; ++i)
      memcpy (image2 + i * new_width * 4, image + i * imagewidth * 4, imagewidth * 4);
    ATOM_DELETE_ARRAY(image);
    image = image2;
    imagewidth = new_width;
    imageheight = new_height;
    ATOM_ASSERT (rootnode->verify ());
  }
  catch (std::bad_alloc)
  {
    return false;
  }

  return true;
}

ImagePackerNode *ImagePacker::put (int width, int height, unsigned char *data) {
  ImagePackerNode *node = rootnode->split (width, height);
  if (node) 
  {
    ATOM_ASSERT (node->getWidth() == width);
    ATOM_ASSERT (node->getHeight() == height);
    unsigned char *src = data;
    unsigned char *dst = image + node->getY() * imagewidth * 4 + node->getX() * 4;
    for (int i = 0; i < height; ++i)
    {
      memcpy (dst, src, width * 4);
      src += width * 4;
      dst += imagewidth * 4;
    }
    node->setEmpty (false);
    return node;
  }
  else
  {
    if (imagewidth > imageheight)
    {
      if (!grow (imagewidth, imageheight * 2))
        return 0;
    }
    else
    {
      if (!grow (imagewidth * 2, imageheight))
        return 0;
    }

    return put (width, height, data);
  }
}

ImagePackerNode *ImagePacker::put (int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  ATOM_ASSERT (rootnode->verify ());
  ImagePackerNode *node = rootnode->split (width, height);
  ATOM_ASSERT (rootnode->verify ());
  if (node) 
  {
    ATOM_ASSERT (node->getWidth() == width);
    ATOM_ASSERT (node->getHeight() == height);
    unsigned char *dst = image + node->getY() * imagewidth * 4 + node->getX() * 4;
    for (int i = 0; i < height; ++i)
    {
      for (int j = 0; j < width; ++j)
      {
        dst[j*4+0] = r;
        dst[j*4+1] = g;
        dst[j*4+2] = b;
		dst[j*4+3] = a;
      }
      dst += imagewidth * 4;
    }
    node->setEmpty (false);
    return node;
  }
  else
  {
    if (imagewidth > imageheight)
      grow (imagewidth, imageheight * 2);
    else
      grow (imagewidth * 2, imageheight);

    return put (width, height, r, g, b, a);
  }
}

bool ImagePacker::putImage (unsigned id, int width, int height, unsigned char *data) {
  ATOM_ASSERT (fragments.find (id) == fragments.end());
  ImagePackerNode *node = put (width, height, data);
  if (node)
  {
    Rect v(node->getX(), node->getY(), node->getWidth(), node->getHeight());
    fragments[id] = v;
    return true;
  }
  return false;
}

bool ImagePacker::putImage (unsigned id, int width, int height, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
  ATOM_ASSERT (fragments.find (id) == fragments.end());
  ImagePackerNode *node = put (width, height, r, g, b, a);
  if (node)
  {
    Rect v(node->getX(), node->getY(), node->getWidth(), node->getHeight());
    fragments[id] = v;
    return true;
  }
  return false;
}

const ImagePacker::Rect *ImagePacker::getImage (int id) const {
  ATOM_HASHMAP<unsigned, Rect>::const_iterator it = fragments.find (id);
  return (it == fragments.end()) ? 0 : &(it->second);
}


