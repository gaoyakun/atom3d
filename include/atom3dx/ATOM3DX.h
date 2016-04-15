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

//! �����ļ�����һ���ַ�������������
//! �˷������ļ�����׼����ͳһ��ΪСд������fileList������
//! \param filename �ļ���
//! \param fileList Ҫ���������
//! \note �ļ���ΪVFS��ʽ�������ʹ��ǰ��Ҫ��ʼ��VFS�ļ�ϵͳ��VFS�ļ����������ļ���֮���ת����ʹ�÷���ATOM_GetNativePathName��ATOM_GetPhysicalPathName
ATOMX_API void ATOMX_CALL ATOMX_StoreFileName (const char *filename, ATOM_SET<ATOM_STRING> &fileList);

//! ��ȡĳ���ļ������õ������ļ��б���������
//! ֧��.ui, .nm2, .nm, .cps, .csp, .cp, .ccp, .3sg, .mat�ļ����˷������Ὣfilename��������
//! \param filename �ļ���
//! \param nonAsyncOnly ���filename�ǳ���.3sg�ļ�ʱ���˲���Ϊtrue��ֻ��ȡ���첽���ض��������õ��ļ��������ȡ���������ļ�
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileList (const char *filename, bool nonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��ui�ļ������õ������ļ��б���������
//! �˷������ὫuiFilename��������
//! \param uiFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfUI (const char *uiFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��.nm2�ļ������õ������ļ��б���������
//! �˷������ὫmodelFilename��������
//! \param modelFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfNM2 (const char *modelFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��.nm�ļ������õ������ļ��б���������
//! �˷������ὫmodelFilename��������
//! \param modelFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfNM (const char *modelFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��.cps�ļ������õ������ļ��б���������
//! �˷������ὫcpsFilename��������
//! \param cpsFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCPS (const char *cpsFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��.csp�ļ������õ������ļ��б���������
//! �˷������ὫcspFilename��������
//! \param cspFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCSP (const char *cspFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��cp�ļ������õ������ļ��б���������
//! �˷������ὫcpFilename��������
//! \param cpFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCP (const char *cpFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��ccp�ļ������õ������ļ��б���������
//! �˷������ὫccpFilename��������
//! \param ccpFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfCCP (const char *ccpFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ������xml�ļ������õ������ļ��б���������
//! �˷������ὫterrainFilename��������
//! \param terrainFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfTerrain (const char *terrainFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��.3sg�ļ������õ������ļ��б���������
//! �˷������ὫscnFilename��������
//! \param scnFilename �ļ���
//! \param nonAsyncOnly �˲���Ϊtrue��ֻ��ȡ���첽���ض��������õ��ļ��������ȡ���������ļ�
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOf3SG (const char *scnFilename, bool NonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ȡĳ��.mat�����ļ������õ������ļ��б���������
//! �˷������ὫmatFilename��������
//! \param matFilename �ļ���
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetReferencedFileListOfMat (const char *matFilename, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ����ĳ��Ŀ¼���������е��ļ���������
//! \param directory Ŀ¼��
//! \param recursive ���Ϊtrue���Ŀ¼�µ���Ŀ¼���ݹ����
//! \param addDepends ���Ϊtrue���鱻������ÿ���ļ������õ��ļ�����������
//! \param nonAsyncOnly ������������ļ�Ϊ�����ļ����˲���Ϊtrue��ֻ��ȡ���첽���ض��������õ��ļ��������ȡ���������ļ�
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetFileListInDirectory (const char *directory, bool recursive, bool addDepends, bool nonAsyncOnly, ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ϵͳ�ر������ļ����б��������
//! \param fileList Ҫ���������
//! \param cookie �ļ���������������������Ч��
//! \note �ļ���ΪVFS��ʽ
ATOMX_API void ATOMX_CALL ATOMX_GetPrerequiredFileList (ATOM_SET<ATOM_STRING> &fileList, ATOMX_ReferenceCookie *cookie = 0);

//! ��ǿ���ļ�����
//! �˷���������Ŀ���ļ����ڴ���Դ�ļ���Ŀ¼�ṹ
//! \param sourceFile Ҫ�������ļ���
//! \param destDir �������ĸ�Ŀ¼
//! \param createDir �Ƿ���Ŀ¼�д���Դ�ļ���Ŀ¼�ṹ
//! \return true�ɹ���falseʧ��
//! \note �ļ�����Ŀ¼������ҪΪVFS����·����ʽ
ATOMX_API bool ATOMX_CALL ATOMX_CopyFile (const char *sourceFile, const char *destDir, bool createDir);

#endif // __ATOM3DX_H
