#ifndef __ATOM3D_KERNEL_FILEITERATOR_H
#define __ATOM3D_KERNEL_FILEITERATOR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_vfs.h"

class ATOM_KERNEL_API ATOM_FileIterator: private ATOM_Noncopyable
{
public:
	ATOM_FileIterator (void);
	ATOM_FileIterator (const char *dir);
	~ATOM_FileIterator (void);

public:
	ATOM_VFS::finddata_t *getData (void) const;
	void open (const char *dir);
	void close (void);
	void next (void);
	bool isValid (void) const;

private:
	ATOM_VFS::finddata_t *_findData;
	bool _valid;
};

#endif // __ATOM3D_KERNEL_FILEITERATOR_H
