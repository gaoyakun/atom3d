#ifndef __ATOM3D_STUDIO_MODEL_PROP_EDITOR_H
#define __ATOM3D_STUDIO_MODEL_PROP_EDITOR_H

#if _MSC_VER > 1000
# pragma once
#endif

class AS_Editor;
class ModelPropCommandCallback;
class MyValueChangeCallback;

class ModelPropEditor
{
public:
	ModelPropEditor (AS_Editor *editor);
	~ModelPropEditor (void);

public:
	void setPosition (int x, int y, int w, int h);
	void show (bool b);
	bool isShown (void) const;
	void refresh (void);
	void setModel (ATOM_Geode *model);
	ATOMX_TweakBar *getBar (void) const;
	AS_Editor *getEditor (void) const;
	void setParamChanged (bool b);
	void updateTime (void);
	void setCurrentMesh (int currentMesh);
	int getCurrentMesh (void) const;
	ATOM_Geode *getModel (void) const;
	void setModified (bool b);
	bool isModified (void) const;
	void setReadonly (bool readonly);

private:
	void createBar (void);
	void setupBar (void);
	void newParameter (const char *name, ATOM_ParameterTable::ValueHandle value);
	void newFloatParameter (const char *name, float value);
	void newIntParameter (const char *name, int value);
	void newVectorParameter (const char *name, const ATOM_Vector4f &value);
	void newColorParameter (const char *name, const ATOM_Vector4f &value);
	void newDirectionParameter (const char *name, const ATOM_Vector4f &value);
	void newBoolParameter (const char *name, bool value);
	void newEnumParameter (const char *name, const ATOM_VECTOR<ATOM_STRING> &enumNames, const ATOM_VECTOR<int> &enumValues, int value);
	void newTextureParameter (const char *name, const char *filename);

private:
	ATOM_AUTOREF(ATOM_Geode) _model;
	AS_Editor *_editor;
	ATOMX_TweakBar *_bar;
	MyValueChangeCallback *_valueCallback;
	int _currentMesh;
	bool _isShown;
	bool _modified;
	ModelPropCommandCallback *_callback;
};

class ModelPropCommandCallback: public ATOMX_TweakBar::CommandCallback
{
public:
	ModelPropEditor *_editor;

	ModelPropCommandCallback (ModelPropEditor *editor): _editor(editor)
	{
	}

	void callback (ATOMX_TWCommandEvent *event);
};

#endif // __ATOM3D_STUDIO_MODEL_PROP_EDITOR_H
