/********************************************************************
	created:	2005/08/23
	created:	23:8:2005   12:15
	filename: 	I:\Heeze\Util\mydm.h
	file path:	I:\Heeze\Util
	file base:	mydm
	file ext:	h
	author:		Dashi Bai
	email:		gzxb525@gmail.com
	
	purpose:	
*********************************************************************/


#ifndef HEEZE_DM_H
#define HEEZE_DM_H

#include <memory>
#include <stdexcept>

#define HEEZE_NAMESPACE_BEGIN namespace heeze{
#define HEEZE_NAMESPACE_END	};

#if defined _USRDLL
#	define HEEZE_EXPORT __declspec (dllexport)
//#	define HEEZE_EXPORT __declspec (dllimport)
#else
#	define HEEZE_EXPORT
#endif

//////////////////////////////////////////////////////////////////////////
// Define special class
#ifndef HEEZE_NO_INSTANCE 
#define HEEZE_NO_INSTANCE(CLASS)		\
protected:								\
	CLASS(){}					
#endif

#ifndef HEEZE_DESTROY_SELF
#define HEEZE_DESTROY_SELF(CLASS)		\
protected:								\
	virtual ~CLASS(){}					
#endif

#ifndef HEEZE_NO_COPY_CLASS 
#define HEEZE_NO_COPY_CLASS(CLASS)		\
protected:								\
	CLASS(const CLASS&){}				\
	CLASS &operator=(const CLASS&){}	
#endif

#ifndef HEEZE_NO_DERIVED
#define HEEZE_NO_DERIVED(CLASS)			\
private:								\
	CLASS();
#endif

/*
#define HEEZE_KEY_RELATION_OPERATOR( CLASS, KEYVAR)	\
bool operator ==(const CLASS& ref){		\
	return this->KEYVAR == ref.KEYVAR;	\
}	\
bool operator !=(const CLASS& ref){		\
	return this->KEYVAR != ref.KEYVAR;	\
}	\
bool operator >(const CLASS& ref){		\
	return this->KEYVAR > ref.KEYVAR;	\
}	\
bool operator <(const CLASS& ref){		\
	return this->KEYVAR < ref.KEYVAR;	\
}	\
bool operator >=(const CLASS& ref){		\
	return this->KEYVAR >= ref.KEYVAR;	\
}	\	
bool operator <=(const CLASS& ref){		\
	return this->KEYVAR <= ref.KEYVAR;	\
}
*/

HEEZE_NAMESPACE_BEGIN

class null_type;
class empty_type{
};

typedef unsigned long ulong;

class bad_ptr
	: public std::exception{
public:
	bad_ptr(const char* msg = _MESG("bad point")) _THROW0()
		: exception(msg){
	}

	virtual ~bad_ptr() _THROW0()
	{
	}
};

HEEZE_NAMESPACE_END

#endif	// HEEZE_DM_H