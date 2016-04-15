#ifndef __ATOM_DEBUGHELP_ASSERTION_H__
#define __ATOM_DEBUGHELP_ASSERTION_H__

#if _MSC_VER > 1000
# pragma once
#endif

#include <stdio.h>
#include <stdarg.h>
#include "basedefs.h"

#pragma warning(disable:4800)

class ATOM_CallStack;

#if ATOM3D_MINIMAL_BUILD
	class ATOM_AssertionReporter;
#else
	class ATOM_DBGHLP_API ATOM_AssertionReporter
	{
	public:
		ATOM_AssertionReporter (void);
		virtual ~ATOM_AssertionReporter (void);
		virtual int report (int counter, const char *msg, const char *filename, int lineno, unsigned mask, const char *title, const char *files);
	};
#endif

#define ATOM_ASSERT_CONTINUE  0
#define ATOM_ASSERT_BREAK     1
#define ATOM_ASSERT_TERMINATE 2
#define ATOM_ASSERT_CONTINUE_MASK (1<<ATOM_ASSERT_CONTINUE)
#define ATOM_ASSERT_BREAK_MASK (1<<ATOM_ASSERT_BREAK)
#define ATOM_ASSERT_TERMINATE_MASK (1<<ATOM_ASSERT_TERMINATE)
#define ATOM_ASSERT_ALL_MASK 0xFFFFFFFF

struct ATOM_DBGHLP_API ATOM_Assertion
{
	const char *_filename;
	int _lineno;
	bool _expr;
	unsigned _mask;
	const char *_title;
	const char *_files;
	ATOM_AssertionReporter *_reporter;
	ATOM_Assertion (const char *filename, int lineno, bool expr, ATOM_AssertionReporter *reporter = 0, unsigned mask = ATOM_ASSERT_ALL_MASK, const char *title = 0, const char *files = 0);
	void operator () ();
	void operator () (const char *format, ...);
	void doAssertion (const char *msg);
};

extern"C" ATOM_DBGHLP_API int ATOM_ShowCallstackDialog (void *parentWnd, const char *caption, const char *filename, int lineno, const ATOM_CallStack *callstack, const char *desc, int skip, unsigned mask = ATOM_ASSERT_ALL_MASK, const char *files = 0);
extern"C" ATOM_DBGHLP_API void ATOM_SendErrorFiles (const char *errorFiles);
extern"C" ATOM_DBGHLP_API void *ATOM_SetDialogParentWindow (void *hWnd);
extern"C" ATOM_DBGHLP_API ATOM_AssertionReporter *ATOM_SetAssertionReporter (ATOM_AssertionReporter *reporter);

#define ATOM_VERIFY(EXPR, REPORTER) ATOM_Assertion(__FILE__, __LINE__, EXPR, REPORTER)
#define ATOM_VERIFY_EX(EXPR, REPORTER, MASK, TITLE, FILES) ATOM_Assertion(__FILE__, __LINE__, EXPR, REPORTER, MASK, TITLE, FILES)

#if !defined(NDEBUG)||defined(ATOM_ALWAYS_ASSERT)
#  define ATOM_ASSERT(EXPR) ATOM_VERIFY(EXPR, NULL)()
#  define ATOM_ASSERT_EX(EXPR, MASK, TITLE, FILES) ATOM_VERIFY_EX(EXPR, NULL, MASK, TITLE, FILES)()
#  define ATOM_ASSERT_MSG(EXPR) ATOM_VERIFY(EXPR, NULL)
#  define ATOM_ASSERT_MSG_EX(EXPR, MASK, TITLE, FILES) ATOM_VERIFY_EX(EXPR, NULL, MASK, TITLE, FILES)
#else
#  define ATOM_ASSERT
#  define ATOM_ASSERT_EX
#  define ATOM_ASSERT_MSG
#  define ATOM_ASSERT_MSG_EX
#endif

#endif // __ATOM_DEBUGHELP_ASSERTION_H__
