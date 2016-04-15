#ifndef __ATOM_SCRIPT_SCRIPT_H
#define __ATOM_SCRIPT_SCRIPT_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <string>
#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "var.h"

struct ATOM_InstanceOp;
struct ATOM_DynamicFunction;
struct ATOM_ClassStruct;
struct ATOM_FunctionStruct;
class ATOM_ScriptFactory;

class ATOM_SCRIPT_API ATOM_Script
{
public:
	enum {
		LOADED    = 0,
		LOADERR   = 1,
		YIELD     = 2,
		FINISHED  = 3
	};

	enum {
		TYPE_NUMBER,
		TYPE_BOOL,
		TYPE_STRING,
		TYPE_POINTER,
		TYPE_NIL,
		TYPE_UNKNOWN
	};

	typedef void (*OutputFunc) (const char *str, bool error);

public:
	ATOM_Script (const char *vendor): _M_vendor(vendor), _M_output_func(0) {}
	virtual ~ATOM_Script (void);

public:
	const char *getVendor (void) const;
	const char *getSource (void) const;
	void setFunction (const char *funcname);
	void setNumParameters (unsigned num);
	void setNumResults (unsigned num);
	void setParameter (int index, const ATOM_ScriptVariant &param);
	const ATOM_ScriptVariant &getResult (int index);

public:
	void registerTypes (ATOM_ClassStruct *types, unsigned num);
	void registerFunctions (ATOM_FunctionStruct *funcs, unsigned num);
	void registerConstants (ATOM_FunctionStruct *constants, unsigned num);
	void setErrorOutputCallback (OutputFunc callback);
	void setGlobali (const char *name, int value);
	void setGlobalf (const char *name, float value);
	void setGlobals (const char *name, const char *value);
	void setGlobalp (const char *name, void *value);
	OutputFunc getErrorOutputCallback (void) const;

protected:
	virtual void registerUserType (ATOM_ClassStruct *type) = 0;
	virtual void registerUserFunction (ATOM_FunctionStruct *func) = 0;
	virtual void registerUserConstant (ATOM_FunctionStruct *func) = 0;
	virtual void setGlobaliImp (const char *name, int value) = 0;
	virtual void setGlobalfImp (const char *name, float value) = 0;
	virtual void setGlobalsImp (const char *name, const char *value) = 0;
	virtual void setGlobalpImp (const char *name, void *value) = 0;

public:

	virtual int getParameterType (int index) = 0;
	virtual int getParameteri (int index) = 0;
	virtual unsigned int getParameterui (int index) = 0;
	virtual float getParameterf (int index) = 0;
	virtual bool getParameterb (int index) = 0;
	virtual const char *getParameters (int index) = 0;
	virtual void *getParameterp (int index) = 0;
	virtual void *getParameterc (int index) = 0;
	virtual ATOM_ScriptVariant getParameterv (int index) = 0;
	virtual int setReturni (int ret) = 0;
	virtual int setReturnui (unsigned int ret) = 0;
	virtual int setReturnf (float ret) = 0;
	virtual int setReturnb (bool ret) = 0;
	virtual int setReturns (const char *ret) = 0;
	virtual int setReturnp (void *ret, ATOM_DereferenceFunc deref) = 0;
	virtual int setReturnc (void *ret, const char *classname, ATOM_DereferenceFunc deref) = 0;
	virtual int setReturnpi (void *ret, ATOM_InstanceOp *op) = 0;
	virtual int setReturnci (void *ret, const char *classname, ATOM_InstanceOp *op) = 0;
	virtual int setReturnv (const ATOM_ScriptVariant &var, ATOM_DereferenceFunc deref) = 0;
	virtual bool isFunction (const char *name) = 0;

public:
	virtual bool setSource (const char *src, bool lib) = 0;
	virtual void execute (void) = 0;
	virtual bool executeString (const char *str) = 0;
	virtual int yeild (void) = 0;
	virtual bool resume (void) = 0;
	virtual int getStatus (void) const = 0;
	virtual ATOM_Script *clone (void) const = 0;

public:
	virtual ATOM_DynamicFunction *allocDynamicFunction (const char *function, const ATOM_Script *scp, void *callfunc, void *dst);

protected:
	typedef ATOM_VECTOR<ATOM_ClassStruct*> ClassVector;
	typedef ClassVector::iterator ClassVectorIter;
	typedef ClassVector::const_iterator ClassVectorConstIter;
	ClassVector _M_classstructs;

private:
	void updateClassStructDependencies ();

protected:
	ATOM_STRING _M_vendor;
	ATOM_STRING _M_source;
	ATOM_STRING _M_function_name;
	unsigned _M_stack_top;
	OutputFunc _M_output_func;
	ATOM_VECTOR<ATOM_ScriptVariant> _M_parameters;
	ATOM_VECTOR<ATOM_ScriptVariant> _M_results;
	typedef ATOM_HASHMAP<ATOM_STRING, ATOM_DynamicFunction*> FunctionMap;
	typedef FunctionMap::iterator FunctionMapIter;
	typedef FunctionMap::const_iterator FunctionMapConstIter;
	typedef ATOM_HASHMAP<const ATOM_Script*, FunctionMap> ScriptFunctionMap;
	typedef ScriptFunctionMap::iterator ScriptFunctionMapIter;
	typedef ScriptFunctionMap::const_iterator ScriptFunctionMapConstIter;
	ScriptFunctionMap _M_dynamic_funcs;
};

// inline member functions
inline const char *ATOM_Script::getVendor () const {
	return _M_vendor.c_str();
}

inline const char *ATOM_Script::getSource (void) const {
	return _M_source.c_str();
}

inline void ATOM_Script::setFunction (const char *funcname) {
	_M_function_name = funcname ? funcname : "";
}

inline void ATOM_Script::setNumParameters (unsigned num) {
	_M_parameters.resize (num);
}

inline void ATOM_Script::setNumResults (unsigned num) {
	_M_results.resize (num);
}

inline void ATOM_Script::setParameter (int index, const ATOM_ScriptVariant &param) {
	_M_parameters[index] = param;
}

inline const ATOM_ScriptVariant &ATOM_Script::getResult (int index) {
	return _M_results[index];
}

inline void ATOM_Script::setErrorOutputCallback (OutputFunc func) {
	_M_output_func = func;
}

inline ATOM_Script::OutputFunc ATOM_Script::getErrorOutputCallback (void) const {
	return _M_output_func;
}

#endif // __ATOM_SCRIPT_SCRIPT_H

