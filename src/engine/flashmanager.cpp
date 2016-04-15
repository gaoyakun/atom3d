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
#include "flashmanager.h"
#include "keyboardhook.h"
#include <objbase.h>
#include <direct.h>
#include <stdlib.h>

ATOM_STRING getCurrentWorkingDirectory()
{
	ATOM_STRING workingDirectory = "";
	char currentPath[_MAX_PATH];
	getcwd(currentPath, _MAX_PATH);
	workingDirectory = currentPath;

	return workingDirectory + "\\";
}

FlashManager* FlashManager::instance = 0;

FlashManager::FlashManager(const ATOM_STRING& assetsDirectory) : flashLib(0), focusedControl(0), mouseXPos(0), mouseYPos(0), 
	mouseButtonRDown(false), zOrderCounter(1), keyboardHook(0)
{
	if(instance)
		throw std::exception("In FlashManager constructor, FlashManager is already instantiated!");

	instance = this;

	ATOM_STRING workingDirectory = getCurrentWorkingDirectory();
	/*
	this->basePath = workingDirectory + assetsDirectory;

	if(this->basePath.at(this->basePath.length()-1) != '\\')
		this->basePath.push_back('\\');
	*/

	CoInitialize(0);

	// Attempt to load 'Flash.ocx', it's alrite if this fails, we have a fallback strategy
	flashLib = LoadLibrary((workingDirectory + "Flash.ocx").c_str());

	keyboardHook = new KeyboardHook();
}

FlashManager::~FlashManager()
{
	for(ControlMap::iterator i = controls.begin(); i != controls.end();)
	{
		FlashControl* toDelete = i->second;
		i = controls.erase(i);
		delete toDelete;
	}

	if(keyboardHook) delete keyboardHook;

	if(flashLib) FreeLibrary(flashLib);

	CoUninitialize();

	instance = 0;
}

FlashManager& FlashManager::Get()
{
	if(!instance)
		instance = new FlashManager ("");
		//throw std::exception("In FlashManager::Get(), FlashManager is uninitialized!");

	return *instance;
}

FlashManager* FlashManager::GetPointer()
{
	return instance;
}

FlashControl* FlashManager::getFlashControl(const char *controlName) const
{
	ControlMap::const_iterator i = controls.find(controlName);
	if(i != controls.end())
		return i->second;

	return 0;
}

void FlashManager::update()
{
	for(ControlMap::iterator iter = controls.begin(); iter != controls.end();)
	{
		if(iter->second->okayToDelete)
		{
			FlashControl* controlToDelete = iter->second;
			iter = controls.erase(iter);
			if(focusedControl == controlToDelete) focusedControl = 0;
			delete controlToDelete;
		}
		else
		{
			iter->second->update();
			iter++;
		}
	}
}

bool FlashManager::isAnyFocused()
{
	return !!focusedControl;
}

void FlashManager::defocusAll()
{
	focusedControl = 0;
}

bool FlashManager::injectMouseMove(short x, short y)
{
	bool eventHandled = false;

	for(ControlMap::iterator iter = controls.begin(); iter != controls.end(); iter++)
	{
		/*
		if(iter->second->overlay)
			iter->second->injectMouseMove(iter->second->overlay->getRelativeX(x), iter->second->overlay->getRelativeY(y));
		*/

		if(!eventHandled)
			if(iter->second->isPointOverMe(x, y))
				eventHandled = true;
	}

	mouseXPos = x;
	mouseYPos = y;

	return eventHandled;
}

bool FlashManager::injectMouseDown(int buttonID)
{
	if(buttonID == LeftMouseButton)
	{
		if(focusControl(mouseXPos, mouseYPos))
		{
			/*
			int relX = focusedControl->overlay->getRelativeX(mouseXPos);
			int relY = focusedControl->overlay->getRelativeY(mouseYPos);

			focusedControl->injectMouseDown(relX, relY);
			*/
		}
	}
	else if(buttonID == RightMouseButton)
	{
		mouseButtonRDown = true;
		focusControl(mouseXPos, mouseYPos);
	}

	if(focusedControl)
		return true;

	return false;
}

bool FlashManager::injectMouseUp(int buttonID)
{
	if(buttonID == LeftMouseButton && focusedControl)
	{
		/*
		if(focusedControl->overlay)
			focusedControl->injectMouseUp(focusedControl->overlay->getRelativeX(mouseXPos), focusedControl->overlay->getRelativeY(mouseYPos));
		*/
	}
	else if(buttonID == RightMouseButton)
		mouseButtonRDown = false;

	if(focusedControl)
		return true;

	return false;
}

bool FlashManager::injectMouseWheel(int relScroll)
{
	if(focusedControl)
	{
		if(focusedControl->isPointOverMe(mouseXPos, mouseYPos))
		{
			//focusedControl->injectMouseWheel(relScroll, focusedControl->overlay->getRelativeX(mouseXPos), focusedControl->overlay->getRelativeY(mouseYPos));
			return true;
		}
	}

	return false;
}

void FlashManager::injectKeyEvent(UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*
	if(focusedControl)
		focusedControl->handleKeyEvent(msg, wParam, lParam);
	*/
}

void FlashManager::setKeyboardHookEnabled(bool isEnabled)
{
	if(isEnabled)
	{
		if(!keyboardHook)
			keyboardHook = new KeyboardHook();
	}
	else
	{
		if(keyboardHook)
		{
			delete keyboardHook;
			keyboardHook = 0;
		}
	}
}

bool FlashManager::focusControl(int x, int y, FlashControl* selection)
{
	/*
	defocusAll();

	if(selection)
	{
		if(!selection->overlay)
		{
			focusedControl = selection;
			return true;
		}
	}

	FlashControl* controlToFocus = selection? selection : getTopControl(x, y);

	if(!controlToFocus)
		return false;

	std::vector<FlashControl*> sortedControls;

	std::map<ATOM_STRING, FlashControl*>::iterator iter;
	for(iter = controls.begin(); iter != controls.end(); iter++)
		if(iter->second->overlay)
			sortedControls.push_back(iter->second);

	struct compare { bool operator()(FlashControl* a, FlashControl* b){ return(a->overlay->overlay->getZOrder() > b->overlay->overlay->getZOrder()); }};
	std::sort(sortedControls.begin(), sortedControls.end(), compare());

	if(sortedControls.size())
	{
		if(sortedControls.at(0) != controlToFocus)
		{
			unsigned int popIdx = 0;
			for(; popIdx < sortedControls.size(); popIdx++)
				if(sortedControls.at(popIdx) == controlToFocus)
					break;

			unsigned short highestZ = sortedControls.at(0)->overlay->overlay->getZOrder();
			for(unsigned int i = 0; i < popIdx; i++)
				sortedControls.at(i)->overlay->overlay->setZOrder(sortedControls.at(i+1)->overlay->overlay->getZOrder());
			
			sortedControls.at(popIdx)->overlay->overlay->setZOrder(highestZ);
		}
	}

	focusedControl = controlToFocus;
	*/
	return true;
}

FlashControl* FlashManager::getTopControl(int x, int y)
{
	/*
	FlashControl* top = 0;

	std::map<Ogre::String, FlashControl*>::iterator iter;
	for(iter = controls.begin(); iter != controls.end(); iter++)
	{
		if(!iter->second->isPointOverMe(x, y))
			continue;

		if(!top)
			top = iter->second;
		else
			top = top->overlay->panel->getZOrder() > iter->second->overlay->panel->getZOrder() ? top : iter->second;
	}

	return top;
	*/
	return 0;
}