#ifndef __ATOM3DX_ATOM_TWFONT_H
#define __ATOM3DX_ATOM_TWFONT_H

class CAtom3DTextObject
{
public:
	struct CBgVtx
	{
		float		_pos[4];
		unsigned    _color;
	};

	struct Line
	{
		ATOM_AUTOREF(ATOM_Text) line;
		ATOM_ColorARGB foregroundColor;
		ATOM_ColorARGB backgroundColor;
		int lineMargin;
	};

public:
	CAtom3DTextObject (void);
	~CAtom3DTextObject (void);

public:
	void appendLine (ATOM_Text *text, ATOM_ColorARGB fg, ATOM_ColorARGB bg, int charHeight, int lineMargin, int bgWidth);
	unsigned getNumLines (void) const;
	const Line *getLine (unsigned lineIndex) const;
	ATOM_VECTOR<CBgVtx> *getBgVertices (void);
	void removeAllLines (void);

private:
	ATOM_VECTOR<Line> _lines;
	ATOM_VECTOR<CBgVtx> _bgVerts;
};

#endif // __ATOM3DX_ATOM_TWFONT_H
