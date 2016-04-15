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

#include "stdafx.h"
#include "flashcontrol.h"
#include "flashsite.h"
#include "flashhandler.h"
#include "flashmanager.h"
#include "renderbuffer.h"
#include "flashctrl.h"
#include "gui_events.h"

static const char flashTextureMaterial[] =
"<?xml version=\"1.0\"?>\n\
<material auto=\"0\">\n\
	<param name=\"texture\" type=\"texture\" />\n\
	<param name=\"filter\" type=\"int\" />\n\
	<param name=\"zfunc\" type=\"int\" />\n\
	<effect name=\"default\">\n\
		<pass>\n\
			<streamdefine>\n\
				<stream semantec=\"position\"/>\n\
				<stream semantec=\"color0\" />\n\
				<stream semantec=\"texcoord0\" type=\"float2\" />\n\
			</streamdefine>\n\
			<alphablend_state>\n\
				<enable value=\"true\" />\n\
				<srcblend value=\"srcalpha\" />\n\
				<destblend value=\"invsrcalpha\" />\n\
			</alphablend_state>\n\
			<depth_state>\n\
				<zfunc value=\"$zfunc\" />\n\
				<zwrite value=\"false\" />\n\
			</depth_state>\n\
			<rasterizer_state>\n\
				<cullmode value=\"none\" />\n\
			</rasterizer_state>\n\
			<texturestage_state index=\"0\">\n\
				<colorop value=\"modulate\" />\n\
				<colorarg1 value=\"texture\" />\n\
				<colorarg2 value=\"diffuse\" />\n\
				<alphaop value=\"modulate\" />\n\
				<alphaarg1 value=\"texture\" />\n\
				<alphaarg2 value=\"diffuse\" />\n\
			</texturestage_state>\n\
			<texturestage_state index=\"1\">\n\
				<colorop value=\"disable\" />\n\
				<alphaop value=\"disable\" />\n\
			</texturestage_state>\n\
			<sampler_state index=\"0\">\n\
				<filter value=\"$filter\" />\n\
				<addressu value=\"clamp\" />\n\
				<addressv value=\"clamp\" />\n\
				<texture value=\"$texture\" />\n\
			</sampler_state>\n\
		</pass>\n\
	</effect>\n\
</material>\n";

FlashControl::FlashControl(ATOM_FlashCtrl *parent, const char *name, int width, int height)
:	owner(parent),
	name(name),
	site(0),
	handler(0),
	flashInterface(0),
	oleObject(0), windowlessObject(0),
	width(width), height(height),
	state(0),
	comCount(0),
	mainContext(0),	mainBitmap(0), mainBuffer(0),
	altContext(0), altBitmap(0), altBuffer(0),
	renderBuffer(0),
	usingAlphaHack(true),
	lastDirtyWidth(0),
	lastDirtyHeight(0),
	charFlag(0),
	isClean(true), isTotallyDirty(false),
	texWidth(width), texHeight(height), texDepth(0), texPitch(0), 
	compensateNPOT(false), isTransparent(false), okayToDelete(false), isDraggable(false),
	isIgnoringTransparent(false), transThreshold(0)
{
	renderBuffer = new RenderBuffer(width, height);
	createControl();
	createMaterial();
}

FlashControl::~FlashControl()
{
	if (flashInterface)
	{
		stop ();
	}

	if(windowlessObject) windowlessObject->Release();
	if(flashInterface) flashInterface->Release();
	
	if(handler)
	{
		handler->Shutdown();
		handler->Release();
	}

	if(oleObject)
	{
		oleObject->Close(OLECLOSE_NOSAVE);
		oleObject->Release();
	}

	if(site) site->Release();

	if(comCount)
		ATOM_LOGGER::warning("WARNING: Hikari::FlashControl is leaking COM objects!");
	
	if(mainContext) ::DeleteDC(mainContext);
	if(mainBitmap) ::DeleteObject(mainBitmap);
	if(altContext) ::DeleteDC(altContext);
	if(altBitmap) ::DeleteObject(altBitmap);
	if(renderBuffer) delete renderBuffer;

	::Sleep (200);
}

typedef HRESULT (__stdcall *GetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID * ppv); 

void FlashControl::createControl()
{
	site = new FlashSite();
	site->AddRef();	
	site->Init(this);
	
	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName ("/support/Flash10c.ocx", buffer);
	HMODULE flashLib = ::LoadLibraryA (buffer);
	if (!flashLib)
	{
		HMODULE flashLib = FlashManager::Get().flashLib;
	}
	
	// Try to load from user-supplied Flash OCX first
	if(flashLib)
	{
		IClassFactory* factory = 0;
		GetClassObject getClassFunc = (GetClassObject)GetProcAddress(flashLib, "DllGetClassObject");
		HRESULT result = getClassFunc(ShockwaveFlashObjects::CLSID_ShockwaveFlash, IID_IClassFactory, (void**)&factory);
		if(SUCCEEDED(result))
		{
			factory->CreateInstance(NULL, IID_IOleObject, (void**)&oleObject);
			factory->Release();	
		}
	}

	// If we still don't have the object, try loading from registry
	if(!oleObject)
	{
		HRESULT result = CoCreateInstance(ShockwaveFlashObjects::CLSID_ShockwaveFlash, 0, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&oleObject);
		if(FAILED(result))
		{
			flashInterface =0;
			return;
		}
	}

	IOleClientSite* clientSite = 0;
	site->QueryInterface(__uuidof(IOleClientSite), (void**)&clientSite);
	oleObject->SetClientSite(clientSite);

	IOleInPlaceObject* inPlaceObject = 0;	
	oleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*)&inPlaceObject);			

	if(inPlaceObject)
	{
		invalidateTotally();

		inPlaceObject->SetObjectRects(&dirtyBounds, &dirtyBounds);
		inPlaceObject->Release();
	}

	oleObject->QueryInterface(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (LPVOID*)&flashInterface);

	flashInterface->PutWMode(_bstr_t("transparent"));
	flashInterface->PutQuality2("Best");
	flashInterface->PutScaleMode(2);
	flashInterface->DisableLocalSecurity ();
	flashInterface->PutEmbedMovie (FALSE);
	flashInterface->PutAllowScriptAccess (L"always");

	oleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, 0, clientSite, 0, 0, 0);
	clientSite->Release();	
		
	oleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (LPVOID*)&windowlessObject);
			
	handler = new FlashHandler();
	handler->AddRef();	
	handler->Init(this);
}

ATOM_Texture *FlashControl::getTexture (void) const
{
	return texture.get();
}

void FlashControl::createMaterial()
{
	texture = 0;
	texWidth = width;
	texHeight = height;

	// Create the texture
	texture = ATOM_GetRenderDevice()->allocTexture (0, 0, texWidth, texHeight, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
	ATOM_Texture::LockedRect lockedRect;
	texture->lock (0, &lockedRect);

	texDepth = 4;
	texPitch = lockedRect.pitch;

	unsigned char* pDest = static_cast<unsigned char*>(lockedRect.bits);

	memset(pDest, 0, texHeight*texPitch);

	texture->unlock(0);

	if (!material)
	{
		material = ATOM_MaterialManager::createMaterialFromCore (ATOM_GetRenderDevice(), "/materials/builtin/flash.mat");
		if (!material)
		{
			  material = ATOM_MaterialManager::createMaterialFromCoreString (ATOM_GetRenderDevice(), flashTextureMaterial);
		}
	}

	if (material)
	{
		material->setActiveEffect ("default");
	}

	invalidateTotally();
}

bool FlashControl::load(const ATOM_STRING& movieFilename)
{
	if (!flashInterface)
	{
		return false;
	}

	{
		ATOM_AutoFile f(movieFilename.c_str(), ATOM_VFS::read);
		if (!f)
		{
			return false;
		}
	}

	char physicBuffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (movieFilename.c_str(), physicBuffer);

	flashInterface->PutMovie(physicBuffer);
	return true;
}

void FlashControl::play()
{
	if (flashInterface)
	{
		flashInterface->Play();
	}
}

void FlashControl::stop()
{
	if (flashInterface && flashInterface->IsPlaying ())
	{
		flashInterface->StopPlay();
	}
}

void FlashControl::rewind()
{
	if (flashInterface)
	{
		flashInterface->Rewind();
	}
}

void FlashControl::gotoFrame(long frameNum)
{
	if (flashInterface)
	{
		flashInterface->raw_GotoFrame(frameNum);
	}
}

void FlashControl::setLoop(bool shouldLoop)
{
	if (flashInterface)
	{
		flashInterface->PutLoop(shouldLoop);
	}
}

void FlashControl::setTransparent(bool isTransparent, bool useAlphaHack)
{
	if (flashInterface)
	{
		this->isTransparent = isTransparent;
		if(isTransparent)
		{
			flashInterface->PutWMode("transparent");
			usingAlphaHack = useAlphaHack;
		}
		else
		{
			flashInterface->PutWMode("opaque");
			usingAlphaHack = false;
		}
	
		createMaterial();
	}
}

void FlashControl::setQuality(short renderQuality)
{
	if (flashInterface)
	{
		switch(renderQuality)
		{
		case RQ_LOW:
			flashInterface->PutQuality2("low");
			break;
		case RQ_MEDIUM:
			flashInterface->PutQuality2("medium");
			break;
		case RQ_HIGH:
			flashInterface->PutQuality2("high");
			break;
		case RQ_BEST:
			flashInterface->PutQuality2("best");
			break;
		case RQ_AUTOLOW:
			flashInterface->PutQuality2("autolow");
			break;
		case RQ_AUTOHIGH:
			flashInterface->PutQuality2("autohigh");
			break;
		}
	}
}

void FlashControl::setScaleMode(short scaleMode)
{
	if (flashInterface)
	{
		flashInterface->PutScaleMode(scaleMode);
	}
}

void FlashControl::setDraggable(bool isDraggable)
{
	this->isDraggable = isDraggable;
}

void FlashControl::setIgnoreTransparentPixels(bool shouldIgnore, float threshold)
{
	isIgnoringTransparent = shouldIgnore;
	transThreshold = threshold;
}

const ATOM_STRING & FlashControl::getName() const
{
	return name;
}

void FlashControl::bind(const char *funcName, const FlashDelegate& callback)
{
	if(!funcName || callback.empty())
		return;

	delegateMap[funcName] = callback;
}

void FlashControl::unbind(const char *funcName)
{
	DelegateIter i = delegateMap.find(funcName);
	if(i != delegateMap.end())
		delegateMap.erase(i);
}

ATOM_FlashValue FlashControl::callFunction(const char *funcName, const ATOM_FlashArguments& args)
{
	if (!flashInterface)
	{
		return ATOM_FlashValue();
	}

	BSTR returnVal = 0;
	HRESULT result = flashInterface->raw_CallFunction(_bstr_t(serializeInvocation(funcName, args).c_str()), &returnVal);

#if 0
#ifdef _DEBUG
	if(FAILED(result))
		throw "Failed to call ActionScript function";
#endif
#endif

	wchar_t *r = (wchar_t*)returnVal;
	if (r)
	{
		ATOM_STRING str;
		convertFromWStringToString (std::wstring((wchar_t*)returnVal), str);
		return deserializeValue(str);
	}

	return FLASH_VOID;
}

void FlashControl::hide()
{
}

void FlashControl::show()
{
}

bool FlashControl::getVisibility() const
{
	return false;
}

void FlashControl::setOpacity(float opacity)
{
}

void FlashControl::getExtents(unsigned short &width, unsigned short &height) const
{
	width = this->width;
	height = this->height;
}

void FlashControl::getUVScale(float &uScale, float &vScale) const
{
	uScale = vScale = 1;

	if(compensateNPOT)
	{
		uScale = (float)width/(float)texWidth;
		vScale = (float)height/(float)texHeight;
	}
}

void FlashControl::injectMouseMove(int xPos, int yPos)
{
	LRESULT result;
	windowlessObject->OnWindowMessage(WM_MOUSEMOVE, 0, MAKELPARAM(xPos, yPos), &result);
}

void FlashControl::injectMouseDown(int xPos, int yPos)
{
	LRESULT result;
	windowlessObject->OnWindowMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(xPos, yPos), &result);
}

void FlashControl::injectMouseUp(int xPos, int yPos)
{
	LRESULT result;
	windowlessObject->OnWindowMessage(WM_LBUTTONUP, 0, MAKELPARAM(xPos, yPos), &result);
}

#ifndef WM_MOUSEWHEEL
#	define WM_MOUSEWHEEL 0x020A
#endif

void FlashControl::injectMouseWheel(int relScroll, int xPos, int yPos)
{
	LRESULT result;
	windowlessObject->OnWindowMessage(WM_MOUSEWHEEL, MAKEWPARAM(0, relScroll), MAKELPARAM(xPos, yPos), &result);
}

bool FlashControl::isPointOverMe(int screenX, int screenY)
{
	return false;
}

void FlashControl::invalidateTotally()
{
	isClean = false;
	isTotallyDirty = true;
	dirtyBounds.left = 0;
	dirtyBounds.top = 0;
	dirtyBounds.right = width;
	dirtyBounds.bottom = height;
}

void FlashControl::handleCharEvent(unsigned short ch)
{
	LRESULT aResult;
	windowlessObject->OnWindowMessage(WM_UNICHAR, ch, 0, &aResult);
}

void FlashControl::handleKeyEvent(ATOM_AppWMEvent *event)
{
	LRESULT aResult = 0;
	UINT msg = event->msg;

	switch (msg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSDEADCHAR:
	case WM_SYSCHAR:
	case WM_IME_CHAR:
	case WM_IME_COMPOSITION:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_CONTROL:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_KEYDOWN:
	case WM_IME_KEYUP:
	case WM_IME_NOTIFY:
	case WM_IME_REQUEST:
	case WM_IME_SELECT:
	case WM_IME_SETCONTEXT:
	case WM_IME_STARTCOMPOSITION:
	case WM_HELP:
	case WM_CANCELMODE:
		windowlessObject->OnWindowMessage (msg, event->wParam, event->lParam, &aResult);
		break;
	}
}

void FlashControl::handleFlashCall(const std::wstring& xmlString)
{
	if (flashInterface)
	{
		ATOM_STRING funcName;
		ATOM_STRING xmlStringAnsi;
		convertFromWStringToString (xmlString, xmlStringAnsi);

		ATOM_FlashArguments args;

		if(!deserializeInvocation(xmlStringAnsi, funcName, args))
			return;

		ATOM_FlashCallEvent event(owner->getId(), funcName.c_str(), args);
		owner->handleEvent (&event);
		ATOM_WSTRING wStr;
		convertFromStringToWString (serializeValue(event.returnVal), wStr);
		flashInterface->SetReturnValue(wStr.c_str());
	}
	/*
	DelegateIter i = delegateMap.find(funcName);
	if(i != delegateMap.end())
	{
		ATOM_FlashValue retval = i->second(this, args);
		ATOM_WSTRING wStr;
		convertFromStringToWString (serializeValue(retval), wStr);
		flashInterface->SetReturnValue(wStr.c_str());
	}
	*/
}

void FlashControl::handleFSCommand(const std::wstring& cmd, const std::wstring &args)
{
	ATOM_FlashFSCommandEvent event(owner->getId(), cmd.c_str(), args.c_str());
	owner->handleEvent (&event);
}

void FlashControl::update()
{
	if(isClean || !flashInterface)
		return;

	int dirtyWidth = dirtyBounds.right - dirtyBounds.left;
	int dirtyHeight = dirtyBounds.bottom - dirtyBounds.top;
	int dirtyBufSize = dirtyWidth * dirtyHeight * 4;

	IViewObject* curView = 0;
	flashInterface->QueryInterface(IID_IViewObject, (void**)&curView);

	if(!oleObject || !curView)
		return;

	if(!mainContext || dirtyWidth != lastDirtyWidth || dirtyHeight != lastDirtyHeight)
	{
		if(mainContext)
		{
			DeleteDC(mainContext);
			mainContext = 0;
		}
		if(mainBitmap)
		{
			DeleteObject(mainBitmap);
			mainBitmap = 0;
		}
		
		lastDirtyWidth = dirtyWidth;
		lastDirtyHeight = dirtyHeight;

		HDC hdc = GetDC(0);
		BITMAPINFOHEADER bih = {0};
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biPlanes = 1;
		bih.biWidth = dirtyWidth;
		bih.biHeight = -dirtyHeight;
		mainContext = CreateCompatibleDC(hdc);
		mainBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bih, DIB_RGB_COLORS, (void**)&mainBuffer, 0, 0);
		SelectObject(mainContext, mainBitmap);

		if(usingAlphaHack)
		{
			if(altContext)
			{
				DeleteDC(altContext);
				altContext = 0;
			}
			if(altBitmap)
			{
				DeleteObject(altBitmap);
				altBitmap = 0;
			}

			altContext = CreateCompatibleDC(hdc);
			altBitmap = CreateDIBSection(hdc, (BITMAPINFO*)&bih, DIB_RGB_COLORS, (void **)&altBuffer, 0, 0);
			SelectObject(altContext, altBitmap);
		}

		ReleaseDC(0, hdc);
	}

	RECT local;
	local.left = -dirtyBounds.left;
	local.top = -dirtyBounds.top;
	local.right = local.left + dirtyBounds.right;
	local.bottom = local.top + dirtyBounds.bottom;

	if(!usingAlphaHack)
	{
		memset(mainBuffer, 0, dirtyBufSize);
		
		HRESULT hr = OleDraw(curView, DVASPECT_TRANSPARENT, mainContext, &local);
	}
	else 
	{
		memset(mainBuffer, 0, dirtyBufSize);
		memset(altBuffer, 255, dirtyBufSize);

		OleDraw(curView, DVASPECT_TRANSPARENT, mainContext, &local);
		OleDraw(curView, DVASPECT_TRANSPARENT, altContext, &local);

		// We've rendered the dirty area twice: once on black and once
		// on white. Now we compare the red channels of each to determine
		// the alpha value of each pixel.
		BYTE *blackBuffer, *whiteBuffer;
		blackBuffer = mainBuffer;
		whiteBuffer = altBuffer;
		BYTE blackRed, whiteRed;
		int size = dirtyWidth * dirtyHeight;
		for(int i = 0; i < size; i++)
		{
			blackRed = *blackBuffer;
			whiteRed = *whiteBuffer;
			blackBuffer += 3;
			whiteBuffer += 4;
			*blackBuffer++ = 255 - (whiteRed - blackRed);
		}
	}

	renderBuffer->copyArea(dirtyBounds, mainBuffer, dirtyWidth * 4);
	
	ATOM_Texture::LockedRect lockedRect;
	texture->lock (0, &lockedRect);

	unsigned char * destBuffer = static_cast<unsigned char*>(lockedRect.bits);
	renderBuffer->blitBGR(destBuffer, (int)texPitch, (int)texDepth);

	texture->unlock(0);

	isClean = true;
	isTotallyDirty = false;

	curView->Release ();
}

ATOM_Material *FlashControl::getMaterial (void) const
{
	return material.get();
}

unsigned FlashControl::getWidth () const
{
	return width;
}

unsigned FlashControl::getHeight () const
{
	return height;
}

void FlashControl::resize (int w, int h)
{
	if (w == width && h == height)
		return;

	IOleInPlaceObject* pInPlaceObject = NULL;
	oleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*) &pInPlaceObject);
	if (pInPlaceObject != NULL)
	{
		RECT rect = { 0, 0, w, h};
		pInPlaceObject->SetObjectRects(&rect, &rect);
		pInPlaceObject->Release();

		width = w;
		height = h;
		texWidth = w;
		texHeight = h;

		texture = ATOM_GetRenderDevice()->allocTexture (0, 0, texWidth, texHeight, ATOM_PIXEL_FORMAT_BGRA8888, ATOM_Texture::TEXTURE2D|ATOM_Texture::NOCOMPRESS|ATOM_Texture::NOMIPMAP);
		material->getParameterTable()->setTexture ("texture", texture.get());

		ATOM_Texture::LockedRect lockedRect;
		texture->lock (0, &lockedRect);
		texDepth = 4;
		texPitch = lockedRect.pitch;
		unsigned char* pDest = static_cast<unsigned char*>(lockedRect.bits);
		memset(pDest, 0, texHeight*texPitch);
		texture->unlock(0);

		renderBuffer->resize (w, h);
		invalidateTotally();
	}

	/*
	if (m_alphaBlackDC)
	{
		DeleteDC(m_alphaBlackDC);
		DeleteObject(m_alphaBlackBitmap);
		DeleteDC(m_alphaWhiteDC);
		DeleteObject(m_alphaWhiteBitmap);

		m_alphaBlackDC = NULL;
		m_alphaBlackBitmap = NULL;
		m_alphaBlackBuffer = NULL;
		m_alphaWhiteDC = NULL;
		m_alphaWhiteBitmap = NULL;
		m_alphaWhiteBuffer = NULL;
	}
	*/
}

int FlashControl::getTransparency (int x, int y) const
{
	return renderBuffer->getTransparency (x, y);
}

