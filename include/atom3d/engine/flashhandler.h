/*
	This file is part of Hikari, a library that allows developers
	to use Flash in their Ogre3D applications.

	Copyright (C) 2008 Adam J. Simmons

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ATOM3D_FLASH_FLASHHANDLER_H
#define ATOM3D_FLASH_FLASHHANDLER_H

#import "flash10c.ocx" named_guids
#include <sstream>
#include "flashvalue.h"

inline void replaceAll(std::wstring &sourceStr, const std::wstring &replaceWhat, const std::wstring &replaceWith)
{
	for(size_t i = sourceStr.find(replaceWhat); i != ATOM_STRING::npos; i = sourceStr.find(replaceWhat, i + replaceWith.length()))
	{
		sourceStr.erase(i, replaceWhat.length());
		sourceStr.insert(i, replaceWith);
	}
}

inline void replaceAll(ATOM_STRING &sourceStr, const ATOM_STRING &replaceWhat, const ATOM_STRING &replaceWith)
{
	for(size_t i = sourceStr.find(replaceWhat); i != std::string::npos; i = sourceStr.find(replaceWhat, i + replaceWith.length()))
	{
		sourceStr.erase(i, replaceWhat.length());
		sourceStr.insert(i, replaceWith);
	}
}

template <class StringType, class WStringType>
void convertFromStringToWString (const StringType &src, WStringType &result) 
{
	unsigned strLength = src.length();
	wchar_t *dst = (wchar_t*)ATOM_MALLOC(strLength * 2);
	::MultiByteToWideChar(CP_ACP, 0, src.c_str(), strLength, dst, strLength+1);
	result = dst;
	ATOM_FREE(dst);
}

template <class StringType, class WStringType>
void convertFromWStringToString (const WStringType &src, StringType &result) 
{
	unsigned strLength = src.length();
	char *dst = (char*)ATOM_MALLOC(strLength*2+1);
	::WideCharToMultiByte (CP_ACP, 0, src.c_str(), strLength, dst, strLength*2+1, NULL, NULL);
	result = dst;
	ATOM_FREE(dst);
}

inline ATOM_STRING serializeValue(const ATOM_FlashValue& value)
{
	switch(value.getType())
	{
	case ATOM_FlashValue::FT_NULL:
		return "<null/>";
	case ATOM_FlashValue::FT_BOOLEAN:
		return value.getBool()? "<true/>" : "<false/>";
	case ATOM_FlashValue::FT_NUMBER:
		{
			static std::stringstream converter;
			converter.clear();
			converter.str("");
			converter << value.getNumber();
			return ("<number>" + converter.str() + "</number>").c_str();
		}
	case ATOM_FlashValue::FT_STRING:
		{
			ATOM_STRING stringVal = value.getString();
			replaceAll(stringVal, "&", "&amp;");
			replaceAll(stringVal, "\"", "&quot;");
			replaceAll(stringVal, "'", "&apos;");
			replaceAll(stringVal, "<", "&lt;");
			replaceAll(stringVal, ">", "&gt;");

			return ("<string>" + stringVal + "</string>").c_str();
		}
	}

	return "<null/>";
}

inline ATOM_FlashValue deserializeValue(const ATOM_STRING& valueStr)
{
	if(valueStr == "<null/>")
		return FLASH_VOID;
	else if(valueStr == "<true/>")
		return ATOM_FlashValue(true);
	else if(valueStr == "<false/>")
		return ATOM_FlashValue(false);

	if(valueStr.substr(0, 8) == "<string>")
	{
		ATOM_STRING stringVal = valueStr.substr(8, valueStr.find("</string>", 8) - 8);
		//std::wstring stringVal = valueStr.substr(8, valueStr.find(L"</string>", 8) - 8);
		replaceAll(stringVal, "&quot;", "\"");
		replaceAll(stringVal, "&apos;", "'");
		replaceAll(stringVal, "&lt;", "<");
		replaceAll(stringVal, "&gt;", ">");
		replaceAll(stringVal, "&amp;", "&");
		return ATOM_FlashValue(stringVal.c_str());
	}
	else if(valueStr.substr(0, 8) == "<number>")
	{
		static std::stringstream converter;
		converter.clear();
		float numValue = 0;

		converter.str(valueStr.substr(8, valueStr.find("</number>", 8) - 8).c_str());
		converter >> numValue;

		return ATOM_FlashValue(numValue);
	}

	return FLASH_VOID;
}

inline ATOM_STRING serializeInvocation(const ATOM_STRING& funcName, const ATOM_FlashArguments& args)
{
	ATOM_STRING result;

	result += "<invoke name=\"" + funcName + "\" returntype=\"xml\">";

	if(args.size())
	{
		result += "<arguments>";
		for(ATOM_FlashArguments::const_iterator i = args.begin(); i != args.end(); i++)
			result += serializeValue(*i);
		result += "</arguments>";
	}

	result += "</invoke>";

	return result;
}

inline bool deserializeInvocation(const ATOM_STRING& xmlString, ATOM_STRING& funcName, ATOM_FlashArguments& args)
{
	size_t indexA = 0;
	size_t indexB = 0;

	if((indexA = xmlString.find("<invoke name=\"")) == ATOM_STRING::npos)
		return false;

	if((indexB = xmlString.find("\"", indexA + 14)) == ATOM_STRING::npos)
		return false;

	funcName = xmlString.substr(indexA + 14, indexB - (indexA + 14));
	args.clear();

	if((indexA = xmlString.find("<arguments>", indexB)) == ATOM_STRING::npos)
		return true;

	indexA += 11;
	ATOM_STRING argString(xmlString.substr(indexA, xmlString.find("</arguments>", indexA) - indexA));

	for(indexA = 0, indexB = 0; true;)
	{
		if((indexA = argString.find("<", indexB)) == ATOM_STRING::npos)
			break;

		if((indexB = argString.find(">", indexA)) == ATOM_STRING::npos)
			break;

		if(argString[indexB-1] != '/')
		{
			if((indexB = argString.find(">", indexB + 1)) == ATOM_STRING::npos)
				break;
		}

		args.push_back(deserializeValue(argString.substr(indexA, indexB + 1 - indexA)));
	}

	return true;
}

class FlashHandler : public ShockwaveFlashObjects::_IShockwaveFlashEvents
{
public:	
	LPCONNECTIONPOINT connectionPoint;	
	DWORD cookie;
	int refCount;
	FlashControl* owner;

public:
	FlashHandler() : cookie(0), connectionPoint(0), refCount(0), owner(0)
	{		
	}

	virtual ~FlashHandler()
	{
		owner->comCount--;
	}

	HRESULT Init(FlashControl* owner)
	{
		this->owner = owner;
		owner->comCount++;

		HRESULT result = NOERROR;
		LPCONNECTIONPOINTCONTAINER cPointContainer = 0;
						
		if ((owner->flashInterface->QueryInterface(IID_IConnectionPointContainer, (void**)&cPointContainer) == S_OK) &&
			(cPointContainer->FindConnectionPoint(__uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents), &connectionPoint) == S_OK))			
		{
			IDispatch* dispatch = 0;
			QueryInterface(__uuidof(IDispatch), (void**)&dispatch);

			if(dispatch)
			{
				result = connectionPoint->Advise((LPUNKNOWN)dispatch, &cookie);
				dispatch->Release();
			}
		}
				
		if(cPointContainer)
			cPointContainer->Release();

		return result;
	}

	HRESULT Shutdown()
	{
		HRESULT result = S_OK;

		if(connectionPoint)
		{
			if(cookie)
			{
				result = connectionPoint->Unadvise(cookie);
				cookie = 0;
			}

			connectionPoint->Release();
			connectionPoint = 0;
		}

		return result;
	}
 
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
	{
		*ppv = 0;

		if (riid == IID_IUnknown)
		{
			*ppv = (LPUNKNOWN)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IDispatch)
		{
			*ppv = (IDispatch*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents))
		{
			*ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*) this;
			AddRef();
			return S_OK;
		}
		else
		{   
			return E_NOTIMPL;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{  
		return ++refCount;
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		--refCount;

		if(!refCount)
			delete this;		

		return refCount;
	}

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
	{
		return E_NOTIMPL; 
	}

	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid,DISPID* rgDispId)
	{
		return E_NOTIMPL; 
	}

	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
		WORD wFlags, ::DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult,
		::EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr)
	{
		switch(dispIdMember)
		{
		case 0x7a6:			
			break;
		case 0x96:			
			if((pDispParams->cArgs == 2) && (pDispParams->rgvarg[0].vt == VT_BSTR) && (pDispParams->rgvarg[1].vt == VT_BSTR))
				FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
			break;
		case 0xC5:
			if((pDispParams->cArgs == 1) && (pDispParams->rgvarg[0].vt == VT_BSTR))
				FlashCall(pDispParams->rgvarg[0].bstrVal);
			break;
		case DISPID_READYSTATECHANGE:					
			break;
		default:			
			return DISP_E_MEMBERNOTFOUND;
		} 
		
		return NOERROR;
	}

	HRESULT OnReadyStateChange (long newState)
	{	
		return S_OK;
	}
    
	HRESULT OnProgress(long percentDone )
	{		
		return S_OK;
	}

	/*	in flash:
		fscommand ("command", "args");

		in C++
		handle ATOM_FlashFSCommandEvent event
	*/
	HRESULT FSCommand (_bstr_t command, _bstr_t args)
	{
		// TODO: Handle FSCommand
		owner->handleFSCommand ((wchar_t*)command, (wchar_t*)args);

		return S_OK;
	}	

	/*	in flash:
		flash.external.ExternalInterface.call ("cppFunc");

		in C++
		handle ATOM_FlashCallEvent event
	*/
    HRESULT FlashCall (_bstr_t request)
	{
		owner->handleFlashCall((wchar_t*)request);

		return S_OK;
	}
};

#endif // ATOM3D_FLASH_FLASHHANDLER_H
