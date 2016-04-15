#ifndef __ATOM_SCRIPT_LUASCRIPT_H
#define __ATOM_SCRIPT_LUASCRIPT_H

extern"C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "script.h"
#include "funcbind.h"
#include "scriptinterface.h"

class ATOM_LuaScript: public ATOM_Script
  {
  public:
    typedef ATOM_HASHMAP<ATOM_STRING, ATOM_FunctionStruct*> ConstantMap;
    typedef ConstantMap::iterator ConstantMapIter;
    typedef ConstantMap::const_iterator ConstantMapConstIter;
    typedef ATOM_HASHMAP<ATOM_STRING, ConstantMap> NamespaceMap;
    typedef NamespaceMap::iterator NamespaceMapIter;
    typedef NamespaceMap::const_iterator NamespaceMapConstIter;

  public:
    friend class ATOM_LuaScriptFactory;
    struct ReferencedPtr_t {
      int refcount;
      void *instance;
      ATOM_InstanceOp *instance_op;
      ATOM_BaseCreator *creator;
      ATOM_DereferenceFunc deref;
    };
    struct UserData_t { 
      ReferencedPtr_t *instance; 
    }; // Store a full lua userdata, so we can have a metatable.

  protected:
    virtual void registerUserType (ATOM_ClassStruct *type);
    virtual void registerUserFunction (ATOM_FunctionStruct *func);
    virtual void registerUserConstant (ATOM_FunctionStruct *func);

  public:
    ATOM_LuaScript (void);
	ATOM_LuaScript (lua_State *master);
    virtual ~ATOM_LuaScript ();

  public:
    lua_State *getLuaState () const;

  public:
    virtual int getParameterType (int index);
    virtual int getParameteri (int index);
    virtual unsigned int getParameterui (int index);
    virtual float getParameterf (int index);
    virtual bool getParameterb (int index);
    virtual const char *getParameters (int index);
    virtual void *getParameterp (int index);
    virtual void *getParameterc (int index);
    virtual ATOM_ScriptVariant getParameterv (int index);
    virtual int setReturni (int ret);
    virtual int setReturnui (unsigned int ret);
    virtual int setReturnf (float ret);
    virtual int setReturnb (bool ret);
    virtual int setReturns (const char *ret);
    virtual int setReturnp (void *ret, ATOM_DereferenceFunc deref);
    virtual int setReturnc (void *ret, const char *classname, ATOM_DereferenceFunc deref);
    virtual int setReturnpi (void *ret, ATOM_InstanceOp *op);
    virtual int setReturnci (void *ret, const char *classname, ATOM_InstanceOp *op);
    virtual int setReturnv (const ATOM_ScriptVariant &var, ATOM_DereferenceFunc deref);
	virtual bool isFunction (const char *name);

  public:
    virtual bool setSource (const char *src, bool lib);
    virtual void execute (void);
    virtual bool executeString (const char *str);
	virtual int yeild (void);
    virtual bool resume (void);
    virtual int getStatus (void) const;
	virtual ATOM_Script *clone (void) const;

  protected:
    virtual void setGlobaliImp (const char *name, int value);
	virtual void setGlobalfImp (const char *name, float value);
	virtual void setGlobalsImp (const char *name, const char *value);
	virtual void setGlobalpImp (const char *name, void *value);

  private:
    void registerUserConstants (void);
    void pushParameter (const ATOM_ScriptVariant &var);
    void getResult (ATOM_ScriptVariant &var);

  private:
    int _M_status;
    lua_State *_M_state;
    bool _M_const_dirty;
	bool _isThread;
	int _threadRef;
    NamespaceMap _M_constants;
  };

// inline member functions
inline lua_State *ATOM_LuaScript::getLuaState () const {
  return _M_state;
}

#endif // __ATOM_SCRIPT_LUASCRIPT_H
