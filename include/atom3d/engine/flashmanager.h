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

#ifndef ATOM3D_FLASH_FLASHMANAGER_H
#define ATOM3D_FLASH_FLASHMANAGER_H

#include <string>
#include <vector>
#include <windows.h>
#include "flashcontrol.h"

class KeyboardHook;

enum MouseButtonID
{
	LeftMouseButton = 0,
	RightMouseButton, 
	MiddleMouseButton
};

/**
* The HikariManager is used to create/destroy FlashControls and handle global
* events such as mouse-input propagation and the management of keyboard focus.
*/
class ATOM_ENGINE_API FlashManager
{
public:
	/**
	* Instantiates the HikariManager singleton. You can access the singleton later
	* using the HikariManager::Get and GetPointer.
	*
	* @param	assetsDirectory	Specifies the directory (relative to the path of the
	*							executable) that holds your .swf files. This is used
	*							by FlashControl::load.
	*/
	FlashManager(const ATOM_STRING& assetsDirectory = "");

	/**
	* Explicitly destroys the HikariManager singleton and destroys any active FlashControls.
	*/
	~FlashManager();

	/**
	* Retrieves the HikariManager singleton (must be instantiated first!)
	*/
	static FlashManager& Get();

	/**
	* Retrieves a pointer to the HikariManager singleton.
	*/
	static FlashManager* GetPointer();

	/**
	* Retrieves a previously-created FlashControl by name.
	*
	* @param	controlName	The name of the FlashControl to retrieve.
	*
	* @return	If it is found, returns a pointer to the FlashControl, else returns 0.
	*/
	FlashControl* getFlashControl(const char *controlName) const;

	/**
	* Gives each active FlashControl a chance to update and destroys any
	* control that is flagged for destruction.
	*/
	void update();

	/**
	* Returns whether or not any FlashControl is focused (and is capturing keyboard input).
	*/
	bool isAnyFocused();

	/**
	* De-focuses any currently-focused FlashControl (removes keyboard focus).
	*/
	void defocusAll();

	/**
	* Injects a global mouse-move to be handled by all FlashControl overlays.
	*
	* @param	x	The absolute x-coordinate in screen-space.
	* @param	y	The absolute y-coordinate in screen-space.
	*
	* @return	Returns true if the coordinates are over a FlashControl overlay, else returns false.
	*/
	bool injectMouseMove(short x, short y);

	/**
	* Injects a global mouse-down to be handled by all FlashControl overlays.
	*
	* @param	buttonID	The MouseButtonID of the pressed button.
	*
	* @return	Returns true if the mouse event was handled by a FlashControl overlay, else returns false.
	*/
	bool injectMouseDown(int buttonID);

	/**
	* Injects a global mouse-up to be handled by all FlashControl overlays.
	*
	* @param	buttonID	The MouseButtonID of the released button.
	*
	* @return	Returns true if the mouse event was handled by a FlashControl overlay, else returns false.
	*/
	bool injectMouseUp(int buttonID);

	/**
	* Injects a global mouse-wheel event to be handled by all FlashControl overlays.
	*
	* @param	relScroll	The relative scroll amount of the mouse (in OIS, this is arg.state.Z.rel).
	*
	* @return	Returns true if the mouse event was handled by a FlashControl overlay, else returns false.
	*/
	bool injectMouseWheel(int relScroll);

	/**
	* Injects a keyboard-event to be handled by the focused FlashControl. You generally
	* don't need to call this unless you've disabled the keyboard-hook and wish to
	* inject keyboard/IME messages directly from your WindowProc.
	*
	* @param	msg		Specifies the message to be sent.
	* @param	wParam	Specifies additional message-specific information.
	* @param	lParam	Specifies additional message-specific information.
	*
	* @note You should only forward the following window messages using this function:
	*	<ul>
	*	<li>WM_KEYDOWN
	*	<li>WM_KEYUP
	*	<li>WM_CHAR
	*	<li>WM_DEADCHAR
	*	<li>WM_SYSKEYDOWN
	*	<li>WM_SYSKEYUP
	*	<li>WM_SYSDEADCHAR
	*	<li>WM_SYSCHAR
	*	<li>WM_IME_CHAR
	*	<li>WM_IME_COMPOSITION
	*	<li>WM_IME_COMPOSITIONFULL
	*	<li>WM_IME_CONTROL
	*	<li>WM_IME_ENDCOMPOSITION
	*	<li>WM_IME_KEYDOWN
	*	<li>WM_IME_KEYUP
	*	<li>WM_IME_NOTIFY
	*	<li>WM_IME_REQUEST
	*	<li>WM_IME_SELECT
	*	<li>WM_IME_SETCONTEXT
	*	<li>WM_IME_STARTCOMPOSITION
	*	<li>WM_HELP
	*	<li>WM_CANCELMODE
	*	</ul>
	*/
	void injectKeyEvent(UINT msg, WPARAM wParam, LPARAM lParam);

	/**
	* Sets whether or not the keyboard-hook is enabled and should
	* automatically catch and forward all keyboard messages to the
	* focused FlashControl. (Enabled by default)
	*
	* @param	isEnabled	Whether or not the keyboard-hook is enabled.
	*
	* @note	For some reason, IME doesn't work properly using the
	*	the keyboard hook to catch keyboard messages and so, as
	*	a workaround, you can disable the keyboard-hook and forward
	*	keyboard/IME messages directly to Hikari from your main
	*	window's WindowProc.
	*/
	void setKeyboardHookEnabled(bool isEnabled);

protected:

	bool focusControl(int x, int y, FlashControl* selection = 0);
	FlashControl* getTopControl(int x, int y);

	static FlashManager* instance;
	typedef std::map<ATOM_STRING, FlashControl*> ControlMap;
	ControlMap controls;
	HMODULE flashLib;
	//ATOM_STRING basePath;
	KeyboardHook* keyboardHook;
	FlashControl* focusedControl;
	int mouseXPos, mouseYPos;
	bool mouseButtonRDown;
	unsigned short zOrderCounter;

	friend class FlashControl;
	friend class KeyboardHook;
};

#endif // ATOM3D_FLASH_FLASHMANAGER_H
