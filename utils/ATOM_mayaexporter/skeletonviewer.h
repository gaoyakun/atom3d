#ifndef __ATOM_MAYAEXPORTER_SKELETONVIEWER_H
#define __ATOM_MAYAEXPORTER_SKELETONVIEWER_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <vector>
#include <list>
#include <windows.h>
#include <commctrl.h>

class ATOM_Skeleton;
class CMeshData;

struct SkeletonInfo
{
	ATOM_Skeleton *skeleton;
	std::vector<HTREEITEM> treeItems;
};

class SkeletonViewer
{
public:
	SkeletonViewer (void);
	~SkeletonViewer (void);

public:
	bool setSkeleton (ATOM_Skeleton *skeleton, const std::list<CMeshData*> &meshList, bool selectBoneByName);
	bool showDialog (HWND hWndParent);
	SkeletonInfo &getSkeleton (void);
	const SkeletonInfo &getSkeleton (void) const;
	void syncUI (HWND hWnd);
	void setCheckState (HWND treeCtrl, HTREEITEM item, BOOL check);
	void clear (void);
	void trackMenu (HWND hWnd, int x, int y);

private:
	void initTree (HWND treeCtrl);
	void setCheckStateUp (HWND treeCtrl, HTREEITEM item, BOOL check);
	void setCheckStateR_down (HWND treeCtrl, HTREEITEM item, BOOL check);

private:
	SkeletonInfo _skeletonInfo;
	std::vector<int> _remap;
};

#endif // __ATOM_MAYAEXPORTER_SKELETONVIEWER_H

