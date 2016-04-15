/********************************************************************
	created:	2005/08/23
	created:	23:8:2005   12:16
	filename: 	I:\Heeze\Util\safe_delete.h
	file path:	I:\Heeze\Util
	file base:	safe_delete
	file ext:	h
	author:		Dashi Bai
	email:		gzxb525@gmail.com
	
	purpose:	
*********************************************************************/


#ifndef HEEZE_SAFE_DELETE_H
#define HEEZE_SAFE_DELETE_H

#include "mydm.h"
HEEZE_NAMESPACE_BEGIN

template<class T>
void safe_delete( T& p){
	if( p ){
		delete p;
		p = NULL;
	}
}

HEEZE_NAMESPACE_END

#endif	// HEEZE_SAFE_DELETE_H