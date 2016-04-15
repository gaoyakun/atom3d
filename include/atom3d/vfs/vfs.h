#ifndef __ATOM_VFS_VFS_H
#define __ATOM_VFS_VFS_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "../ATOM_dbghlp.h"
#include "basedefs.h"

#if defined(_MSC_VER)
# pragma warning(push)
# pragma warning(disable:4251)
#endif

class ATOM_VFS_API ATOM_VFS
{
public:
  enum
  {
	binary = 0,
	read = (1<<0),
	write = (1<<1),
	append = (1<<2),
	text = (1<<3),
	trunc = (1<<4),
	max_filename_length = 512,

	begin = 0,
	current = 1,
	end = 2,

	file = 0,
	directory = 1
  };

  struct finddata_t
  {
    char filename[max_filename_length];
    char *filenameNoPath;
    int type;
    unsigned reserved;
  };

protected:
  struct filedesc_t
  {
    ATOM_VFS *vfs;
    ATOM_VFS *owner;
    char filename[max_filename_length];
  };

public:
  typedef unsigned handle;
  enum
  {
	invalid_handle = 0
  };

public:
  ATOM_VFS (void);
  virtual ~ATOM_VFS (void);

public:
  // �ļ����Ƿ����ִ�Сд
  virtual bool isCaseSensitive (void) const = 0;

  // �ļ����ָ���
  virtual char getSeperator (void) const = 0;

  // ��Ŀ¼�в��ҵ�һ���ļ�
  virtual ATOM_VFS::finddata_t *findFirst (const char *dir) = 0;

  // ��Ŀ¼�в�����һ���ļ�
  virtual bool findNext (ATOM_VFS::finddata_t *handle) = 0;

  // ֹͣ����
  virtual void findClose (ATOM_VFS::finddata_t *handle) = 0;

  // ���ĳ���ļ��Ƿ����
  virtual bool doesFileExists (const char *file) = 0;

  // ���ĳ���ļ��Ƿ�Ŀ¼
  virtual bool isDir (const char *file) = 0;

  // ����Ŀ¼
  virtual bool mkDir (const char *dir) = 0;

  // ɾ��һ���ļ���Ŀ¼
  virtual bool removeFile (const char *file, bool force) = 0;

  // ��ȡ��ǰĿ¼
  virtual void getCWD (char *buffer) const = 0;

  // �޸ĵ�ǰĿ¼
  virtual bool chDir (const char *dir) = 0;

  // ���ļ����·��ת��Ϊ��׼�ľ���·��
  virtual char *identifyPath (const char *dir, char *buffer) const = 0;

  // ������VFS�����ļ����Ļ���ȡ����ϵͳ�е����������ļ�·��
  virtual ATOM_VFS *getNativePath (const char *mount_path, char *buffer) const = 0;

  // ��һ���ļ�
  virtual ATOM_VFS::handle vfopen (const char *filename, int mode) = 0;

  // ��ȡ�ļ�
  virtual unsigned vfread (ATOM_VFS::handle f, void *buffer, unsigned size) = 0;

  // д���ļ�
  virtual unsigned vfwrite (ATOM_VFS::handle f, const void *buffer, unsigned size) = 0;

  // �ļ�ָ�붨λ
  virtual long vfseek (ATOM_VFS::handle f, long offset, int origin) = 0;

  // ��ȡ��ǰ�ļ�ָ��
  virtual long vftell (ATOM_VFS::handle f) = 0;

  // ��ȡĳ���ļ��Ĵ�С
  virtual long vfsize (ATOM_VFS::handle f) = 0;

  // �رմ򿪵��ļ�
  virtual void vfclose (ATOM_VFS::handle f) = 0;

  // �鿴�ļ�ָ���Ƿ񵽴��β
  virtual bool vfeof (ATOM_VFS::handle f) = 0;

private:
  // prevent instance duplication
  ATOM_VFS (const ATOM_VFS&);
  void operator = (const ATOM_VFS&);

protected:
  // ����FindData�ṹ
  virtual ATOM_VFS::finddata_t *_newFindData (void);
  // ɾ��FindData�ṹ
  virtual void _deleteFindData (ATOM_VFS::finddata_t *fd);
  virtual ATOM_VFS::finddata_t *_allocFindData (void);
  virtual void _freeFindData (ATOM_VFS::finddata_t *fd);
  virtual void _purgeFindDatas (void);

  // �����ļ������ṹ
  virtual ATOM_VFS::filedesc_t *_newFileDesc (void);
  // ɾ���ļ������ṹ
  virtual void _deleteFileDesc (ATOM_VFS::filedesc_t *fd);
  virtual ATOM_VFS::filedesc_t *_allocFileDesc (void);
  virtual void _freeFileDesc (ATOM_VFS::filedesc_t *fd);
  virtual void _purgeFileDescs (void);

public:
  // for internal use
  virtual int _strcmp (const char *s1, const char *s2) const;
  virtual int _strncmp (const char *s1, const char *s2, unsigned n) const;

public:
  static ATOM_VFS::handle ATOM_CALL vfsOpen (ATOM_VFS *vfs, const char *filename, int mode);
  static unsigned ATOM_CALL vfsRead (ATOM_VFS::handle f, void *buffer, unsigned size);
  static unsigned ATOM_CALL vfsWrite (ATOM_VFS::handle f, const void *buffer, unsigned size);
  static bool ATOM_CALL vfsEof (ATOM_VFS::handle f);
  static long ATOM_CALL vfsSeek (ATOM_VFS::handle f, long offset, int origin);
  static long ATOM_CALL vfsTell (ATOM_VFS::handle f);
  static long ATOM_CALL vfsSize (ATOM_VFS::handle f);
  static void ATOM_CALL vfsClose (ATOM_VFS::handle f);
  static int ATOM_CALL vfsCRC32 (ATOM_VFS::handle f);
  static const char* ATOM_CALL vfsGetFileName (ATOM_VFS::handle f);
  static ATOM_VFS* ATOM_CALL vfsGetVFS (ATOM_VFS::handle f);

private:
  typedef ATOM_VECTOR<ATOM_VFS::finddata_t*> finddatavec_t;
  unsigned _M_num_finddatas;
  finddatavec_t _M_finddatas;

  typedef ATOM_VECTOR<ATOM_VFS::filedesc_t*> filedescvec_t;
  unsigned _M_num_filedescs;
  filedescvec_t _M_filedescs;
};

#if defined(_MSC_VER)
# pragma warning(pop)
#endif

#endif // __ATOM_VFS_VFS_H
