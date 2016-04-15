#ifndef __ATOM3D_STUDIO_COLORGRADING_EDITOR_H
#define __ATOM3D_STUDIO_COLORGRADING_EDITOR_H

class EditorImpl;
class CGETrigger;

class ColorGradingEditor: public ATOM_EventTrigger
{
	static const int Channel_None = -1;
	static const int Channel_RGB = 0;
	static const int Channel_R = 1;
	static const int Channel_G = 2;
	static const int Channel_B = 3;

public:
	ColorGradingEditor (EditorImpl *editorImpl, ATOM_ColorGradingEffect *effect);
	~ColorGradingEditor (void);

public:
	bool showEditor (void);
	void hideEditor (void);
	void setChannel (int channel);
	int getChannel (void) const;
	void setWaveType (ATOM_WaveType waveType);
	ATOM_WaveType getWaveType (void) const;
	void setTextureDimension (int dim);
	int getTextureDimension (void) const;
	void enableLinearFilter (bool enable);
	bool isLinearFilterEnabled (void) const;
	bool saveToFile (const char *filename);
	bool loadFromFile (const char *filename);
	bool exportTexture (const char *filename, int dim);
	bool updateColorGradingTexture (const ATOM_FloatAnimationCurve &redCurve, const ATOM_FloatAnimationCurve &greenCurve, const ATOM_FloatAnimationCurve &blueCurve, int dim);
	ATOM_AUTOREF(ATOM_Image) createColorGradingLUTImage (unsigned w, unsigned h, unsigned d);

public:
	void onCommand (ATOM_WidgetCommandEvent *event);
	void onClose (ATOM_TopWindowCloseEvent *event);
	void onCurveChanged (ATOM_CurveChangedEvent *event);

public:
	ATOM_AUTOREF(ATOM_Image) _identityTransformImage;
	ATOM_AUTOREF(ATOM_Texture) _colorGradingTexture;
	ATOM_AUTOREF(ATOM_ColorGradingEffect) _colorGradingEffect;
	ATOM_Dialog *_dialog;
	ATOM_MenuBar *_menuBar;
	ATOM_CurveEditor *_curveEditor;
	EditorImpl *_editorImpl;
	ATOM_FloatAnimationCurve _rgbCurve;
	ATOM_FloatAnimationCurve _redCurve;
	ATOM_FloatAnimationCurve _greenCurve;
	ATOM_FloatAnimationCurve _blueCurve;
	int _textureDimension;
	bool _linearFilter;
	int _currentChannel;
	ATOM_WaveType _rgbWave;
	ATOM_WaveType _redWave;
	ATOM_WaveType _greenWave;
	ATOM_WaveType _blueWave;

	ATOM_DECLARE_EVENT_MAP(ColorGradingEditor, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_COLORGRADING_EDITOR_H
