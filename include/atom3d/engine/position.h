#ifndef ATOM3D_FLASH_POSITION_H
#define ATOM3D_FLASH_POSITION_H

#include "basedefs.h"

/**
* Defines the relative positions for the Position object.
*/
enum RelativePosition
{
	Left,
	TopLeft,
	TopCenter,
	TopRight,
	Right,
	BottomRight,
	BottomCenter,
	BottomLeft,
	Center
};

/**
* Used by HikariManager::createFlashOverlay to define the position of the overlay.
*/
struct ATOM_ENGINE_API Position
{
	bool usingRelative;
	union {
		struct { RelativePosition position; short x; short y; } rel;
		struct { short left; short top; } abs;
	} data;

	/**
	* Creates a default Position object (absolute, top-left corner).
	*/
	Position();

	/**
	* Creates a relatively-positioned Position object.
	*
	* @param	relPosition		The position of the Object in relation to the Viewport
	*
	* @param	offsetLeft	How many pixels from the left to offset the Object from the relative position.
	*
	* @param	offsetTop	How many pixels from the top to offset the Object from the relative position.
	*/
	Position(const RelativePosition &relPosition, short offsetLeft = 0, short offsetTop = 0);

	/**
	* Creates an absolutely-positioned Position object.
	*
	* @param	absoluteLeft	The number of pixels from the left of the Render Window.
	*
	* @param	absoluteTop		The number of pixels from the top of the Render Window.
	*/
	Position(short absoluteLeft, short absoluteTop);
};


#endif // ATOM3D_FLASH_POSITION_H
