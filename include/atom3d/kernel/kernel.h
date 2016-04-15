/**	\file kernel.h
 *	核心全局函数声明.
 *
 *	\author 高雅昆
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

//! 定义插件库
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

//! 定义一个插件类
#define ATOM_PLUGIN_DEF(ns, classname)  \
  { ns##_plugin_impl::get_cf(ns##_plugin_impl::wrapper<classname>()), \
    ns##_plugin_impl::get_df(ns##_plugin_impl::wrapper<classname>()), \
    ns##_plugin_impl::get_pf(ns##_plugin_impl::wrapper<classname>()), \
    ns##_plugin_impl::get_si(ns##_plugin_impl::wrapper<classname>()), \
	classname::_classname(),  \
    0 \
  },

//! 结束定义插件库
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

//! 获取ATOM3D版本
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetVersion (void);

//! 释放一个插件对象实例.
//! 通过减少插件对象指针的引用计数来释放一个对象，如果引用为0则会删除该对象
//! \param object 要释放的插件对象
ATOM_KERNEL_API void ATOM_CALL ATOM_ReleaseObject(ATOM_Object* object);

//! 创建一个插件对象
//! \param classname 对象的类名
//! \param objectname 对象的实例名称
//! \return 创建的对象，失败会返回NULL
//! \note 如果该类插件未加载或者已经有了同名的该类对象，创建失败
ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_CreateObject(const char* classname, const char* objectname);


ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LoadObject(const char *filename, const char* objectname);

ATOM_KERNEL_API bool ATOM_SaveObject(ATOM_Object *object, const char *filename);

//! 查找一个命名的插件对象
//! \param classname 对象的类名
//! \param objectname 对象的实例名称
//! \return 对象实例，未查到返回NULL
ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LookupObject(const char *classname, const char* objectname);

//! 查找一个命名的插件对象，如果未查到则创建该对象
//! \param classname 对象的类名
//! \param objectname 对象的实例名称
//! \param createNew 如果是新创建的对象，该值返回true
//! \return 对象实例，未查到返回NULL
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

//! 打印一个对象的信息到文件
//! \param filename 文件名，如果该文件不存在将会打印到标准输出
ATOM_KERNEL_API void ATOM_CALL ATOM_DumpObjects(const char *filename);

//! 开始对象实例查找
ATOM_KERNEL_API void * ATOM_CALL ATOM_BeginFindObject (void);

//! 查找下一个对象实例
ATOM_KERNEL_API bool ATOM_CALL ATOM_FindNextObject (void *handle);

//! 结束查找
ATOM_KERNEL_API void ATOM_CALL ATOM_EndFindObject (void *handle);

//! 获取找到的对象指针
ATOM_KERNEL_API ATOM_Object *ATOM_CALL ATOM_GetObjectFound (void *handle);

//! 获取一类对象的数量
//! \param classname 对象类名
//! \return 该类对象的数量
ATOM_KERNEL_API unsigned     ATOM_CALL ATOM_GetObjectCount(const char *classname);

//! 创建当前对象工厂内所有对象状态的快照
//! 这个函数主要是为了调试用
//! \return 新创建快照的索引
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_CreateFactorySnapshot(void);

//! 打印两个对象工厂快照的不同之处
//! 这个函数主要是为了调试用
//! \param snapshot1 快照1的索引
//! \param snapshot2 快照2的索引
ATOM_KERNEL_API void ATOM_CALL ATOM_DumpSnapshotDifference(unsigned snapshot1, unsigned snapshot2);

//! 注册一个插件类型
//! \param classname 类型名称
//! \param cf 对象创建函数指针
//! \param df 对象删除函数指针
//! \param pf 对象彻底删除函数指针
//! \param si 对象脚本接口指针
//! \param cache 是否缓冲对象(该参数已作废未使用)
ATOM_KERNEL_API void ATOM_CALL ATOM_RegisterType(const char* classname,ATOM_Object *(*cf) (void),void (*df) (ATOM_Object*), void (*pf) (ATOM_Object*), ATOM_ScriptInterfaceBase *si, bool cache);

//! 注销一个插件类型
//! \param classname 要注销的类型名称
ATOM_KERNEL_API void ATOM_CALL ATOM_UnregisterType(const char* classname);

//! 查询某个字符串是否是合法的插件类型名称
//! \param classname 要查询的字符串
//! \return true 是合法类型 false 不是合法类型
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsObjectType(const char* classname);

//! 从文件载入一个资源对象
//! 该函数会先查看该对象是否已经载入过，如果没载入过就会尝试创建并加载该对象，否则会返回已存在的现有对象指针
//! \param classname 对象类型名
//! \param path 资源文件路径
//! \param binary true表示2进制文件，false表示文本文件
//! \param userdata 可以传递给载入函数一个自定义数据
//! \return 载入的对象指针
ATOM_KERNEL_API ATOM_AUTOREF(ATOM_Object) ATOM_CALL ATOM_LoadResource(const char* classname, const char* path, bool binary, void *userdata = 0);

//! 查找一个插件类型的属性结构
//! \param classname 类型名称
//! \param attrib 要查找的属性名称
//! \return 属性结构指针
ATOM_KERNEL_API ATOM_AttribAccessorBase*  ATOM_CALL ATOM_FindAttrib(const char *classname, const char *attrib);

//! 加载一个虚拟文件系统
//! \param vfs 要加载的文件系统
//! \param isPhysical 是否物理文件系统
//! \return true 成功 false 失败
ATOM_KERNEL_API bool ATOM_CALL ATOM_AddVFS (ATOM_VFS *vfs, bool isPhysical);

//! 从物理文件名称转换为虚拟文件名称
//! \param path 物理文件名
//! \param vfsPath 虚拟文件名
//! \return 返回该文件的虚拟文件系统
ATOM_KERNEL_API ATOM_VFS *   ATOM_CALL ATOM_GetVFSPathName(const char *path, char *vfsPath);

//! 从虚拟文件名称转换为系统物理文件名称
//! \param path 虚拟文件名
//! \param nativePath 物理文件名
//! \return 返回该文件的虚拟文件系统
ATOM_KERNEL_API ATOM_VFS *   ATOM_CALL ATOM_GetNativePathName(const char *path, char *nativePath);

//! 强制转换一个虚拟文件名称到物理文件名称
//! \param path 虚拟文件名
//! \param physicalPath 物理文件名
//! \return 返回该文件的虚拟文件系统
ATOM_KERNEL_API ATOM_VFS *   ATOM_CALL ATOM_GetPhysicalPathName(const char *path, char *physicalPath);

//! 在一个虚拟目录下查找第一个文件
//! \param dir 要查找的目录
//! \return 文件查找结构，未查到文件返回NULL
ATOM_KERNEL_API ATOM_VFS::finddata_t * ATOM_CALL ATOM_FindFirst(const char* dir);

//! 查找下一个文件
//! \param handle 文件查找结构
//! \return true 查到下一个 false 没查到下一个
//! \sa ATOM_FindFirst
//! \sa ATOM_CloseFind
ATOM_KERNEL_API bool ATOM_CALL ATOM_FindNext(ATOM_VFS::finddata_t *handle);

//! 结束查找
//! \param handle 文件查找结构
ATOM_KERNEL_API void ATOM_CALL ATOM_CloseFind(ATOM_VFS::finddata_t *handle);

//! 检测某个虚拟文件是否存在
//! \param path 虚拟文件名
//! \return true 存在 false 不存在
ATOM_KERNEL_API bool ATOM_CALL ATOM_FileExists(const char* path);

//! 检测某个虚拟目录是否存在
//! \param path 虚拟目录名
//! \return true 存在 false 不存在
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsDirectory(const char* path);

//! 创建一个虚拟目录
//! \param path 虚拟目录名
//! \return true 成功 false 不成功
ATOM_KERNEL_API bool ATOM_CALL ATOM_CreateDirectory(const char* path);

//! 删除一个虚拟文件或目录
//! \param path 虚拟文件名
//! \return true 成功 false 不成功
ATOM_KERNEL_API bool ATOM_CALL ATOM_Remove(const char* path);

//! 获取虚拟文件系统中的当前工作路径
//! \param buffer 当前工作路径将保存在这个缓冲区中
ATOM_KERNEL_API void ATOM_CALL ATOM_GetCWD(char* buffer);

//! 设置虚拟文件系统中的当前工作路径
//! \param path 要设置当前工作路径
ATOM_KERNEL_API bool ATOM_CALL ATOM_SetCWD(const char* path);

//! 规范化一个虚拟文件路径
//! \param path 虚拟文件路径
//! \param buffer 存放规范化以后的虚拟文件路径
ATOM_KERNEL_API bool ATOM_CALL ATOM_CompletePath(const char* path, char* buffer);

//! 打开一个虚拟文件
//! \param path 虚拟文件名
//! \param mode 打开模式
//! \return 文件指针，打开失败返回NULL
ATOM_KERNEL_API ATOM_File *ATOM_CALL ATOM_OpenFile(const char* path, int mode);

//! 检查一个文件是否存在
//! \param path 虚拟文件名
//! \return true存在 false不存在
ATOM_KERNEL_API bool ATOM_CALL ATOM_CheckFileExistence(const char *path);

//! 保存已打开
ATOM_KERNEL_API void ATOM_CALL ATOM_SaveOpenedFileList(const char* path);
ATOM_KERNEL_API void ATOM_CALL ATOM_ClearOpenedFileList(const char* path);
ATOM_KERNEL_API void ATOM_CALL ATOM_SetFileRecordID(const char *id);
ATOM_KERNEL_API int ATOM_CALL ATOM_CopyOpenedFileList(const char *path, const char *id, bool createIdDirectory);

//! 关闭一个虚拟文件
//! \param file 虚拟文件
ATOM_KERNEL_API void ATOM_CALL ATOM_CloseFile(ATOM_File* file);

//! 获取一个插件类型的属性数量
//! \param classname 插件类型名称
//! \return 属性数量
ATOM_KERNEL_API unsigned     ATOM_CALL ATOM_GetAttribCount (const char *classname);

//! 获取一个插件类型的某个属性名称
//! \param classname 插件类型名称
//! \param index 属性索引
//! \return 属性名称
ATOM_KERNEL_API const char*  ATOM_CALL ATOM_GetAttribName (const char *classname, unsigned index);

//! 获取一个插件类型的某个属性类型
//! \param classname 插件类型名称
//! \param attrib 属性名称
//! \return 属性类型
ATOM_KERNEL_API int          ATOM_CALL ATOM_GetAttribType (const char *classname, const char *attrib);

//! 获取一个插件类型的某个属性类型描述
//! \param classname 插件类型名称
//! \param attrib 属性名称
//! \return 属性类型描述
ATOM_KERNEL_API const char*  ATOM_CALL ATOM_GetAttribTypeDesc (const char *classname, const char *attrib);

//! 获取一个插件类型的某个属性注释
//! \param classname 插件类型名称
//! \param attrib 属性名称
//! \return 属性注释
ATOM_KERNEL_API const char*  ATOM_CALL ATOM_GetAttribComment (const char *classname, const char *attrib);

//! 查询一个插件类型的某个属性是否只读属性
//! \param classname 插件类型名称
//! \param attrib 属性名称
//! \return true 是只读属性 false 不是只读属性
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribReadonly (const char *classname, const char *attrib);

//! 查询一个插件类型的某个属性是否可以存档
//! \param classname 插件类型名称
//! \param attrib 属性名称
//! \return true 可以 false 不可以
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribPersistent (const char *classname, const char *attrib);

//! 查询一个插件类型的某个属性是否拥有默认值
//! \param classname 插件类型名称
//! \param attrib 属性名称
//! \return true 有 false 没有
ATOM_KERNEL_API bool ATOM_CALL ATOM_IsAttribHasDefaultValue (const char *classname, const char *attrib);

//! 查询一个插件类型的某个属性的默认值
//! \param classname 插件类型名称
//! \param attrib 属性名称
//! \param value 保存获取的默认值
//! \return true 成功 false 不成功
ATOM_KERNEL_API bool ATOM_CALL ATOM_GetDefaultAttribValue (const char *classname, const char *attrib, ATOM_ScriptVar &value);

//! 设置一个插件对象的某个属性
//! \param object 插件对象
//! \param attrib 属性名称
//! \param value 要设置的值
//! \return true 成功 false 不成功
ATOM_KERNEL_API bool ATOM_CALL ATOM_SetAttribValue (ATOM_Object *object, const char *attrib, const ATOM_ScriptVar &value);

//! 获取一个插件对象的某个属性
//! \param object 插件对象
//! \param attrib 属性名称
//! \param value 保存获得的属性值
//! \return true 成功 false 不成功
ATOM_KERNEL_API bool ATOM_CALL ATOM_GetAttribValue (ATOM_Object *object, const char *attrib, ATOM_ScriptVar &value);

//! 设置环境变量
//! \param var 变量名
//! \param value 变量值
ATOM_KERNEL_API void ATOM_CALL ATOM_PutEnv(const char* var, const char* value);

//! 获取环境变量
//! \param var 变量名
//! \param buffer 保存值的缓冲区
//! \param size 缓冲区的字节数
//! \return 写入缓冲区的字节数
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetEnv(const char* var, char* buffer, unsigned size);

//! 压缩一个数据块
//! \param input 输入的数据块
//! \param len 数据块长度
//! \param output 输出缓冲区
//! \param level 压缩质量
//! \return 压缩后的字节数
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_CompressBuffer(const void *input, unsigned len, void *output, unsigned level = 5);

//! 解压缩一个数据块
//! \param input 输入的数据块
//! \param len 数据块长度
//! \param output 输出缓冲区
//! \return 解压缩后的字节数
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_DecompressBuffer(const void *input, unsigned len, void *output);

//! 得到当前的系统计时
//! \return 计时，以毫秒为单位
ATOM_KERNEL_API unsigned ATOM_CALL ATOM_GetTick();			//From process started;

typedef ATOM_VFS * (ATOM_CALL *VFSLoadCallback) (const char *filename, const char *password, void *userData);
//! 设置VFS载入回调
ATOM_KERNEL_API void ATOM_SetVFSLoadCallback (VFSLoadCallback callback, void *userdata);

//! 载入VFS
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
