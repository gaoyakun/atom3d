#ifndef __ATOM3DX_H
#define __ATOM3DX_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../atom3d/ATOM_engine.h"
#include "types.h"
#include "tw_events.h"
#include "twbar.h"
#include "twpropbar.h"
#include "trackball.h"
#include "trackballnode.h"
#include "skeleton_visualizer.h"

// misc
ATOMX_API void ATOMX_CALL ATOMX_RegisterObjects (void);

// shapes
ATOMX_API ATOM_Node * ATOMX_CALL ATOMX_FindFirstNodeByClassName (ATOM_Node *rootNode, const char *classname);
ATOMX_API ATOM_Node * ATOMX_CALL ATOMX_FindFirstNodeByObjectName (ATOM_Node *rootNode, const char *objectname);
ATOMX_API bool ATOMX_CALL ATOMX_ReadFile (const char *filename, bool text, ATOM_VECTOR<char> &buffer);
ATOMX_API void ATOMX_CALL ATOMX_CreateCylinder (const ATOM_Vector3f &center, bool withCaps, int smoothLevel, const ATOM_Vector3f &axis, float radius, float height, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset = 0);
ATOMX_API void ATOMX_CALL ATOMX_CreateCone (const ATOM_Vector3f &center, bool withCap, int smoothLevel, const ATOM_Vector3f &axis, float radius, float height, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset = 0);
ATOMX_API void ATOMX_CALL ATOMX_CreateSphere (const ATOM_Vector3f &center, int numRings, int numSegments, float radius, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset = 0);
ATOMX_API void ATOMX_CALL ATOMX_CreateBox (const ATOM_Vector3f &center, const ATOM_Vector3f &size, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset = 0);
ATOMX_API void ATOMX_CALL ATOMX_CreateCircle (const ATOM_Vector3f &center, int smoothLevel, float radius, const ATOM_Vector3f &axis, ATOM_Vector3f *vertexbuffer, unsigned short *indexbuffer, unsigned *numVertices, unsigned *numIndices, unsigned short indexOffset = 0);

// computation
ATOMX_API float ATOMX_CALL ATOMX_MeasureScreenDistance (ATOM_Camera *camera, const ATOM_Vector3f &locationWorld, float distanceWorld);

// misc
class ATOMX_API ATOMX_ReferenceCookie
{
public:
	void insert (const char *filename);
	bool test (const char *filename);
private:
	ATOM_SET<ATOM_STRING> _filenameSet;
};

//! 保存文件名到一个字符串集合容器中
//! 此方法将文件名标准化并统一改为小写并存入fileList容器中
//! \param filename 文件名
//! \param fileList 要存入的容器
//! \note 文件名为VFS格式，因此在使用前需要初始化VFS文件系统，VFS文件名和物理文件名之间的转化可使用方法ATOM_GetNativePathName和ATOM_GetPhysicalPathName
ATOMX_API void ATOMX_CALL ATOMX_StoreFileName (const char *filename, ATOM_SET<ATOM_STRING> &fileList);

//! 获取某个文件所引用的其他文件列表并存入容器
//! 支持.ui, .nm2, .nm, .cps, .csp, .cp, .ccp, .3sg, .mat文件，此方法不会将filename存入容器
//! \param filename 文件名
//! \param nonAsyncOnly 如果filename是场景.3sg文件时，此参数为true则只获取非异步加载对象所引用的文件，否则获取所有引用文件
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileList (const char *filename, bool nonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个ui文件所引用的其他文件列表并存入容器
//! 此方法不会将uiFilename存入容器
//! \param uiFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfUI (const char *uiFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个.nm2文件所引用的其他文件列表并存入容器
//! 此方法不会将modelFilename存入容器
//! \param modelFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfNM2 (const char *modelFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个.nm文件所引用的其他文件列表并存入容器
//! 此方法不会将modelFilename存入容器
//! \param modelFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfNM (const char *modelFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个.cps文件所引用的其他文件列表并存入容器
//! 此方法不会将cpsFilename存入容器
//! \param cpsFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCPS (const char *cpsFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个.csp文件所引用的其他文件列表并存入容器
//! 此方法不会将cspFilename存入容器
//! \param cspFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCSP (const char *cspFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个cp文件所引用的其他文件列表并存入容器
//! 此方法不会将cpFilename存入容器
//! \param cpFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCP (const char *cpFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个ccp文件所引用的其他文件列表并存入容器
//! 此方法不会将ccpFilename存入容器
//! \param ccpFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCCP (const char *ccpFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个地形xml文件所引用的其他文件列表并存入容器
//! 此方法不会将terrainFilename存入容器
//! \param terrainFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfTerrain (const char *terrainFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个.3sg文件所引用的其他文件列表并存入容器
//! 此方法不会将scnFilename存入容器
//! \param scnFilename 文件名
//! \param nonAsyncOnly 此参数为true则只获取非异步加载对象所引用的文件，否则获取所有引用文件
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOf3SG (const char *scnFilename, bool NonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 获取某个.mat材质文件所引用的其他文件列表并存入容器
//! 此方法不会将matFilename存入容器
//! \param matFilename 文件名
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfMat (const char *matFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 遍历某个目录并保存其中的文件名到容器
//! \param directory 目录名
//! \param recursive 如果为true则对目录下的子目录做递归操作
//! \param addDepends 如果为true则检查被操作的每个文件所引用的文件并存入容器
//! \param nonAsyncOnly 如果被操作的文件为场景文件，此参数为true则只获取非异步加载对象所引用的文件，否则获取所有引用文件
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetFileListInDirectory (const char *directory, bool recursive, bool addDepends, bool nonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 将系统必备基本文件名列表存入容器
//! \param fileList 要存入的容器
//! \param cookie 文件名缓存对象，用于提高运行效率
//! \note 文件名为VFS格式
ATOMX_API void ATOMX_CALL ATOMX_GetPrerequiredFileList (ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! 增强的文件拷贝
//! 此方法允许在目标文件夹内创建源文件的目录结构
//! \param sourceFile 要拷贝的文件名
//! \param destDir 拷贝到哪个目录
//! \param createDir 是否在目录中创建源文件的目录结构
//! \return true成功　false失败
//! \note 文件名和目录名称需要为VFS绝对路径格式
ATOMX_API bool ATOMX_CALL ATOMX_CopyFile (const char *sourceFile, const char *destDir, bool createDir);

#endif // __ATOM3DX_H
