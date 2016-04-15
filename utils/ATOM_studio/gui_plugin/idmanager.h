#ifndef __ATOM3D_STUDIO_GUIEDITOR_IDMANAGER_H
#define __ATOM3D_STUDIO_GUIEDITOR_IDMANAGER_H

#include <ATOM_dbghlp.h>

class IdManager
{
public:
	static bool findId (int id);
	static bool addId (int id);
	static bool removeId (int id);
	static void clear (void);
	static int aquireId (int minValue);
	static int peekId (int minValue);

private:
	static ATOM_SET<int> _idSet;
};

#endif // __ATOM3D_STUDIO_GUIEDITOR_IDMANAGER_H
