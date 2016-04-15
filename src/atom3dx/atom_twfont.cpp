#include "atom_twfont.h"

CAtom3DTextObject::CAtom3DTextObject (void)
{
}

CAtom3DTextObject::~CAtom3DTextObject (void)
{
}

void CAtom3DTextObject::appendLine (ATOM_Text *text, ATOM_ColorARGB fg, ATOM_ColorARGB bg, int charHeight, int lineMargin, int bgWidth)
{
	_lines.resize (_lines.size() + 1);
	_lines.back().line = text;
	_lines.back().foregroundColor = fg;
	_lines.back().backgroundColor = bg;
	_lines.back().lineMargin = charHeight + lineMargin;

	if( bgWidth > 0 )
	{
		int x = 0;
		int y = (_lines.size() - 1) * (charHeight + lineMargin);
		int y1 = y+charHeight;

		CBgVtx BgVtx;
		BgVtx._pos[2] = 0;
		BgVtx._pos[3] = 1;
		BgVtx._color = bg;

		BgVtx._pos[0] = -1;
		BgVtx._pos[1] = (float)y;
		_bgVerts.push_back(BgVtx);

		BgVtx._pos[0] = (float)(bgWidth+1);
		BgVtx._pos[1] = (float)y;
		_bgVerts.push_back(BgVtx);

		BgVtx._pos[0] = -1;
		BgVtx._pos[1] = (float)y1;
		_bgVerts.push_back(BgVtx);

		BgVtx._pos[0] = (float)(bgWidth+1);
		BgVtx._pos[1] = (float)y;
		_bgVerts.push_back(BgVtx);

		BgVtx._pos[0] = (float)(bgWidth+1);
		BgVtx._pos[1] = (float)y1;
		_bgVerts.push_back(BgVtx);

		BgVtx._pos[0] = -1;
		BgVtx._pos[1] = (float)y1;
		_bgVerts.push_back(BgVtx);
	}
}

unsigned CAtom3DTextObject::getNumLines (void) const
{
	return _lines.size();
}

void CAtom3DTextObject::removeAllLines (void)
{
	_lines.clear ();
	_bgVerts.clear ();
}

const CAtom3DTextObject::Line *CAtom3DTextObject::getLine (unsigned lineIndex) const
{
	return &_lines[lineIndex];
}

ATOM_VECTOR<CAtom3DTextObject::CBgVtx> *CAtom3DTextObject::getBgVertices (void)
{
	return &_bgVerts;
}

