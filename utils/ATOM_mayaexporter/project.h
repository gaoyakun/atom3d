#ifndef __ATOM_TOOLS_MAYAEXPORTER_PROJECT_H
#define __ATOM_TOOLS_MAYAEXPORTER_PROJECT_H

#if _MSC_VER > 1000
# pragma once
#endif

class Project
{
public:
	Project (void);
	~Project (void);

private:
	ExportOptions *_options;
	VertexAnimationInfo *_actionInfo;
	SkeletonViewer *_skeletonViewer;
	MaterialViewer *_materialViewer;
	MeshViewer *_meshViewer;
};

#endif // __ATOM_TOOLS_MAYAEXPORTER_PROJECT_H