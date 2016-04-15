#include "StdAfx.h"
#include "app.h"

static int testLZMA (void)
{
	const char *x = "&*s3dfdfa3234_(%^&*(_()*&*(&^dasfi2i342apfuap0o9821ffjal;sdkfjpoai8ro23ij4r23iup8fjasdjfaos8uwapoi3jr;qifuawp98";
	const unsigned len = strlen(x);
	unsigned lenCompressed = LZMAGetMaxCompressedSize (len);
	char *p = new char[lenCompressed];
	lenCompressed = LZMACompress(x, len, p, lenCompressed);

	char buffer[32];
	LZMADecompress(p, lenCompressed, buffer, 32);

	return buffer[0];
}

static void ATOM_CALL cr_01 (void *p)
{
	int i = 0;

	for (;;)
	{
		if (i > 5000)
		{
			break;
		}
		ATOM_LOGGER::log ("cr01: %d\n", i++);
		ATOM_Coroutine::yieldTo (NULL, 500);
	}
}

static void ATOM_CALL cr_02 (void *p)
{
	int i = 0;

	for (;;)
	{
		if (i > 5000)
		{
			break;
		}
		ATOM_LOGGER::log ("cr02: %d\n", i++);
		ATOM_Coroutine::yieldTo (NULL, 100);
	}
}

static void ATOM_CALL cr_03 (void *p)
{
	int i = 0;

	for (;;)
	{
		if (i > 5000)
		{
			break;
		}
		ATOM_LOGGER::log ("cr03: %d\n", i++);
		ATOM_Coroutine::yieldTo (NULL, 200);
	}
}

class WeakPtrTestClass
{
	ATOM_DECLARE_WEAK_REF
};

ATOM_JointTransformInfo genRandomJointTransform (void)
{
	ATOM_JointTransformInfo t;
	ATOM_Vector3f axis(ATOM_randomf(1.f, 2.f), ATOM_randomf(-2.f, 6.f), ATOM_randomf(-3.f, 3.f));
	axis.normalize();
	float angle = ATOM_randomf(0.f, 10.f);
	t.rotate.fromAxisAngle (angle, axis);
	t.scale.set (ATOM_randomf(1.f, 4.f), ATOM_randomf(-1.f, 1.f), ATOM_randomf(-2.f, 2.f), 1.f);
	t.translate.set (ATOM_randomf(1.f, 4.f), ATOM_randomf(-1.f, 1.f), ATOM_randomf(-2.f, 2.f), 1.f);
	return t;
}

void genRandomJointTransforms (ATOM_VECTOR<ATOM_JointTransformInfo> &v)
{
	for (int i =0; i < v.size(); ++i)
	{
		v[i] = genRandomJointTransform();
	}
}

void genRandomMatrix34 (ATOM_VECTOR<ATOM_Matrix3x4f> &v)
{
	for (int i =0; i < v.size(); ++i)
	{
		ATOM_JointTransformInfo info = genRandomJointTransform ();
		ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getIdentityMatrix();
		info.rotate.toMatrix (m);
		m.setRow3 (3, info.translate.x, info.translate.y, info.translate.z);
		m >>= ATOM_Matrix4x4f::getScaleMatrix (info.scale.getVector3());
		v[i] = m;
	}
}

void convertJointQuatToJointMat_ (ATOM_Matrix3x4f *mats, const ATOM_JointTransformInfo *quats, unsigned numJoints)
{
	for (int i = 0; i < numJoints; ++i)
	{
		const ATOM_JointTransformInfo &info = quats[i];
		ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getIdentityMatrix();
		info.rotate.toMatrix (m);
		m.setRow3 (3, info.translate.x, info.translate.y, info.translate.z);
		m >>= ATOM_Matrix4x4f::getScaleMatrix (info.scale.getVector3());
		mats[i] = m;
	}
}

void transformSkeleton_ (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint)
{
	for (int i = firstJoint; i <= lastJoint; ++i)
	{
		jointMats[i] <<= jointMats[parents[i]];
	}
}

static void test (void)
{
}

int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int nShowCmd)
{
	ATOM_ScriptManager sm;
	ATOM_Script *script = sm.createScript (nullptr);
	sm.destroyScript (script);

	test ();

#if 0
	WeakPtrTestClass *p = ATOM_NEW(WeakPtrTestClass);
	WeakPtrTestClass *p2 = ATOM_NEW(WeakPtrTestClass);

	ATOM_WeakPtrT<WeakPtrTestClass> ptr = p;
	ATOM_WeakPtrT<WeakPtrTestClass> ptr2(ptr);
	ATOM_WeakPtrT<WeakPtrTestClass> ptr3(p2);
	ptr2 = ptr3;
	ptr3 = ptr;

	ATOM_DELETE(p);
	ATOM_DELETE(p2);

	p = ptr.get();

	for (;;)
	{
		ATOM_DownloadContext *downloadContext = ATOM_NewDownload ("http://192.168.18.122/gamedata/ATOM_studio.exe", 128*1024);
		ATOM_StartDownloading (downloadContext, "D:\\DownloadTest.tmp");
		ATOM_WaitForDownloading (downloadContext, INFINITE);
		::DeleteFileA (ATOM_GetDownloadedFileName(downloadContext));
		ATOM_DestroyDownload (downloadContext);
	}

	{
		WSADATA wsa = { 0 };
		WSAStartup (MAKEWORD(2,2),&wsa);
		ATOM_DownloadContext *d = ATOM_NewDownload ("http://pocoproject.org/releases/poco-1.4.6/poco-1.4.6p1.zip");
		ATOM_StartDownloading (d, "F:\\poco-1.4.6p1.zip");
		ATOM_WaitForDownloading (d, INFINITE);
		ATOM_DestroyDownload (d);
	}
#endif

	ATOM_VECTOR<ATOM_STRING> l;
	l.push_back ("abcdxx@1");
	l.push_back ("acof@2");
	l.push_back ("abcdxx@3");
	l.push_back ("acof@4");
	l.push_back ("uifedf@2");
	l.push_back ("uifedf@4");
	l.push_back ("acof@8");
	l.push_back ("abcdxx@8");
	l.push_back ("abddef");
	std::sort (l.begin(), l.end());

	StudioApp app;
	app.setFrameInterval (1);

	//ATOM_Coroutine::createCoroutine (&cr_01, 0);
	//ATOM_Coroutine::createCoroutine (&cr_02, 0);
	//ATOM_Coroutine::createCoroutine (&cr_03, 0);

	//ATOM_AutoFile f("/lua.txt", ATOM_VFS::read|ATOM_VFS::text);
	//if (f)
	//{
	//	char *p = (char*)ATOM_MALLOC(f->size() + 1);
	//	unsigned sz = f->read (p, f->size());
	//	p[sz] = '\0';

	//	ATOM_ScriptManager mngr;
	//	ATOM_Script *script = mngr.createScript ("lua");
	//	script->setSource (p);

	//	for (;;)
	//	{
	//		script->resume ();
	//	}
	//}

	return app.run ();
}
