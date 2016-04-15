/**	\file object.h
 *	�������������������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_OBJECT_H
#define __ATOM_KERNEL_OBJECT_H

#if _MSC_VER > 1000
# pragma once
#endif

#ifdef _MSC_VER
# pragma warning(disable:4250)
#endif

#include "../ATOM_dbghlp.h"
#include "tinyxml.h"
#include "thread.h"
#include "refobj.h"
#include "weakptr.h"

//! ����һ�������
#define ATOM_CLASS(ns, classname, name) \
  friend class ns##_plugin_impl;  \
  public: \
  static const char * _classname () { return #name; } \
  static ATOM_Object * _create () { return ATOM_NEW(classname); }  \
  static void _destroy (ATOM_Object *p) { ATOM_DELETE(dynamic_cast<classname*>(p)); } \
  static void _purge (ATOM_Object *p) { ATOM_DELETE(dynamic_cast<classname*>(p)); }

//! ����һ������Ĳ������
#define ATOM_CLASS_PURE(ns, classname, name) \
  public: \
  static const char * _classname () { return #name; }

//! ����һ�������Ľű��ӿ�
#define ATOM_DECLARE_SCRIPT_INTERFACE(classname) \
  private: \
  friend class classname##_ScriptInterface; \
  class classname##_ScriptInterface: public ATOM_ScriptInterfaceBase { \
    protected: \
	  classname##_ScriptInterface (void) {} \
      virtual ~classname##_ScriptInterface (); \
    public: \
      virtual unsigned getAttribCount (); \
      virtual ATOM_AttribAccessorBase *findAttrib (const char *name); \
      virtual ATOM_AttribAccessorBase *getAttrib (unsigned index); \
      virtual void cleanup (void); \
    public: \
      static classname##_ScriptInterface *getInstance(); \
    private: \
      static ATOM_AttribAccessorBase *_attribs[]; \
      static unsigned _attrib_count; \
  }; \
  public: \
    static ATOM_ScriptInterfaceBase *_get_script_interface();

//! ��ʼ��������Ľű��ӿ�
#define ATOM_SCRIPT_INTERFACE_BEGIN(classname) \
  ATOM_ScriptInterfaceBase * classname::_get_script_interface() {  \
	  return classname::classname##_ScriptInterface::getInstance(); \
  } \
  classname::classname##_ScriptInterface *classname::classname##_ScriptInterface::getInstance() { \
    static classname::classname##_ScriptInterface _instance; \
    return &_instance; \
  };

//! ��ʼ�������������
#define ATOM_ATTRIBUTES_BEGIN(classname) \
  ATOM_AttribAccessorBase *classname::classname##_ScriptInterface::_attribs[] = {

//! ����һ������������
#define ATOM_ATTRIBUTE(classname, namestr, readfunc, writefunc, comment) \
  ATOM_AttribAccessorGenerate (namestr, &classname::writefunc, &classname::readfunc, comment),

//! ����һ��������ֻ������
#define ATOM_ATTRIBUTE_READONLY(classname, namestr, readfunc, comment) \
  ATOM_ReadonlyAttribAccessorGenerate (namestr, &classname::readfunc, comment),

//! ����һ�������Ŀɴ浵����
#define ATOM_ATTRIBUTE_PERSISTENT(classname, namestr, readfunc, writefunc, defaultvalue, comment) \
  ATOM_PersistentAttribAccessorGenerate (namestr, &classname::writefunc, &classname::readfunc, defaultvalue, comment),

//! ����һ�������Ĳ�����Ĭ��ֵ�Ŀɴ浵����
#define ATOM_ATTRIBUTE_PERSISTENT_NODEFAULT(classname, namestr, readfunc, writefunc, comment) \
  ATOM_NoDefaultPersistentAttribAccessorGenerate (namestr, &classname::writefunc, &classname::readfunc, comment),

//! �����������������
#define ATOM_ATTRIBUTES_END() \
  0 };

//! ������������Ľű��ӿ�
#define ATOM_SCRIPT_INTERFACE_END(classname, superclass) \
  unsigned classname::classname##_ScriptInterface::_attrib_count = sizeof(classname::classname##_ScriptInterface::_attribs)/sizeof(classname::classname##_ScriptInterface::_attribs[0]) - 1; \
  unsigned classname::classname##_ScriptInterface::getAttribCount () { \
    return classname::classname##_ScriptInterface::_attrib_count + superclass::_get_script_interface()->getAttribCount(); \
  } \
  ATOM_AttribAccessorBase *classname::classname##_ScriptInterface::getAttrib (unsigned index) { \
    if (index >= classname::classname##_ScriptInterface::getAttribCount())  \
      return 0; \
    if (index < classname::classname##_ScriptInterface::_attrib_count) \
      return classname::classname##_ScriptInterface::_attribs[index]; \
    else \
      return superclass::_get_script_interface()->getAttrib (index - classname::classname##_ScriptInterface::_attrib_count); \
  } \
  ATOM_AttribAccessorBase *classname::classname##_ScriptInterface::findAttrib (const char *name) { \
    if (!name) return 0; \
    int n = sizeof(classname##_ScriptInterface::_attribs)/sizeof(classname##_ScriptInterface::_attribs[0]); \
    for (int i = 0; i < n; ++i) { \
      if (classname##_ScriptInterface::_attribs[i] && !strcmp(name, classname##_ScriptInterface::_attribs[i]->attrib.name)) \
        return classname##_ScriptInterface::_attribs[i]; \
    } \
    return superclass::_get_script_interface()->findAttrib (name); \
  } \
  void classname::classname##_ScriptInterface::cleanup (void) { \
    int n = sizeof(classname##_ScriptInterface::_attribs)/sizeof(classname##_ScriptInterface::_attribs[0]); \
    for (int i = 0; i < n; ++i) { \
      ATOM_DELETE(classname##_ScriptInterface::_attribs[i]); \
      classname##_ScriptInterface::_attribs[i] = 0; \
    } \
    _attrib_count = 0; \
  } \
  classname::classname##_ScriptInterface::~classname##_ScriptInterface () { \
    cleanup (); \
  }
    
#define ATOM_SCRIPT_INTERFACE_ROOT_END(classname) \
  unsigned classname::classname##_ScriptInterface::_attrib_count = sizeof(classname::classname##_ScriptInterface::_attribs)/sizeof(classname::classname##_ScriptInterface::_attribs[0]) - 1; \
  unsigned classname::classname##_ScriptInterface::getAttribCount () { \
    return classname::classname##_ScriptInterface::_attrib_count; \
  } \
  ATOM_AttribAccessorBase *classname::classname##_ScriptInterface::getAttrib (unsigned index) { \
    if (index < classname::classname##_ScriptInterface::_attrib_count) \
      return classname::classname##_ScriptInterface::_attribs[index]; \
    return 0; \
  } \
  ATOM_AttribAccessorBase *classname::classname##_ScriptInterface::findAttrib (const char *name) { \
    if (!name) return 0; \
    int n = sizeof(classname##_ScriptInterface::_attribs)/sizeof(classname##_ScriptInterface::_attribs[0]); \
    for (int i = 0; i < n; ++i) { \
      if (classname##_ScriptInterface::_attribs[i] && !strcmp(name, classname##_ScriptInterface::_attribs[i]->attrib.name)) \
        return classname##_ScriptInterface::_attribs[i]; \
    } \
    return 0; \
  } \
  void classname::classname##_ScriptInterface::cleanup (void) { \
    int n = sizeof(classname##_ScriptInterface::_attribs)/sizeof(classname##_ScriptInterface::_attribs[0]); \
    for (int i = 0; i < n; ++i) { \
      ATOM_DELETE(classname##_ScriptInterface::_attribs[i]); \
      classname##_ScriptInterface::_attribs[i] = 0; \
    } \
    _attrib_count = 0; \
  } \
  classname::classname##_ScriptInterface::~classname##_ScriptInterface () { \
    cleanup (); \
  }

struct ATOM_AttribAccessorBase;

class ATOM_KERNEL_API ATOM_ScriptInterfaceBase 
  {
  public:
    virtual unsigned getAttribCount () { return 0; }
    virtual ATOM_AttribAccessorBase *findAttrib (const char *name) { return 0; }
    virtual ATOM_AttribAccessorBase *getAttrib (unsigned index) { return 0; }
    virtual void cleanup (void) {}
  };

struct ATOM_RuntimeAttrib
{
  const char *name;
  const char *typedesc;
  const char *comment;
  int type;

  bool operator == (const ATOM_RuntimeAttrib &other) const {
    return other.type == type && !strcmp(other.name, name);
  }
  bool operator != (const ATOM_RuntimeAttrib &other) const {
    return ! operator == (other);
  }
};

struct ATOM_RuntimeFunc
{
  const char *name;
  const char *comment;
  ATOM_VECTOR<int> paramtypes;
  ATOM_VECTOR<const char*> paramtypedescs;
  int returntype;
  const char *returntypedesc;

  bool operator == (const ATOM_RuntimeFunc &other) const {
    if (other.returntype != returntype)
      return false;

    if (other.paramtypes.size() != paramtypes.size())
      return false;

    if (strcmp (other.name, name))
      return false;

    for (unsigned i = 0; i < paramtypes.size(); ++i)
    {
      if (paramtypes[i] != other.paramtypes[i])
        return false;
    }

    return true;
  }

  bool operator != (const ATOM_RuntimeFunc &other) const {
    return ! operator == (other);
  }
};

//! \class ATOM_Object
//! �����Ļ��࣬���в�����ɴ�������
//! \author ������
class ATOM_KERNEL_API ATOM_Object: public ATOM_ReferenceObj
{
	ATOM_CLASS_PURE(kernel, ATOM_Object, ATOM_Object)
	ATOM_DECLARE_SCRIPT_INTERFACE(ATOM_Object)
	friend class ATOM_ObjectFactory;
	friend class ATOM_Object_ScriptInterface;

public:
	//! ���캯��
	ATOM_Object (void);
	//! ��������
	virtual ~ATOM_Object (void);

public:
	//! �õ��ò��ʵ��������
	//! \return ʵ������
	const char* getObjectName (void) const;

	//! �õ��ò��ʵ��������
	//! \return ʵ��������
	const char* getClassName (void) const;

	//! ��ȡ�������б�
	//! \return �������б�
	ATOM_WeakPtrList &getWeakPtrList (void);

	//! �����������Ե���������
	//! \param obj ���������ԵĶ���
	virtual void copyAttributesTo (ATOM_Object *obj) const;

	//! ��XML�ڵ�������ʵ��������ֵ
	//! \param xmlelement XML�ڵ�
	//! \return true �ɹ� false ���ɹ�
	//! \note ֻ�пɴ浵������ֵ�ſ��Ա�����
	//! \sa ATOM_Object::writeAttribute
	virtual bool loadAttribute(const ATOM_TiXmlElement *xmlelement);

	//! д��ʵ��������ֵ��XML�ڵ�
	//! \param xmlelement XML�ڵ�
	//! \return true �ɹ� false ���ɹ�
	//! \note ֻ�пɴ浵������ֵ�ſ��Ա�д��
	//! \sa ATOM_Object::loadAttribute
	virtual bool writeAttribute(ATOM_TiXmlElement *xmlelement);

	//! ����������ֵд����־
	//! \param indent д��ʱ�������
	//! \param attrib Ҫ������������ƣ����ΪNULL�������������
	//! \note ���������Ҫ�ǵ�����
	virtual void dumpAttributes(const char *indent, const char *attrib = 0);

private:
	void setObjectName(const char* name);
	void setClassName(const char* name);

private:
	ATOM_STRING att_GetClassName (void) const;
	const ATOM_STRING &att_GetObjectName (void) const;

protected:
	ATOM_STRING _M_object_name;
	const char *_M_class_name;
	ATOM_WeakPtrList _M_weakptr_list;
};

#endif // __ATOM_KERNEL_OBJECT_H
/*! @} */
