#ifndef ATOM3D_FLASH_FLASHCONTROL_H
#define ATOM3D_FLASH_FLASHCONTROL_H

#include <windows.h>

#include "basedefs.h"
#include "position.h"
#include "delegate.h"

namespace ShockwaveFlashObjects { struct IShockwaveFlash; }

struct IOleObject;
struct IOleInPlaceObjectWindowless;

class FlashSite;
class FlashHandler;
class RenderBuffer;
class ViewportOverlay;

class ATOM_FlashCtrl;

/**
* Used by FlashControl::setQuality, defines the Flash rendering quality.
* 
* <ul>
* <li>RQ_LOW - Favors playback speed over appearance and never uses anti-aliasing.
* <li>RQ_MEDIUM - Applies some anti-aliasing and does not smooth bitmaps. It produces a better quality than the Low setting, but lower quality than the High setting.
* <li>RQ_HIGH - Favors appearance over playback speed and always applies anti-aliasing. If the movie does not contain animation, bitmaps are smoothed; if the movie has animation, bitmaps are not smoothed.
* <li>RQ_BEST - Provides the best display quality and does not consider playback speed. All output is anti-aliased and all bitmaps are smoothed.
* <li>RQ_AUTOLOW - Emphasizes speed at first but improves appearance whenever possible. Playback begins with anti-aliasing turned off. If the Flash Player detects that the processor can handle it, anti-aliasing is turned on.
* <li>RQ_AUTOHIGH - Emphasizes playback speed and appearance equally at first but sacrifices appearance for playback speed if necessary. Playback begins with anti-aliasing turned on. If the actual frame rate drops below the specified frame rate, anti-aliasing is turned off to improve playback speed.
* </ul>
*/
enum RenderQuality
{
	RQ_LOW,
	RQ_MEDIUM,
	RQ_HIGH,
	RQ_BEST,
	RQ_AUTOLOW,
	RQ_AUTOHIGH
};

/**
* Used by FlashControl::setScaleMode, defines the scaling mode to use when the aspect ratio of the control does not match that of the movie.
*
* <ul>
* <li>SM_SHOWALL - Preserves the movie's aspect ratio by adding borders. (Default)
* <li>SM_NOBORDER - Preserves the movie's aspect ratio by cropping the sides.
* <li>SM_EXACTFIT - Does not preserve the movie's aspect ratio, scales the movie to the dimensions of the control.
*/
enum ScaleMode
{
	SM_SHOWALL,
	SM_NOBORDER,
	SM_EXACTFIT
};

/**
* The FlashControl class is an instance of the Flash Player that is rendered dynamically to a texture and
* wrapped in an optional movable overlay using the Ogre3D engine.
*/
class ATOM_ENGINE_API FlashControl
{
	friend class FlashSite;
	friend class FlashHandler;
	friend class FlashManager;

protected:
	ATOM_FlashCtrl *owner;
	FlashSite* site;
	FlashHandler* handler;
	ShockwaveFlashObjects::IShockwaveFlash* flashInterface;
	IOleObject*	oleObject;
	IOleInPlaceObjectWindowless* windowlessObject;

	ATOM_STRING name;
	int width, height;
	int state;
	int comCount;
	HDC mainContext, altContext;
	HBITMAP mainBitmap, altBitmap;
	BYTE *mainBuffer, *altBuffer;
	RenderBuffer* renderBuffer;
	bool usingAlphaHack;
	RECT dirtyBounds;
	int lastDirtyWidth;
	int lastDirtyHeight;
	bool isClean, isTotallyDirty;

	int charFlag;
	WPARAM wParamChar;
	LPARAM lParamChar;

	size_t texWidth, texHeight, texDepth, texPitch;
	ATOM_AUTOREF(ATOM_Texture) texture;
	ATOM_AUTOPTR(ATOM_Material) material;
	bool compensateNPOT;
	bool isTransparent;
	bool isDraggable;
	bool isIgnoringTransparent;
	float transThreshold;

	typedef std::map<ATOM_STRING, FlashDelegate> DelegateMap;
	typedef DelegateMap::iterator DelegateIter;
	DelegateMap delegateMap;
	bool okayToDelete;

	void createControl();
	void createMaterial();
	bool isPointOverMe(int screenX, int screenY);
	void invalidateTotally();
	void handleFlashCall(const std::wstring& xmlString);
	void handleFSCommand(const std::wstring& cmd, const std::wstring &args);


public:

	FlashControl(ATOM_FlashCtrl *parent, const char *name, int width, int height);
	~FlashControl();
	ATOM_Material *getMaterial (void) const;
	void handleKeyEvent(ATOM_AppWMEvent *event);
	void handleCharEvent(unsigned short ch);
	void update();
	unsigned getWidth () const;
	unsigned getHeight () const;
	void resize (int w, int h);
	int getTransparency (int x, int y) const;
	ATOM_Texture *getTexture (void) const;

	/**
	* Loads a movie (a .swf file) into this FlashControl and begins playing.
	*
	* @param	movieFilename	The filename of the movie to load.
	*
	* @note	The specified movie should reside in the "assetsDirectory" that
	*		was declared when the HikariManager was instantiated.
	*/
	bool load(const ATOM_STRING& movieFilename);

	/**
	* Plays the currently-loaded movie.
	*/
	void play();

	/**
	* Stops the currently-loaded movie.
	*/
	void stop();

	/**
	* Rewinds the currently-loaded movie to the beginning.
	*/
	void rewind();

	/**
	* Navigates the currently-loaded movie to a certain frame and stops there.
	*/
	void gotoFrame(long frameNum);

	/**
	* Sets whether or not the currently-loaded movie should restart when
	* it reaches the end.
	*
	* @param	shouldLoop	Whether the currently-loaded movie should loop.
	*/
	void setLoop(bool shouldLoop);

	/**
	* Sets whether or not the currently-loaded movie should use a
	* transparent background instead of the default background-color.
	*
	* @param	isTransparent	Whether or not the movie should use "transparent" rendering.
	* @param	useAlphaHack	With some Flash versions, there are certain glitches with
	*							transparent rendering (usually with text and aliased geometry).
	*							Set this parameter to 'true' to use an alternative alpha-rendering
	*							hack that may mitigate these issues at the cost of some performance.
	*/
	void setTransparent(bool isTransparent, bool useAlphaHack = false);

	/**
	* Sets the Flash rendering quality for the currently-loaded movie.
	*
	* @param	renderQuality	The RenderQuality to use.
	*/
	void setQuality(short renderQuality);

	/**
	* Sets the scaling mode to use when the aspect ratio of the movie and control do not match.
	*
	* @param	scaleMode	The ScaleMode to use.
	*/
	void setScaleMode(short scaleMode);

	/**
	* Sets whether this FlashControl is draggable via the right-mouse-button,
	* this is only applicable to FlashControls created as an overlay.
	*
	* @param	isDraggable		Whether or not this FlashControl should be draggable.
	*/
	void setDraggable(bool isDraggable);

	/**
	* Sets whether or not mouse-clicks over transparent pixels should be ignored (this is on by default),
	* this is only applicable to transparent FlashControls created as an overlay.
	*
	* @param	shouldIgnore	Whether or not transparent pixels should be ignored.
	* @param	threshold	The opacity threshold (in percent, 0 to 1.0), pixels with
	*						opacities less than this amount will be ignored.
	*/
	void setIgnoreTransparentPixels(bool shouldIgnore, float threshold = 0.04);

	/**
	* Returns the name of this FlashControl.
	*/
	const ATOM_STRING & getName() const;

	/**
	* Binds a local callback to a certain function name so that your Flash movie can call the function
	* from ActionScript using ExternalInterface.call('functionName').
	*
	* @param	funcName	The name to bind this callback to.
	* @param	callback	The local function to call, see below for examples of declaring a FlashDelegate.
	*
	*	\code
	*	// Example declaration of a compatible function (static function):
	*	FlashValue myStaticFunction(FlashControl* caller, const Arguments& args)
	*	{
	*		// Handle the callback here
	*		return FLASH_VOID;
	*	}
	*
	*	// Example declaration of a compatible function (member function):
	*	FlashValue MyClass::myMemberFunction(FlashControl* caller, const Arguments& args)
	*	{
	*		// Handle the callback here
	*		return "Some return value!";
	*	}
	*
	*	// FlashDelegate (member function) instantiation:
	*	FlashDelegate callback(this, &MyClass::myMemberFunction); // within a class
	*	FlashDelegate callback2(pointerToClassInstance, &MyClass::myMemberFunction);
	*
	*	// FlashDelegate (static function) instantiation:
	*	FlashDelegate callback(&myStaticFunction);
	*	\endcode
	*/
	void bind(const char *funcName, const FlashDelegate& callback);

	/**
	* Un-binds the specified callback.
	*
	* @param	funcName	The name that the callback was bound to.
	*/
	void unbind(const char *funcName);

	/**
	* Attempts to call a function declared as a callback in the ActionScript of the currently-loaded movie.
	* 
	* @param	funcName	The name of the callback that was declared using 'ExternalInterface.addCallback(funcName, function)'
	*						in the ActionScript of the currently-loaded movie.
	* @param	args	The arguments to pass to the ActionScript function.
	*
	* @return	If the invocation was successful and the ActionScript function returned a value, returns a FlashValue with a non-null type.
	*
	* @note	It is highly recommended to use the 'Args(arg1)(arg2)(arg3)...' helper class to pass arguments.
	*/
	ATOM_FlashValue callFunction(const char *funcName, const ATOM_FlashArguments& args = ATOM_FlashArgs());

	/**
	* If this FlashControl was created as an overlay, hides the overlay.
	*/
	void hide();

	/**
	* If this FlashControl was created as an overlay, shows the overlay.
	*/
	void show();

	/**
	* Returns whether or not the FlashControl overlay is currently visible. (See FlashControl::hide and FlashControl::show)
	* If this FlashControl was not created as an overlay, always returns false.
	*/
	bool getVisibility() const;

	/**
	* Sets the opacity of this FlashControl.
	*
	* @param	opacity	The opacity as a Real value; 0 is totally transparent, 1 is totally opaque.
	*/
	void setOpacity(float opacity);

	/**
	* Retrieves the width and height that this FlashControl was created with.
	*
	* @param[out]	width	The unsigned short that will be used to store the retrieved width.
	*
	* @param[out]	height	The unsigned short that will be used to store the retrieved height.
	*/
	void getExtents(unsigned short &width, unsigned short &height) const;

	/**
	* Gets the UV scale of this FlashControl's internal texture. On certain systems we must compensate for lack of
	* NPOT-support on the videocard by using the next-highest POT texture. Normally, FlashControl overlays compensate 
	* their texture coordinates automatically however FlashControls created as pure materials will need to adjust 
	* their own by use of this function.
	*
	* @param[out]	uScale	The Ogre::Real that will be used to store the retrieved U-scale.
	* @param[out]	vScale	The Ogre::Real that will be used to store the retrieved V-scale.
	*/
	void getUVScale(float &uScale, float &vScale) const;

	/**
	* Injects a mouse-move event into this FlashControl (in the control's local coordinate-space).
	*
	* @param	xPos	The local X-coordinate.
	* @param	yPos	The local Y-coordinate.
	*/
	void injectMouseMove(int xPos, int yPos);

	/**
	* Injects a mouse-down event into this FlashControl (in the control's local coordinate-space).
	*
	* @param	xPos	The local X-coordinate.
	* @param	yPos	The local Y-coordinate.
	*/
	void injectMouseDown(int xPos, int yPos);

	/**
	* Injects a mouse-up event into this FlashControl (in the control's local coordinate-space).
	*
	* @param	xPos	The local X-coordinate.
	* @param	yPos	The local Y-coordinate.
	*/
	void injectMouseUp(int xPos, int yPos);

	/**
	* Injects a mouse-wheel event into this FlashControl (in the control's local coordinate-space).
	*
	* @param	relScroll	The relative scroll amount of the mouse-wheel.
	* @param	xPos	The local X-coordinate of the mouse.
	* @param	yPos	The local Y-coordinate of the mouse.
	*/
	void injectMouseWheel(int relScroll, int xPos, int yPos);

};

#endif // ATOM3D_FLASH_FLASHCONTROL_H
