#ifndef __ATOM_DEBUGHELP_MINIDUMP_H__
#define __ATOM_DEBUGHELP_MINIDUMP_H__

#include "basedefs.h"

#define MDT_NORMAL 0x00000000
#define MDT_WITHDATASEGS 0x00000001
#define MDT_WITHFULLMEMORY 0x00000002
#define MDT_WITHHANDLEDATA 0x00000004
#define MDT_FILTERMEMORY 0x00000008
#define MDT_SCANMEMORY 0x00000010
#define MDT_WITHUNLOADEDMODULES 0x00000020
#define MDT_WITHINDIRECTLYREFERENCEDMEMORY 0x00000040
#define MDT_FILTERMODULEPATHS 0x00000080
#define MDT_WITHPROCESSTHREADDATA 0x00000100
#define MDT_WITHPRIVATEREADWRITEMEMORY 0x00000200
#define MDT_WITHOUTOPTIONALDATA 0x00000400
#define MDT_WITHFULLMEMORYINFO 0x00000800
#define MDT_WITHTHREADINFO 0x00001000
#define MDT_WITHCODESEGS 0x00002000

class ATOM_MiniDumpUserStreams
{
public:
  struct UserStreamInfo
  {
    unsigned long type;
    unsigned buffersize;
    void *buffer;
  };
public:
  virtual ~ATOM_MiniDumpUserStreams (void) {}
  virtual unsigned GetNumStreams (void) const = 0;
  virtual void GetStream (unsigned index, UserStreamInfo *streaminfo) = 0;
};

class ATOM_DBGHLP_API ATOM_MiniDumpWriter
{
public:
	ATOM_MiniDumpWriter (const char *filename = 0, bool fulldump = false);
	ATOM_MiniDumpWriter (int versionNumber, bool writeVersionNumber, const char *filename = 0, bool fulldump = false);
	virtual ~ATOM_MiniDumpWriter (void);

public:
	virtual void setVersionNumber (int version);
	virtual int getVersionNumber (void) const;

	virtual void setWriteVersion (bool writeVersion);
	virtual bool getWriteVersion (void) const;

	virtual void setFileName (const char *filename);
	virtual const char *getFileName (void) const;

	virtual bool snapshot (void);
	virtual void setFullDump (bool b);
	virtual bool getFullDump (void) const;

	virtual void addComment (int id, const char *comment);
	virtual const std::vector<std::pair<int, std::string> > *getComments (void) const;

protected:
	virtual bool beginWriteDump (void);
	virtual void endWriteDump (bool succeeded);
	virtual bool beginSnapShot (void);
	virtual void endSnapShot (bool succeeded);

public:
	static LONG WINAPI dumpHandler (_EXCEPTION_POINTERS *exceptionInfo);

private:
	char _filename[512];
	int _version;
	bool _writeVersion;
	bool _fulldump;
	std::vector<std::pair<int, std::string> > _comments;
};

ATOM_DBGHLP_API bool ATOM_WriteMiniDump (const char *filename, ATOM_MiniDumpUserStreams *userstreams = NULL, EXCEPTION_POINTERS *pExp = NULL, unsigned type = MDT_NORMAL);
ATOM_DBGHLP_API bool ATOM_WriteMiniDump (const char *filename, int processId = 0, ATOM_MiniDumpUserStreams *userstreams = 0, unsigned type = 0);
ATOM_DBGHLP_API bool ATOM_WriteMiniDump (const char *filename, const char *processName = 0, ATOM_MiniDumpUserStreams *userstreams = 0, unsigned type = 0);
ATOM_DBGHLP_API bool ATOM_ReadMiniDumpUserStream (const char *filename, ATOM_MiniDumpUserStreams::UserStreamInfo *info);

#endif // __ATOM_DEBUGHELP_MINIDUMP_H__
