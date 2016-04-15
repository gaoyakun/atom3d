#ifndef __ATOM3D_STUDIO_CLIPBOARD_H
#define __ATOM3D_STUDIO_CLIPBOARD_H

#if _MSC_VER > 1000
# pragma once
#endif

class AS_Clipboard
{
public:
	AS_Clipboard (void);
	virtual ~AS_Clipboard (void);

public:
	static const int CONTENT_TYPE_INVALID = 0;

public:
	void clear (void);
	int allocContentType (const char *name);
	const char *getContentName (int contentType) const;
	int getContentType (const char *name) const;
	bool setContent (int type, const void *data, unsigned size);
	bool setContent (const char *name, const void *data, unsigned size);
	int getContent (void *data) const;
	unsigned getContentSize (void) const;

private:
	int _nextContentType;
	int _contentType;
	void *_data;
	unsigned _dataSize;
	ATOM_HASHMAP<ATOM_STRING, int> _typeMap;
	ATOM_VECTOR<ATOM_STRING> _nameTable;
};

#endif // __ATOM3D_STUDIO_CLIPBOARD_H
