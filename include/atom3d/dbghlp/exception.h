#ifndef __ATOM_DBGHLP_EXCEPTION_H
#define __ATOM_DBGHLP_EXCEPTION_H

#include "basedefs.h"

class ATOM_DBGHLP_API ATOM_Exception
{
public:
	ATOM_Exception (void);
	ATOM_Exception (const char *desc);
	virtual ~ATOM_Exception (void);

public:
	const char *getDesc (void) const;
	const char *getStackTrace (void) const;

protected:
	const char *_desc;
	const char *_stacktrace;
};

class ATOM_DBGHLP_API ATOM_OutOfMemory: public ATOM_Exception
{
public:
	ATOM_OutOfMemory (void);
};

class ATOM_DBGHLP_API ATOM_AbnormalTermination: public ATOM_Exception
{
public:
	ATOM_AbnormalTermination (void);
};

class ATOM_DBGHLP_API ATOM_InvalidCRTParameter: public ATOM_Exception
{
public:
	ATOM_InvalidCRTParameter (const wchar_t *expression, const wchar_t *function, const wchar_t *file, unsigned line);
};

class ATOM_DBGHLP_API ATOM_PureVirtualFunctionCall: public ATOM_Exception
{
public:
	ATOM_PureVirtualFunctionCall (void);
};

class ATOM_DBGHLP_API ATOM_Win32Exception: public ATOM_Exception
{
public:
	ATOM_Win32Exception (int code, PEXCEPTION_POINTERS exceptionPoints);
};

#endif // __ATOM_KERNEL_EXCEPTION_H
