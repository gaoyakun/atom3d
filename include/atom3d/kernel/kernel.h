/**	\file kernel.h
 *	����ȫ�ֺ�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_KERNEL_H
#define __ATOM_KERNEL_KERNEL_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_basedefs.h"
#include "../ATOM_dbghlp.h"
#include "../ATOM_utils.h"
#include "../ATOM_math.h"
#include "../ATOM_vfs.h"
#include "basedefs.h"
#include "object.h"
#include "fileiterator.h"

#define ATOM_DUMP_SORT_NONE       0
#define ATOM_DUMP_SORT_NAME       1
#define ATOM_DUMP_SORT_CLASSNAME  2

#define ATOM_MAKE_VERSION(major, minor, micro) ((((major) & 0xFF)<<24)+(((minor) & 0xFF)<<16)+(micro & 0xFFFF))

#if 1

#define ATOM_DECLARE_PLUGINS(ns) void ATOM_CALL registerPlugins##ns (void);
#define ATOM_PLUGIN_BEGIN(ns, pluginname, major, minor, micro)  void ATOM_CALL registerPlugins##ns (void) {
#define ATOM_PLUGIN_DEF(ns, classname) ATOM_RegisterType (classname::_classname(), classname::_create, classname::_destroy, classname::_purge, classname::_get_script_interface(), 0);
#define ATOM_PLUGIN_END }

#else

#define ATOM_DECLARE_PLUGINS(ns)

//! ��������
#define ATOM_PLUGIN_BEGIN(ns, pluginname, major, minor, micro) \
  class ns##_plugin_impl  \
  { \
  public: \
    template <class Type> struct wrapper {}; \
    template <class Type> \
    static inline void *get_cf (const wrapper<Type>&) { return Type::_create; } \
    template <class Type> \
    static inline void *get_df (const wrapper<Type>&) { return Type::_destroy; }  \
    template <class Type> \
    static inline void *get_pf (const wrapper<Type>&) { return Type::_purge; }  \
    template <class Type> \
	  static inline void *get_si (const wrapper<Type>&) { return Type::_get_script_interface(); } \
  };  \
  static const char *_plugin_name = pluginname;  \
  static unsigned _plugin_version = ATOM_MAKE_VERSION(major, minor, micro); \
  static struct { \
    void *creation_func;  \
    void *deletion_func;  \
    void *purge_func; \
    void *script_interface; \
    const char *name;  \
    int cache; \
  } classinfos[] = {

//! ����һ�������
#define ATOM_PLUGIN_DEF(ns, classname)  \
  { ns##_plugin_impl::get_cf(ns##_plugin_impl::wrapper<classname>()), \
    ns##_plugin_impl::get_df(ns##_plugin_impl::wrapper<classname>()), \
    ns##_plugin_impl::get_pf(ns##_plugin_impl::wrapper<classname>()), \
    ns##_plugin_impl::get_si(ns##_plugin_impl::wrapper<classname>()), \
	classname::_classname(),  \
    0 \
  },

//! ������������
#define ATOM_PLUGIN_END \
  };  \
  extern"C" ATOM_EXPORT unsigned ATOM_CALL ATOM_GetPluginVersion(void) { return _plugin_version; } \
  extern"C" ATOM_EXPORT const char* ATOM_CALL ATOM_GetPluginDescription(void) { return _plugin_name; } \
  extern"C" ATOM_EXPORT unsigned ATOM_CALL ATOM_GetPluginCount(void) { return sizeof(classinfos)/sizeof(classinfos[0]); }  \
  extern"C" ATOM_EXPORT void* ATOM_CALL ATOM_GetPluginClassInfo(unsigned i) { return &classinfos[i]; }

#endif // ATOM3D_BUILD_DLL

#if (_MSC_VER <= 1300)
# define ATOM_AUTOPTR(classname) ATOM_AutoRef<classname, ATOM_NewDeletePolicy<classname> >
# define ATOM_AUTOREF(classname) ATOM_AutoRef<classname, ATOM_ObjectPolicy<classname> > 
# define ATOM_HARDREF(classname) ATOM_AutoRef<classname, ATOM_ObjectHardPolicy<classname> >
# define ATOM_SOFTREF(classname) ATOM_AutoRef<classname, ATOM_ObjectSoftPolicy<classname> >
#else
# define ATOM_AUTOPTR(classname) ATOM_AutoRef<classname, ATOM_NewDeletePolicy>
# define ATOM_AUTOREF(classname) ATOM_AutoRef<classname, ATOM_ObjectPolicy> 
# define ATOM_HARDREF(classname) ATOM_AutoRef<classname, ATOM_ObjectHardPolicy> 
# define ATOM_SOFTREF(classname) ATOM_AutoRef<classname, ATOM_ObjectSoftPolicy> 
#endif

#define ATOM_PARAM(type, index) (*((type*)ATOM_GetScriptParam(index+1)))
#define ATOM_RESULT(type) (*((type*)ATOM_GetScriptParam(0)))
#define ATOM_ATTRIB(type,buf) (*((type*)(buf)))

template <class Type>
struct ATOM_ObjectPolicy;

//! ��ȡATOM3D�汾
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetVersion (void);

//! �ͷ�һ���������ʵ��.
//! ͨ�����ٲ������ָ������ü������ͷ�һ�������������Ϊ0���ɾ���ö���
//! \param object Ҫ�ͷŵĲ������
ATOM_KERNEL_API void ATOM_CALL ATOM_ReleaseObject(ATOM_Object* object);

//! ����һ���������
//! \param classname ���������
//! \param objectname �����ʵ������
//! \return �����Ķ���ʧ�ܻ᷵��NULL
//! \note ���������δ���ػ����Ѿ�����ͬ���ĸ�����󣬴���ʧ��
ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_CreateObject(const char* classname, const char* objectname);


ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LoadObject(const char *filename, const char* objectname);

ATOM_KERNEL_API bool ATOM_SaveObject(ATOM_Object *object, const char *filename);

//! ����һ�������Ĳ������
//! \param classname ���������
//! \param objectname �����ʵ������
//! \return ����ʵ����δ�鵽����NULL
ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LookupObject(const char *classname, const char* objectname);

//! ����һ�������Ĳ���������δ�鵽�򴴽��ö���
//! \param classname ���������
//! \param objectname �����ʵ������
//! \param createNew ������´����Ķ��󣬸�ֵ����true
//! \return ����ʵ����δ�鵽����NULL
ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LookupOrCreateObject(const char *classname, const char* objectname, bool *createNew = 0);

template <class Type>
struct ATOM_ObjectPolicy
{
  Type *init () const 
  {
    return 0;
  }

  void acquire(Type* p) const 
  {
    p->addRef();
  }

  void release(Type* p) const 
  {
      ATOM_ReleaseObject(p);
  }
};

// Policy to delete ATOM_Object family objects
template <class Type>
struct ATOM_NewDeletePolicy
{
  Type *init () const 
  {
    return 0;
  }

  void acquire(Type* p) const 
  {
    p->addRef();
  }

  void release(Type* p) const 
  {
    if (p && !p->decRef ())
	{
	  ATOM_DELETE(p);
	}
  }
};

template <class Type>
struct ATOM_ObjectHardPolicy
{
  Type *init () const 
  {
    return init (0, 0);
  }

  Type *init (const char *classname, const char *objectname) const 
  {
    if (!classname)
      classname = Type::_classname();

    ATOM_AUTOREF(Type) p = !objectname ? ATOM_CreateObject(classname, 0) : ATOM_LookupOrCreateObject(classname, objectname);

    if (p.get()) acquire (p.get());

    return p.get();
  }

  void acquire(Type* p) const 
  {
    p->addRef();
  }

  void release(Type* p) const 
  {
      ATOM_ReleaseObject(p);
  }
};

// Policy to delete ATOM_Object family objects
template <class Type>
struct ATOM_ObjectSoftPolicy
{
  Type *init () const 
  {
    return 0;
  }

  Type *init (const char *classname, const char *objectname) const 
  {
    if (!classname)
      classname = Type::_classname();

    ATOM_AUTOREF(Type) p = ATOM_LookupObject(classname, objectname);

    if (p.get()) acquire (p.get());

    return p.get();
  }

  void acquire(Type* p) const 
  {
    p->addRef();
  }

  void release(Type* p) const 
  {
      ATOM_ReleaseObject(p);
  }
};


class ATOM_KERNEL_API ATOM_File;
class ATOM_KERNEL_API ATOM_ScriptVar;

struct ATOM_FunctionCallerBase;
struct ATOM_AttribAccessorBase;
struct ATOM_VariableAccessorBase;

ATOM_KERNEL_API void ATOM_CALL ATOM_GetCommandLine (ATOM_VECTOR<ATOM_STRING> &args);

//! ��ӡһ���������Ϣ���ļ�
//! \param filename �ļ�����������ļ������ڽ����ӡ����׼���
ATOM_KERNEL_API void ATOM_CALL ATOM_DumpObjects(const char *filename);

//! ��ʼ����ʵ������
ATOM_KERNEL_API void * ATOM_CALL ATOM_BeginFindObject (void);

//! ������һ������ʵ��
ATOM_KERNEL_API bool ATOM_CALL ATOM_FindNextObject (void *handle);

//! ��������
ATOM_KERNEL_API void ATOM_CALL ATOM_EndFindObject (void *handle);

//! ��ȡ�ҵ��Ķ���ָ��
ATOM_KERNEL_API ATOM_Object *ATOM_CALL ATOM_GetObjectFound (void *handle);

//! ��ȡһ����������
//! \param classname ��������
//! \return ������������
ATOM_KERNEL_API unsigned     ATOM_CALL ATOM_GetObjectCount(const char *classname);

//! ������ǰ���󹤳������ж���״̬�Ŀ���
//! ���������Ҫ��Ϊ�˵�����
//! \return �´������յ�����
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_CreateFactorySnapshot(void);

//! ��ӡ�������󹤳����յĲ�֮ͬ��
//! ���������Ҫ��Ϊ�˵�����
//! \param snapshot1 ����1������
//! \param snapshot2 ����2������
ATOM_KERNEL_API void ATOM_CALL ATOM_DumpSnapshotDifference(unsigned snapshot1, unsigned snapshot2);

//! ע��һ���������
//! \param classname ��������
//! \param cf ���󴴽�����ָ��
//! \param df ����ɾ������ָ��
//! \param pf ���󳹵�ɾ������ָ��
//! \param si ����ű��ӿ�ָ��
//! \param cache �Ƿ񻺳����(�ò���������δʹ��)
ATOM_KERNEL_API void ATOM_CALL ATOM_RegisterType(const char* classname,ATOM_Object *(*cf) (void),void (*df) (ATOM_Object*), void (*pf) (ATOM_Object*), ATOM_ScriptInterfaceBase *si, bool cache);

//! ע��һ���������
//! \param classname Ҫע������������
ATOM_KERNEL_API void ATOM_CALL ATOM_UnregisterType(const char* classname);

//! ��ѯĳ���ַ����Ƿ��ǺϷ��Ĳ����������
//! \param classname Ҫ��ѯ���ַ���
//! \return true �ǺϷ����� false ���ǺϷ�����
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsObjectType(const char* classname);

//! ���ļ�����һ����Դ����
//! �ú������Ȳ鿴�ö����Ƿ��Ѿ�����������û������ͻ᳢�Դ��������ظö��󣬷���᷵���Ѵ��ڵ����ж���ָ��
//! \param classname ����������
//! \param path ��Դ�ļ�·��
//! \param binary true��ʾ2�����ļ���false��ʾ�ı��ļ�
//! \param userdata ���Դ��ݸ����뺯��һ���Զ�������
//! \return ����Ķ���ָ��
ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LoadResource(const char* classname, const char* path, bool binary, void *userdata = 0);

//! ����һ��������͵����Խṹ
//! \param classname ��������
//! \param attrib Ҫ���ҵ���������
//! \return ���Խṹָ��
ATOM_KERNEL_API ATOM_AttribAccessorBase*  ATOM_CALL ATOM_FindAttrib(const char *classname, const char *attrib);

//! ����һ�������ļ�ϵͳ
//! \param vfs Ҫ���ص��ļ�ϵͳ
//! \param isPhysical �Ƿ������ļ�ϵͳ
//! \return true �ɹ� false ʧ��
ATOM_KERNEL_API bool ATOM_CALL ATOM_AddVFS (ATOM_VFS *vfs, bool isPhysical);

//! �������ļ�����ת��Ϊ�����ļ�����
//! \param path �����ļ���
//! \param vfsPath �����ļ���
//! \return ���ظ��ļ��������ļ�ϵͳ
ATOM_KERNEL_API ATOM_VFS *   ATOM_CALL ATOM_GetVFSPathName(const char *path, char *vfsPath);

//! �������ļ�����ת��Ϊϵͳ�����ļ�����
//! \param path �����ļ���
//! \param nativePath �����ļ���
//! \return ���ظ��ļ��������ļ�ϵͳ
ATOM_KERNEL_API ATOM_VFS *   ATOM_CALL ATOM_GetNativePathName(const char *path, char *nativePath);

//! ǿ��ת��һ�������ļ����Ƶ������ļ�����
//! \param path �����ļ���
//! \param physicalPath �����ļ���
//! \return ���ظ��ļ��������ļ�ϵͳ
ATOM_KERNEL_API ATOM_VFS *   ATOM_CALL ATOM_GetPhysicalPathName(const char *path, char *physicalPath);

//! ��һ������Ŀ¼�²��ҵ�һ���ļ�
//! \param dir Ҫ���ҵ�Ŀ¼
//! \return �ļ����ҽṹ��δ�鵽�ļ�����NULL
ATOM_KERNEL_API ATOM_VFS::finddata_t * ATOM_CALL ATOM_FindFirst(const char* dir);

//! ������һ���ļ�
//! \param handle �ļ����ҽṹ
//! \return true �鵽��һ�� false û�鵽��һ��
//! \sa ATOM_FindFirst
//! \sa ATOM_CloseFind
ATOM_KERNEL_API bool ATOM_CALL ATOM_FindNext(ATOM_VFS::finddata_t *handle);

//! ��������
//! \param handle �ļ����ҽṹ
ATOM_KERNEL_API void ATOM_CALL ATOM_CloseFind(ATOM_VFS::finddata_t *handle);

//! ���ĳ�������ļ��Ƿ����
//! \param path �����ļ���
//! \return true ���� false ������
ATOM_KERNEL_API bool ATOM_CALL ATOM_FileExists(const char* path);

//! ���ĳ������Ŀ¼�Ƿ����
//! \param path ����Ŀ¼��
//! \return true ���� false ������
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsDirectory(const char* path);

//! ����һ������Ŀ¼
//! \param path ����Ŀ¼��
//! \return true �ɹ� false ���ɹ�
ATOM_KERNEL_API bool ATOM_CALL ATOM_CreateDirectory(const char* path);

//! ɾ��һ�������ļ���Ŀ¼
//! \param path �����ļ���
//! \return true �ɹ� false ���ɹ�
ATOM_KERNEL_API bool ATOM_CALL ATOM_Remove(const char* path);

//! ��ȡ�����ļ�ϵͳ�еĵ�ǰ����·��
//! \param buffer ��ǰ����·���������������������
ATOM_KERNEL_API void ATOM_CALL ATOM_GetCWD(char* buffer);

//! ���������ļ�ϵͳ�еĵ�ǰ����·��
//! \param path Ҫ���õ�ǰ����·��
ATOM_KERNEL_API bool ATOM_CALL ATOM_SetCWD(const char* path);

//! �淶��һ�������ļ�·��
//! \param path �����ļ�·��
//! \param buffer ��Ź淶���Ժ�������ļ�·��
ATOM_KERNEL_API bool ATOM_CALL ATOM_CompletePath(const char* path, char* buffer);

//! ��һ�������ļ�
//! \param path �����ļ���
//! \param mode ��ģʽ
//! \return �ļ�ָ�룬��ʧ�ܷ���NULL
ATOM_KERNEL_API ATOM_File *ATOM_CALL ATOM_OpenFile(const char* path, int mode);

//! ���һ���ļ��Ƿ����
//! \param path �����ļ���
//! \return true���� false������
ATOM_KERNEL_API bool ATOM_CALL ATOM_CheckFileExistence(const char *path);

//! �����Ѵ�
ATOM_KERNEL_API void ATOM_CALL ATOM_SaveOpenedFileList(const char* path);
ATOM_KERNEL_API void ATOM_CALL ATOM_ClearOpenedFileList(const char* path);
ATOM_KERNEL_API void ATOM_CALL ATOM_SetFileRecordID(const char *id);
ATOM_KERNEL_API int ATOM_CALL ATOM_CopyOpenedFileList(const char *path, const char *id, bool createIdDirectory);

//! �ر�һ�������ļ�
//! \param file �����ļ�
ATOM_KERNEL_API void ATOM_CALL ATOM_CloseFile(ATOM_File* file);

//! ��ȡһ��������͵���������
//! \param classname �����������
//! \return ��������
ATOM_KERNEL_API unsigned     ATOM_CALL ATOM_GetAttribCount (const char *classname);

//! ��ȡһ��������͵�ĳ����������
//! \param classname �����������
//! \param index ��������
//! \return ��������
ATOM_KERNEL_API const char*  ATOM_CALL ATOM_GetAttribName (const char *classname, unsigned index);

//! ��ȡһ��������͵�ĳ����������
//! \param classname �����������
//! \param attrib ��������
//! \return ��������
ATOM_KERNEL_API int          ATOM_CALL ATOM_GetAttribType (const char *classname, const char *attrib);

//! ��ȡһ��������͵�ĳ��������������
//! \param classname �����������
//! \param attrib ��������
//! \return ������������
ATOM_KERNEL_API const char*  ATOM_CALL ATOM_GetAttribTypeDesc (const char *classname, const char *attrib);

//! ��ȡһ��������͵�ĳ������ע��
//! \param classname �����������
//! \param attrib ��������
//! \return ����ע��
ATOM_KERNEL_API const char*  ATOM_CALL ATOM_GetAttribComment (const char *classname, const char *attrib);

//! ��ѯһ��������͵�ĳ�������Ƿ�ֻ������
//! \param classname �����������
//! \param attrib ��������
//! \return true ��ֻ������ false ����ֻ������
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribReadonly (const char *classname, const char *attrib);

//! ��ѯһ��������͵�ĳ�������Ƿ���Դ浵
//! \param classname �����������
//! \param attrib ��������
//! \return true ���� false ������
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribPersistent (const char *classname, const char *attrib);

//! ��ѯһ��������͵�ĳ�������Ƿ�ӵ��Ĭ��ֵ
//! \param classname �����������
//! \param attrib ��������
//! \return true �� false û��
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribHasDefaultValue (const char *classname, const char *attrib);

//! ��ѯһ��������͵�ĳ�����Ե�Ĭ��ֵ
//! \param classname �����������
//! \param attrib ��������
//! \param value �����ȡ��Ĭ��ֵ
//! \return true �ɹ� false ���ɹ�
ATOM_KERNEL_API bool ATOM_CALL ATOM_GetDefaultAttribValue (const char *classname, const char *attrib, ATOM_ScriptVar &value);

//! ����һ����������ĳ������
//! \param object �������
//! \param attrib ��������
//! \param value Ҫ���õ�ֵ
//! \return true �ɹ� false ���ɹ�
ATOM_KERNEL_API bool ATOM_CALL ATOM_SetAttribValue (ATOM_Object *object, const char *attrib, const ATOM_ScriptVar &value);

//! ��ȡһ����������ĳ������
//! \param object �������
//! \param attrib ��������
//! \param value �����õ�����ֵ
//! \return true �ɹ� false ���ɹ�
ATOM_KERNEL_API bool ATOM_CALL ATOM_GetAttribValue (ATOM_Object *object, const char *attrib, ATOM_ScriptVar &value);

//! ���û�������
//! \param var ������
//! \param value ����ֵ
ATOM_KERNEL_API void ATOM_CALL ATOM_PutEnv(const char* var, const char* value);

//! ��ȡ��������
//! \param var ������
//! \param buffer ����ֵ�Ļ�����
//! \param size ���������ֽ���
//! \return д�뻺�������ֽ���
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetEnv(const char* var, char* buffer, unsigned size);

//! ѹ��һ�����ݿ�
//! \param input ��������ݿ�
//! \param len ���ݿ鳤��
//! \param output ���������
//! \param level ѹ������
//! \return ѹ������ֽ���
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_CompressBuffer(const void *input, unsigned len, void *output, unsigned level = 5);

//! ��ѹ��һ�����ݿ�
//! \param input ��������ݿ�
//! \param len ���ݿ鳤��
//! \param output ���������
//! \return ��ѹ������ֽ���
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_DecompressBuffer(const void *input, unsigned len, void *output);

//! �õ���ǰ��ϵͳ��ʱ
//! \return ��ʱ���Ժ���Ϊ��λ
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetTick();			//From process started;

typedef ATOM_VFS * (ATOM_CALL *VFSLoadCallback) (const char *filename, const char *password, void *userData);
//! ����VFS����ص�
ATOM_KERNEL_API void ATOM_SetVFSLoadCallback (VFSLoadCallback callback, void *userdata);

//! ����VFS
ATOM_KERNEL_API ATOM_VFS * ATOM_CALL ATOM_LoadVFS (const char *filename, const char *password);

//! LZMA max compressed size
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_LZMAGetMaxCompressedSize (unsigned srcLen);

//! LZMA compress
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_LZMACompress (const void *src, unsigned srcLen, void *dest, unsigned destLen);

//! LZMA decompress
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_LZMADecompress (const void *src, unsigned srcLen, void *dest, unsigned destLen);

//! Convert ansi string to unicode string
ATOM_KERNEL_API ATOM_WSTRING ATOM_CALL ATOM_Ansi2Wide (const char *str, size_t len);

//! Convert unicode string to ansi string
ATOM_KERNEL_API ATOM_STRING ATOM_CALL ATOM_Wide2Ansi (const wchar_t *str, size_t len);

//! Calculate CRC32
ATOM_KERNEL_API int ATOM_CALL ATOM_CalcCRC32 (int lastCRC32, const void *data, unsigned size);

#endif // __ATOM_KERNEL_KERNEL_H
/*! @} */
