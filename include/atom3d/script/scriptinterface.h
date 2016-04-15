#ifndef __ATOM_SCRIPT_SCRIPTINTERFACE_H
#define __ATOM_SCRIPT_SCRIPTINTERFACE_H

#if _MSC_VER > 1000
# pragma once
#endif

#include <stdexcept>
#include "../ATOM_kernel.h"
#include "basedefs.h"
#include "funcbind.h"

#define ATOM_SCRIPT_ARRAY_SIZE(array) (sizeof(array)/sizeof((array)[0]))

template <class T>
struct ATOM_CopyOp
{
  static T copy (const T *p) {
    return *p;
  }
};

struct ATOM_FunctionStruct
{
  ATOM_FunctionCallerBase *function;
  ~ATOM_FunctionStruct () { ATOM_DELETE(function); }
};

struct ATOM_BaseCreator
{
  virtual void *create (void) = 0;
  virtual void release (void *) = 0;
};

struct ATOM_NullCreator: public ATOM_BaseCreator
{
  virtual void *create (void) { return 0; }
  virtual void release (void*) {}
};

template <class T>
struct ATOM_DereferenceCreator: public ATOM_BaseCreator
{
  virtual void *create (void) { throw std::runtime_error("ATOM_DereferenceCreator is used for dereference only"); }
  virtual void release (void *p) { T().release(p); }
};

struct ATOM_NullCreatorHelper
{
  void *create (void) { return 0; }
  void release (void *) {}
};

template <class T, bool IsObject>
struct ATOM_ObjectCreatorHelperT
{
  void *create (void) { 
    ATOM_AUTOREF(T) p = ATOM_CreateObject(T::_classname(), 0);
    if (p)
    {
      p->addRef();
    }
    return p.get();
  }

  void release (void *p) {
    if (p)
    {
      ATOM_ReleaseObject (static_cast<T*>(p));
    }
  }    
};

template <class T, bool IsObject>
struct NullATOM_ObjectCreatorHelperT: public ATOM_ObjectCreatorHelperT<T, IsObject>
{
};

template <class T>
struct ATOM_ObjectCreatorHelperT<T, false>
{
  void *create (void) {
    T *p = ATOM_NEW(T);
    return p;
  }

  void release (void *p) {
    ATOM_DELETE(static_cast<T*>(p));
  }
};

template <class T>
struct NullATOM_ObjectCreatorHelperT<T, false>: public ATOM_NullCreatorHelper
{
};

template <class T, bool IsObject>
struct ATOM_PureObjectCreatorHelperT
{
  void release (void *p) {
    if (p)
    {
      ATOM_ReleaseObject (static_cast<T*>(p));
    }
  }    
};

template <class T>
struct ATOM_PureObjectCreatorHelperT<T, false>
{
  void release (void *p) {
  }
};

template <class T, bool ATOM_IsAbstract>
struct ATOM_ObjectCreatorHelperT2: public ATOM_BaseCreator
{
  static const bool IsObject = ATOM_IsBaseAndDerived<ATOM_Object, T>::result || ATOM_IsSame<ATOM_Object, T>::result;
  typedef ATOM_ObjectCreatorHelperT<T, IsObject> helper;

  virtual void *create (void) {
    return helper().create ();
  }

  virtual void release (void *p) {
    helper().release(p);
  }
};

template <class T, bool ATOM_IsAbstract>
struct ATOM_NullObjectCreatorHelperT2: public ATOM_BaseCreator
{
  static const bool IsObject = ATOM_IsBaseAndDerived<ATOM_Object, T>::result || ATOM_IsSame<ATOM_Object, T>::result;
  typedef NullATOM_ObjectCreatorHelperT<T, IsObject> helper;

  virtual void *create (void) {
    return helper().create ();
  }

  virtual void release (void *p) {
    helper().release(p);
  }
};

template <class T>
struct ATOM_ObjectCreatorHelperT2<T, true>: public ATOM_BaseCreator
{
  static const bool IsObject = ATOM_IsBaseAndDerived<ATOM_Object, T>::result || ATOM_IsSame<ATOM_Object, T>::result;
  typedef ATOM_PureObjectCreatorHelperT<T, IsObject> helper;

  virtual void *create (void) {
    throw std::runtime_error("Couldn't instantiate abstract class");
  }

  virtual void release (void *p) {
    helper().release(p);
  }
};

template <class T>
struct ATOM_NullObjectCreatorHelperT2<T, true>: public ATOM_ObjectCreatorHelperT2<T, true>
{
};

template <class T>
struct ATOM_ObjectCreator: public ATOM_ObjectCreatorHelperT2<T, ATOM_IsAbstract<T>::result>
{
};

template <class T>
struct ATOM_NullObjectCreator: public ATOM_NullObjectCreatorHelperT2<T, ATOM_IsAbstract<T>::result>
{
};

template <class T, bool ATOM_IsAbstract>
struct ATOM_ReturnCiImpl
{
  int operator () (ATOM_Script *scp, const T &ret, const char *name, ATOM_DereferenceFunc deref) {
    ATOM_InstanceOpTCopyable<T> op;
    return scp->setReturnci (const_cast<T*>(&ret), name, &op);
  }
};

template <class T>
struct ATOM_ReturnCiImpl<T, true>
{
  int operator () (ATOM_Script *scp, const T &ret, const char *name, ATOM_DereferenceFunc deref) {
    throw std::runtime_error("Couldn't instantiate abstract class"); \
  }
};

template <class From, class To>
struct ATOM_CastPointer
{
  static void *cast (void *from) {
    return static_cast<To*>((From*)from);
  }
};

template <class T, class W, W T::*F>
W ATOM_RawReadFunc(T *p)
{
  return p->*F;
}

template <class T, class W, W T::*F>
void ATOM_RawWriteFunc(T *p, W v)
{
  p->*F = v;
}

struct ATOM_CommandStruct
{
    ATOM_CommandCallerBase *command;
    ATOM_CommandStruct (): command(0) {}
    ATOM_CommandStruct (ATOM_CommandCallerBase *_cmd): command(_cmd) {}
    ~ATOM_CommandStruct () { ATOM_DELETE(command); }
private:
  ATOM_CommandStruct (const ATOM_CommandStruct&);
  void operator = (const ATOM_CommandStruct&);
};

struct ATOM_IndexCommandStruct
{
    ATOM_IndexCommandCallerBase *command;
    ATOM_IndexCommandStruct (): command(0) {}
    ATOM_IndexCommandStruct (ATOM_IndexCommandCallerBase *_cmd): command(_cmd) {}
    ~ATOM_IndexCommandStruct () { ATOM_DELETE(command); }
private:
  ATOM_IndexCommandStruct (const ATOM_IndexCommandStruct&);
  void operator = (const ATOM_IndexCommandStruct&);
};

struct ATOM_NewIndexCommandStruct
{
    ATOM_NewIndexCommandCallerBase *command;
    ATOM_NewIndexCommandStruct (): command(0) {}
    ATOM_NewIndexCommandStruct (ATOM_NewIndexCommandCallerBase *_cmd): command(_cmd) {}
    ~ATOM_NewIndexCommandStruct () { ATOM_DELETE(command); }
private:
  ATOM_NewIndexCommandStruct (const ATOM_NewIndexCommandStruct&);
  void operator = (const ATOM_NewIndexCommandStruct&);
};

struct ATOM_CommandList
{
  ATOM_VECTOR<ATOM_CommandStruct*> commands;
  ATOM_VECTOR<ATOM_FunctionStruct*> staticcommands;
  ATOM_VECTOR<ATOM_FunctionStruct*> constants;
  ATOM_CommandStruct *namecommand;
  ATOM_IndexCommandStruct *indexcommand;
  ATOM_NewIndexCommandStruct *newindexcommand;
  ATOM_CommandStruct *addfunction;
  ATOM_CommandStruct *subfunction;
  ATOM_CommandStruct *mulfunction;
  ATOM_CommandStruct *divfunction;
  ATOM_CommandStruct *negfunction;
  ATOM_CommandStruct *concatfunction;
  ATOM_CommandStruct *lenfunction;
  ATOM_CommandStruct *eqfunction;
  ATOM_CommandStruct *ltfunction;
  ATOM_CommandStruct *callfunction;
  ATOM_FunctionStruct *constructfunction;
  ATOM_CommandList *addCommand (ATOM_CommandCallerBase *cmd) {
    commands.push_back (ATOM_NEW(ATOM_CommandStruct, cmd));
    return this;
  }
  ATOM_CommandList *setConstructor (ATOM_FunctionCallerBase *func) {
    constructfunction = ATOM_NEW(ATOM_FunctionStruct);
    constructfunction->function = func;
    return this;
  }
  ATOM_CommandList *addStaticCommand (ATOM_FunctionCallerBase *func) {
    staticcommands.push_back (ATOM_NEW(ATOM_FunctionStruct));
    staticcommands.back()->function = func;
    return this;
  }
  ATOM_CommandList *addConstant (ATOM_FunctionCallerBase *func) {
    constants.push_back (ATOM_NEW(ATOM_FunctionStruct));
    constants.back()->function = func;
    return this;
  }
  ATOM_CommandList *addNameCommand (ATOM_CommandCallerBase *cmd) {
    namecommand = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addIndexCommand (ATOM_IndexCommandCallerBase *cmd) {
    indexcommand = ATOM_NEW(ATOM_IndexCommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addNewIndexCommand (ATOM_NewIndexCommandCallerBase *cmd) {
    newindexcommand = ATOM_NEW(ATOM_NewIndexCommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addAddFunction (ATOM_CommandCallerBase *cmd) {
    addfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addSubFunction (ATOM_CommandCallerBase *cmd) {
    subfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addMulFunction (ATOM_CommandCallerBase *cmd) {
    mulfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addDivFunction (ATOM_CommandCallerBase *cmd) {
    divfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addNegFunction (ATOM_CommandCallerBase *cmd) {
    negfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addConcatFunction (ATOM_CommandCallerBase *cmd) {
    concatfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addLenFunction (ATOM_CommandCallerBase *cmd) {
    lenfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addEqFunction (ATOM_CommandCallerBase *cmd) {
    eqfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addLtFunction (ATOM_CommandCallerBase *cmd) {
    ltfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList *addCallFunction (ATOM_CommandCallerBase *cmd) {
    callfunction = ATOM_NEW(ATOM_CommandStruct, cmd);
    return this;
  }
  ATOM_CommandList () {
    namecommand = 0;
    indexcommand = 0;
    newindexcommand = 0;
    addfunction = 0;
    subfunction = 0;
    mulfunction = 0;
    divfunction = 0;
    negfunction = 0;
    concatfunction = 0;
    lenfunction = 0;
    eqfunction = 0;
    ltfunction = 0;
    callfunction = 0;
    constructfunction = 0;
  }
  ~ATOM_CommandList () {
    for (unsigned i = 0; i < commands.size(); ++i)
    {
      ATOM_DELETE(commands[i]);
    }
    for (unsigned i = 0; i < staticcommands.size(); ++i)
    {
      ATOM_DELETE(staticcommands[i]);
    }
    for (unsigned i = 0; i < constants.size(); ++i)
    {
      ATOM_DELETE(constants[i]);
    }
    ATOM_DELETE(namecommand);
    ATOM_DELETE(indexcommand);
    ATOM_DELETE(newindexcommand);
    ATOM_DELETE(addfunction);
    ATOM_DELETE(subfunction);
    ATOM_DELETE(mulfunction);
    ATOM_DELETE(divfunction);
    ATOM_DELETE(negfunction);
    ATOM_DELETE(concatfunction);
    ATOM_DELETE(lenfunction);
    ATOM_DELETE(eqfunction);
    ATOM_DELETE(ltfunction);
    ATOM_DELETE(callfunction);
    ATOM_DELETE(constructfunction);
  }
private:
  ATOM_CommandList (const ATOM_CommandList&);
  void operator = (const ATOM_CommandList&);
};

struct ATOM_ClassStruct
{
  const char *name;
  const char *purename;
  ATOM_BaseCreator *creator;
  ATOM_InstanceOp *instance_op;
  const char *parentname;
  void *(*castfunc)(void*);
  ATOM_ClassStruct *parent;
  ATOM_CommandList *commands;
  ~ATOM_ClassStruct () { 
    ATOM_DELETE(creator);
    ATOM_DELETE(instance_op);
    ATOM_DELETE(commands);
  }
  void help (void)
  {
    char buffer[4096];

    ATOM_ClassStruct *p = this;
    for (ATOM_ClassStruct *cs = p; cs; cs = cs->parent)
    {
      printf ("-- CLASS %s --\n", cs->name);

      if (cs->commands)
      {
        printf("  METHODS:%d\n", cs->commands->commands.size());
        for (unsigned i = 0; i < cs->commands->commands.size();  ++i)
        {
          if (!cs->commands->commands[i]->command->_isAttrib)
          {
            cs->commands->commands[i]->command->helpstr (buffer);
            printf("    %s\n", buffer);
          }
        }

        printf("  ATTRIBUTES:%d\n", cs->commands->commands.size());
        for (unsigned i = 0; i < cs->commands->commands.size();  ++i)
        {
          if (cs->commands->commands[i]->command->_isAttrib)
          {
            cs->commands->commands[i]->command->helpstr (buffer);
            printf("    %s\n", buffer);
          }
        }

        printf("  STATIC METHODS:\n");
        for (unsigned i = 0; i < cs->commands->staticcommands.size(); ++i)
        {
          cs->commands->staticcommands[i]->function->helpstr(buffer);
          printf("    %s\n", buffer);
        }
        printf("  CONSTANTS:\n");
        for (unsigned i = 0; i < cs->commands->constants.size(); ++i)
        {
          printf("    %s\n", cs->commands->constants[i]->function->_purename);
        }
      }
    }
  }
};

template <class ObjectType>
void ATOM_ObjectDerefFunction (void *p)
{
  if (p)
  {
    ::ATOM_ReleaseObject((ObjectType*)p);
  }
}

template <class ObjectType>
void ATOM_DeleteDerefFunction (void *p)
{
  ATOM_DELETE((ObjectType*)p);
}

template <class ObjectType>
void ATOM_DefaultDeleteDerefFunction (void *p)
{
  delete ((ObjectType*)p);
}

template <class ObjectType>
void ATOM_DefaultDeleteArrayDerefFunction (void *p)
{
  delete [] ((ObjectType*)p);
}

template <class T> struct ATOM_VariantValue;

#define __ATOM_VARIANT_NUMBER_VALUE__(type) \
template <> \
struct ATOM_VariantValue<type> \
{ \
  type operator () (const ATOM_ScriptVariant &v) const { \
    switch (v.getType()) \
    { \
    case ATOM_ScriptVariant::TYPE_INT: \
      return (type)v.asInteger(); \
    case ATOM_ScriptVariant::TYPE_FLOAT: \
      return (type)v.asFloat(); \
    default: \
      throw std::runtime_error("Invalid variant type"); \
    } \
  } \
};

__ATOM_VARIANT_NUMBER_VALUE__(char)
__ATOM_VARIANT_NUMBER_VALUE__(signed char)
__ATOM_VARIANT_NUMBER_VALUE__(unsigned char)
__ATOM_VARIANT_NUMBER_VALUE__(short)
__ATOM_VARIANT_NUMBER_VALUE__(unsigned short)
__ATOM_VARIANT_NUMBER_VALUE__(int)
__ATOM_VARIANT_NUMBER_VALUE__(unsigned int)
__ATOM_VARIANT_NUMBER_VALUE__(long)
__ATOM_VARIANT_NUMBER_VALUE__(unsigned long)
__ATOM_VARIANT_NUMBER_VALUE__(long long)
__ATOM_VARIANT_NUMBER_VALUE__(unsigned long long)
__ATOM_VARIANT_NUMBER_VALUE__(float)
__ATOM_VARIANT_NUMBER_VALUE__(double)

template <>
struct ATOM_VariantValue<bool>
{
  bool operator () (const ATOM_ScriptVariant &v) const {
    switch (v.getType())
    {
    case ATOM_ScriptVariant::TYPE_INT:
      return v.asInteger() != 0;
    case ATOM_ScriptVariant::TYPE_FLOAT:
      return v.asFloat() != 0;
    case ATOM_ScriptVariant::TYPE_BOOL:
      return v.asBool();
    default:
      throw std::runtime_error("Invalid variant type");
    }
  }
};

template <>
struct ATOM_VariantValue<const char*>
{
  const char *operator () (const ATOM_ScriptVariant &v) const {
    switch (v.getType())
    {
    case ATOM_ScriptVariant::TYPE_STRING:
      return v.asString();
    default:
      throw std::runtime_error("Invalid variant type");
    }
  }
};

template <class Alloc>
struct ATOM_VariantValue<std::basic_string<char, std::char_traits<char>, Alloc> >
{
  std::basic_string<char, std::char_traits<char>, Alloc> operator () (const ATOM_ScriptVariant &v) const {
    switch (v.getType())
    {
    case ATOM_ScriptVariant::TYPE_STRING:
      return v.asString();
    default:
      throw std::runtime_error("Invalid variant type");
    }
  }
};

template <class T>
struct ATOM_VariantValue<T*>
{
  T * operator () (const ATOM_ScriptVariant &v) const {
    switch (v.getType())
    {
    case ATOM_ScriptVariant::TYPE_POINTER:
      return (T*)v.asPointer();
    default:
      throw std::runtime_error("Invalid variant type");
    }
  }
};

//---- For implement scriptable objects.
#define ATOM_SCRIPT_DECLARE_TYPE_TRAITS(name, classname, no_instance) \
template <> struct __ATOM_ParamHelper__< classname* > { \
classname *operator () (ATOM_Script *scp, int index) { \
  return (classname*)scp->getParameterc (index); \
} \
}; \
template <> struct __ATOM_ReturnHelper__< classname* > { \
int operator () (ATOM_Script *scp, const classname *ret, ATOM_DereferenceFunc deref) { \
  return scp->setReturnc (const_cast< classname* >(ret), #name, deref); \
} \
}; \
template <> struct __ATOM_ReturnHelper__< classname& > { \
int operator () (ATOM_Script *scp, const classname &ret, ATOM_DereferenceFunc deref) { \
  return scp->setReturnc (const_cast< classname* >(&ret), #name, 0); \
} \
}; \
template <> struct __ATOM_ReturnHelper__< classname > { \
static const bool ATOM_IsAbstract = ATOM_IntOr<no_instance, ATOM_IsAbstract< classname >::result>::value; \
int operator () (ATOM_Script *scp, const classname &ret, ATOM_DereferenceFunc deref) { \
  return ::ATOM_ReturnCiImpl< classname, ATOM_IsAbstract >()(scp, ret, #name, deref); \
} \
};

#define ATOM_SCRIPT_DECLARE_NONCOPYABLE_TYPE_TRAITS(name, classname, no_instance) \
template <> struct __ATOM_ParamHelper__< classname* > { \
classname *operator () (ATOM_Script *scp, int index) { \
  return (classname*)scp->getParameterc (index); \
} \
}; \
template <> struct __ATOM_ReturnHelper__< classname* > { \
int operator () (ATOM_Script *scp, const classname *ret, ATOM_DereferenceFunc deref) { \
  return scp->setReturnc (const_cast< classname* >(ret), #name, deref); \
} \
}; \
template <> struct __ATOM_ReturnHelper__< classname& > { \
int operator () (ATOM_Script *scp, const classname &ret, ATOM_DereferenceFunc deref) { \
  return scp->setReturnc (const_cast< classname* >(&ret), #name, 0); \
} \
}; \
template <> struct __ATOM_ReturnHelper__< classname > { \
static const bool ATOM_IsAbstract = ATOM_IntOr<no_instance, ATOM_IsAbstract< classname >::result>::value; \
int operator () (ATOM_Script *scp, const classname &ret, ATOM_DereferenceFunc deref) { \
	throw std::runtime_error("Try return instance of noncopyable class\n"); \
} \
};

#define ATOM_ARRAY_GETTER_FUNCNAME(classname, member) classname##_get##member##_binding
#define ATOM_ARRAY_SETTER_FUNCNAME(classname, member) classname##_set##member##_binding
#define ATOM_OBJECT_DEREF_FUNCNAME(classname) ATOM_ObjectDerefFunction< classname >
#define ATOM_DELETE_DEREF_FUNCNAME(classname) ATOM_DeleteDerefFunction< classname >
#define ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) classname##_AutoRefReturnBinder_##funcname##_binding

#define ATOM_DECL_AUTOREF_RETURN_BINDER0(classname, returntype, funcname) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER1(classname, returntype, funcname, argtype1, arg1) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER2(classname, returntype, funcname, argtype1, arg1, argtype2, arg2) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER3(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER4(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3, argtype4, arg4) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3, argtype4 arg4) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3, arg4); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER5(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3, argtype4, arg4, argtype5, arg5) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3, argtype4 arg4, argtype5 arg5) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3, arg4, arg5); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER6(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3, argtype4, arg4, argtype5, arg5, argtype6, arg6) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3, argtype4 arg4, argtype5 arg5, argtype6 arg6) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3, arg4, arg5, arg6); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER7(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3, argtype4, arg4, argtype5, arg5, argtype6, arg6, argtype7, arg7) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3, argtype4 arg4, argtype5 arg5, argtype6 arg6, argtype7 arg7) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3, arg4, arg5, arg6, arg7); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER8(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3, argtype4, arg4, argtype5, arg5, argtype6, arg6, argtype7, arg7, argtype8, arg8) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3, argtype4 arg4, argtype5 arg5, argtype6 arg6, argtype7 arg7, argtype8 arg8) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER9(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3, argtype4, arg4, argtype5, arg5, argtype6, arg6, argtype7, arg7, argtype8, arg8, argtype9, arg9) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3, argtype4 arg4, argtype5 arg5, argtype6 arg6, argtype7 arg7, argtype8 arg8, argtype9 arg9) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_AUTOREF_RETURN_BINDER10(classname, returntype, funcname, argtype1, arg1, argtype2, arg2, argtype3, arg3, argtype4, arg4, argtype5, arg5, argtype6, arg6, argtype7, arg7, argtype8, arg8, argtype9, arg9, argtype10, arg10) \
static returntype * ATOM_AUTOREF_RETURN_BINDER_FUNCNAME(classname, funcname) (classname *__p, argtype1 arg1, argtype2 arg2, argtype3 arg3, argtype4 arg4, argtype5 arg5, argtype6 arg6, argtype7 arg7, argtype8 arg8, argtype9 arg9, argtype10 arg10) { \
ATOM_AUTOREF(returntype) __r = __p->funcname (arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); \
if (__r) __r->addRef(); \
return __r.get(); \
}

#define ATOM_DECL_OBJECT_DEREF_FUNCTION(classname) \
static void ATOM_OBJECT_DEREF_FUNCNAME(classname) (void *p) { \
::ATOM_ReleaseObject ((classname*)p); \
}

#define ATOM_DECL_ARRAY_ATTRIBUTE_GETTER(classname, member, count, type) \
static ATOM_ScriptVariant ATOM_ARRAY_GETTER_FUNCNAME(classname, member) (const classname *__p) { \
ATOM_ScriptVariant __v[count]; \
for (unsigned __i = 0; __i < (count); ++__i) \
  __v[__i] = __p->member[__i]; \
return ATOM_ScriptVariant (__v, count); \
} 

#define ATOM_DECL_ARRAY_ATTRIBUTE_SETTER(classname, member, count, type) \
static void ATOM_ARRAY_SETTER_FUNCNAME(classname, member) (classname *p, const ATOM_ScriptVariant &v) { \
if (v.getType() != ATOM_ScriptVariant::TYPE_ARRAY) \
  throw std::runtime_error("Invalid parameter type"); \
if (v.getArraySize() != count) \
  throw std::runtime_error("Invalid array size"); \
for (unsigned __i = 0; __i < count; ++__i)  { \
  const ATOM_ScriptVariant &__t = v.asArray()[__i]; \
  p->member[__i] = ATOM_VariantValue<type>()(__t); \
} \
}

#define ATOM_SCRIPT_BEGIN_FUNCTION_TABLE(functions) \
::ATOM_FunctionStruct  functions[] = {

#define ATOM_DECLARE_FUNCTION(name, func) \
{ createFunctionCaller (#name, func) },

#define ATOM_DECLARE_FUNCTION_DEREF(name, func, deref) \
{ createFunctionCallerRef (#name, func, deref) },

#define ATOM_SCRIPT_END_FUNCTION_TABLE \
{ 0 }, \
};

#define ATOM_SCRIPT_BEGIN_CONSTANT_TABLE(constants) \
::ATOM_FunctionStruct constants[] = {

#define ATOM_DECLARE_CONSTANT(name, constant) \
{ createConstantAccessor (#name, constant) },

#define ATOM_DECLARE_INT_CONSTANT(name, constant) \
{ createConstantAccessor (#name, (int)constant) },

#define ATOM_SCRIPT_END_CONSTANT_TABLE \
{ 0 }, \
};

#define ATOM_DECLARE_STATIC_METHOD(name, method) \
->addStaticCommand(::createFunctionCaller(#name, &method))

#define ATOM_DECLARE_STATIC_METHOD_DEREF(name, method, deref) \
->addStaticCommand(::createFunctionCallerRef(#name, &method, deref))

#define ATOM_DECLARE_METHOD(name, method) \
->addCommand(::createCommandCaller (#name, &method))

#define ATOM_DECLARE_METHOD_DEREF(name, method, deref) \
->addCommand(::createCommandCallerRef (#name, &method, deref))

#define ATOM_DECLARE_PRINT_METHOD(method) \
->addNameCommand(::createCommandCaller ("", &method))

#define ATOM_DECLARE_INDEX_METHOD(method) \
->addIndexCommand(::createIndexCommandCaller (&method))

#define ATOM_DECLARE_INDEX_METHOD_DEREF(method, deref) \
->addIndexCommand(::createIndexCommandCaller (&method, deref))

#define ATOM_DECLARE_NEWINDEX_METHOD(method) \
->addNewIndexCommand(::createNewIndexCommandCaller (&method))

#define ATOM_DECLARE_ADD_OP(method) \
->addAddFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_SUB_OP(method) \
->addSubFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_MUL_OP(method) \
->addMulFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_DIV_OP(method) \
->addDivFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_NEG_OP(method) \
->addNegFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_CONCAT_OP(method) \
->addConcatFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_LEN_OP(method) \
->addLenFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_EQ_OP(method) \
->addEqFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_LT_OP(method) \
->addLtFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_CALL_OP(method) \
->addCallFunction(::createCommandCaller("", &method))

#define ATOM_DECLARE_CONSTRUCTOR(func) \
->setConstructor(::createFunctionCaller ("", &func))

#define ATOM_DECLARE_CONSTRUCTOR_DEREF(func, deref) \
->setConstructor(::createFunctionCallerRef ("", &func, deref))

#define ATOM_DECLARE_ATTRIBUTE(name, readfunc, writefunc) \
->addCommand(::createAttribAccessor (#name, &readfunc, &writefunc))

#define ATOM_DECLARE_ATTRIBUTE_DEREF(name, readfunc, writefunc, deref) \
->addCommand(::createAttribAccessorRef (#name, &readfunc, &writefunc, deref))

#define ATOM_DECLARE_ATTRIBUTE_READONLY(name, readfunc) \
->addCommand(::createAttribAccessorReadonly (#name, &readfunc))

#define ATOM_DECLARE_ATTRIBUTE_READONLY_DEREF(name, readfunc, deref) \
->addCommand(::createAttribAccessorReadonlyRef (#name, &readfunc, deref))

#define ATOM_DECLARE_RAW_ATTRIBUTE(name, classname, member, type) \
ATOM_DECLARE_ATTRIBUTE(name, (::ATOM_RawReadFunc< classname, type, &classname::member >), (::ATOM_RawWriteFunc< classname, type, &classname::member >))

#define ATOM_DECLARE_RAW_ATTRIBUTE_DEREF(name, classname, member, type, deref) \
ATOM_DECLARE_ATTRIBUTE_DEREF(name, (::ATOM_RawReadFunc< classname, type, &classname::member >), (::ATOM_RawWriteFunc< classname, type, &classname::member >), deref)

#define ATOM_DECLARE_RAW_ATTRIBUTE_READONLY(name, classname, member, type) \
ATOM_DECLARE_ATTRIBUTE_READONLY(name, (::ATOM_RawReadFunc< classname, type, &classname::member >), )

#define ATOM_DECLARE_RAW_ATTRIBUTE_READONLY_DEREF(name, classname, member, type, deref) \
ATOM_DECLARE_ATTRIBUTE_READONLY_DEREF(name, (::ATOM_RawReadFunc< classname, type, &classname::member >), deref)

#define ATOM_DECLARE_STATIC_CONSTANT(name, value) \
->addConstant(::createConstantAccessor (#name, value))

#define ATOM_DECLARE_STATIC_INT_CONSTANT(name, value) \
->addConstant(::createConstantAccessor (#name, int(value)))

#define ATOM_DECLARE_STATIC_FLOAT_CONSTANT(name, value) \
->addConstant(::createConstantAccessor (#name, float(value)))

#define ATOM_DECLARE_STATIC_DOUBLE_CONSTANT(name, value) \
->addConstant(::createConstantAccessor (#name, double(value)))

#define ATOM_SCRIPT_BEGIN_TYPE_TABLE(classes) \
::ATOM_ClassStruct classes[] = {

#define ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS(name, classname, parentclass, parentclassname) \
{ #name, 0, ATOM_NEW(::ATOM_ObjectCreator< classname >), ATOM_NEW(::ATOM_InstanceOpTNonCopyable< classname >), #parentclassname, &::ATOM_CastPointer< classname, parentclass >::cast, 0, (ATOM_NEW(::ATOM_CommandList))

#define ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT(name, classname) \
{ #name, 0, ATOM_NEW(::ATOM_ObjectCreator< classname >), ATOM_NEW(::ATOM_InstanceOpTNonCopyable< classname >), 0, 0, 0, (ATOM_NEW(::ATOM_CommandList))

#define ATOM_SCRIPT_BEGIN_CLASS(name, classname, parentclass, parentclassname) \
{ #name, \
  0, \
  ATOM_NEW(::ATOM_ObjectCreator< classname >), \
  ATOM_NEW(::ATOM_InstanceOpTCopyable< classname >), \
  #parentclassname, \
  &::ATOM_CastPointer< classname, parentclass >::cast, \
  0, \
  (ATOM_NEW(::ATOM_CommandList))->addCommand(::createCommandCaller("_Copy", &::ATOM_CopyOp< classname >::copy))

#define ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(name, classname) \
{ #name, \
  0, \
  ATOM_NEW(::ATOM_ObjectCreator< classname >), \
  ATOM_NEW(::ATOM_InstanceOpTCopyable< classname >), \
  0, \
  0, \
  0, \
  (ATOM_NEW(::ATOM_CommandList))->addCommand(::createCommandCaller("_Copy", &::ATOM_CopyOp< classname >::copy))

#define ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOCONSTRUCTOR(name, classname, parentclass, parentclassname) \
{ #name, 0, ATOM_NEW(::ATOM_NullObjectCreator< classname >), 0, #parentclassname, &::ATOM_CastPointer< classname, parentclass >::cast, 0, (ATOM_NEW(::ATOM_CommandList))

#define ATOM_SCRIPT_BEGIN_NONCOPYABLE_CLASS_NOPARENT_NOCONSTRUCTOR(name, classname) \
{ #name, 0, ATOM_NEW(::ATOM_NullObjectCreator< classname >), 0, 0, 0, 0, (ATOM_NEW(::ATOM_CommandList))

#define ATOM_SCRIPT_BEGIN_CLASS_NOCONSTRUCTOR(name, classname, parentclass, parentclassname) \
{ #name, \
  0, \
  ATOM_NEW(::ATOM_NullObjectCreator< classname >), \
  0, \
  #parentclassname, \
  &::ATOM_CastPointer< classname, parentclass >::cast, \
  0, \
  (ATOM_NEW(::ATOM_CommandList))->addCommand(::createCommandCaller("_Copy", &::ATOM_CopyOp< classname >::copy))

#define ATOM_SCRIPT_BEGIN_CLASS_NOPARENT_NOCONSTRUCTOR(name, classname) \
{ #name, \
  0, \
  ATOM_NEW(::ATOM_NullObjectCreator< classname >), \
  0, \
  0, \
  0, \
  0, \
  (ATOM_NEW(::ATOM_CommandList))->addCommand(::createCommandCaller("_Copy", &::ATOM_CopyOp< classname >::copy))

#define ATOM_SCRIPT_END_CLASS() \
},

#define ATOM_SCRIPT_END_TYPE_TABLE  \
{ 0, 0, 0, 0, 0 }  \
};

#define ATOM_SCRIPT_REGISTER_FUNCTION_TABLE(scp, functions) \
(scp)->registerFunctions(functions, ATOM_SCRIPT_ARRAY_SIZE(functions)-1)

#define ATOM_SCRIPT_REGISTER_TYPE_TABLE(scp, classes) \
(scp)->registerTypes(classes, ATOM_SCRIPT_ARRAY_SIZE(classes)-1)

#define ATOM_SCRIPT_REGISTER_CONSTANT_TABLE(scp, constants) \
(scp)->registerConstants(constants, ATOM_SCRIPT_ARRAY_SIZE(constants)-1)

#endif //__ATOM_SCRIPT_SCRIPTINTERFACE_H
