#ifndef __ATOMX_TWBAR_H
#define __ATOMX_TWBAR_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "basedefs.h"

enum ATOMX_TBType
{
	ATOMX_TBTYPE_UNKNOWN		= 0,
	ATOMX_TBTYPE_BOOL,
	ATOMX_TBTYPE_CHAR,
	ATOMX_TBTYPE_INT8,
	ATOMX_TBTYPE_UINT8,
	ATOMX_TBTYPE_INT16,
	ATOMX_TBTYPE_UINT16,
	ATOMX_TBTYPE_INT32,
	ATOMX_TBTYPE_UINT32,
	ATOMX_TBTYPE_FLOAT,
	ATOMX_TBTYPE_COLOR3F,
	ATOMX_TBTYPE_COLOR4F,
	ATOMX_TBTYPE_STRING,
	ATOMX_TBTYPE_QUAT4F,
	ATOMX_TBTYPE_DIR3F,
	ATOMX_TBTYPE_VECTOR2F,
	ATOMX_TBTYPE_VECTOR3F,
	ATOMX_TBTYPE_VECTOR4F
};

class ATOMX_API ATOMX_TBValue
{
	ATOMX_TBType _type;
	union
	{
		int _b;
		char _c;
		char _i8;
		unsigned char _ui8;
		short _i16;
		unsigned short _ui16;
		int _i32;
		unsigned int _ui32;
		float _f;
		float _c3f[3];
		float _c4f[4];
		char *_s;
		float _q4f[4];
		float _d3f[3];
		float _v2f[3];
		float _v3f[3];
		float _v4f[4];

		float _dummy[4];
	};

public:
	ATOMX_TBValue (void);
	ATOMX_TBValue (ATOMX_TBType type);
	ATOMX_TBValue (const ATOMX_TBValue &rhs);
	void swap (ATOMX_TBValue &rhs);
	ATOMX_TBValue & operator = (const ATOMX_TBValue &rhs);
	~ATOMX_TBValue (void);

public:
	void clear (void);
	ATOMX_TBType getType (void) const;
	void setI (ATOMX_TBType type, int value);
	void setU (ATOMX_TBType type, unsigned value);
	void setB (ATOMX_TBType type, bool b);
	void setF (ATOMX_TBType type, float f);
	void set2F (ATOMX_TBType type, float a, float b);
	void set3F (ATOMX_TBType type, float a, float b, float c);
	void set4F (ATOMX_TBType type, float a, float b, float c, float d);
	void setS (ATOMX_TBType type, const char *str);
	int getI (void) const;
	unsigned getU (void) const;
	bool getB (void) const;
	float getF (void) const;
	const float *get2F (void) const;
	const float *get3F (void) const;
	const float *get4F (void) const;
	const char *getS (void) const;
};

class ATOMX_API ATOMX_TBEnum
{
public:
	ATOMX_TBEnum (void);

public:
	void addEnum (const char *name, int value);
	void removeEnum (int value);
	bool isValidEnum (int value) const;
	void clear (void);
	unsigned getNumEnums (void) const;
	int getEnumValue (unsigned index) const;
	const char *getEnumName (unsigned index) const;

private:
	int findEnum (int value) const;

	struct EnumValue
	{
		ATOM_STRING name;
		int value;
	};
	ATOM_VECTOR<EnumValue> _values;
};

class ATOMX_TWValueChangedEvent;
class ATOMX_TWCommandEvent;

class ATOMX_API ATOMX_TweakBar: private ATOM_Noncopyable
{
public:
	class ValueChangedCallback
	{
	public:
		virtual void callback (ATOMX_TWValueChangedEvent *event) = 0;
	};

	class CommandCallback
	{
	public:
		virtual void callback (ATOMX_TWCommandEvent *event) = 0;
	};

	typedef void (ATOMX_CALL *PFNReadFromBarFunc) (const char *name, const ATOMX_TBValue &value, void *userData);
	typedef void (ATOMX_CALL *PFNWriteToBarFunc) (const char *name, ATOMX_TBValue &value, void *userData);
	enum VarType
	{
		VT_BASE,
		VT_VAR
	};

	class VariableBase
	{
	public:
		ATOM_STRING name;
		ATOMX_TweakBar *bar;
		void *userdata;
		int id;
		int type;

		VariableBase (ATOMX_TweakBar *bar_): bar(bar_), id(0), type(VT_BASE), userdata(0) {}
		virtual ~VariableBase (void) {}
	};

	class Variable: public VariableBase
	{
	public:
		Variable (void);

		ATOMX_TBValue value;
		PFNReadFromBarFunc readFromFunc;
		void *userDataReadFrom;
		PFNWriteToBarFunc writeToFunc;
		void *userDataWriteTo;
		Variable (ATOMX_TweakBar *bar_): VariableBase(bar_), readFromFunc(0), writeToFunc(0) {type = VT_VAR;}
	};

	typedef void (ATOMX_CALL *PFNButtonCallback) (void *userData);

	struct ButtonCallbackInfo
	{
		PFNButtonCallback callback;
		void *userdata;
	};

public:
	ATOMX_TweakBar (const char *name);
	virtual ~ATOMX_TweakBar (void);

public:
	virtual void clear (void);
	
public:
	bool addImmediateVariable (const char *name, ATOMX_TBType varType, void *valueAddr, bool readonly, const char *group);
	bool addVariable (const char *name, int id, const ATOMX_TBValue &var, bool readonly, const char *group, void *userData = 0);
	bool addIntVar (const char *name, int id, int value, bool readonly, const char *group, void *userData = 0);
	bool addUIntVar (const char *name, int id, unsigned int value, bool readonly, const char *group, void *userData = 0);
	bool addShortVar (const char *name, int id, short value, bool readonly, const char *group, void *userData = 0);
	bool addUShortVar (const char *name, int id, unsigned short value, bool readonly, const char *group, void *userData = 0);
	bool addByteVar (const char *name, int id, char value, bool readonly, const char *group, void *userData = 0);
	bool addUByteVar (const char *name, int id, unsigned char value, bool readonly, const char *group, void *userData = 0);
	bool addCharVar (const char *name, int id, char value, bool readonly, const char *group, void *userData = 0);
	bool addBoolVar (const char *name, int id, bool value, bool readonly, const char *group, void *userData = 0);
	bool addFloatVar (const char *name, int id, float value, bool readonly, const char *group, void *userData = 0);
	bool addStringVar (const char *name, int id, const char *value, bool readonly, const char *group, void *userData = 0);
	bool addRGBVar (const char *name, int id, float r, float g, float b, bool readonly, const char *group, void *userData = 0);
	bool addRGBAVar (const char *name, int id, float r, float g, float b, float a, bool readonly, const char *group, void *userData = 0);
	bool addQuatVar (const char *name, int id, float x, float y, float z, float w, bool readonly, const char *group, void *userData = 0);
	bool addDirVar (const char *name, int id, float x, float y, float z, bool readonly, const char *group, void *userData = 0);
	bool addVector2fVar (const char *name, int id, float x, float y, bool readonly, const char *group, void *userData = 0);
	bool addVector2fVar (const char *name, int id, const ATOM_Vector2f &v, bool readonly, const char *group, void *userData = 0);
	bool addVector3fVar (const char *name, int id, float x, float y, float z, bool readonly, const char *group, void *userData = 0);
	bool addVector3fVar (const char *name, int id, const ATOM_Vector3f &v, bool readonly, const char *group, void *userData = 0);
	bool addVector4fVar (const char *name, int id, float x, float y, float z, float w, bool readonly, const char *group, void *userData = 0);
	bool addVector4fVar (const char *name, int id, const ATOM_Vector4f &v, bool readonly, const char *group, void *userData = 0);
	bool addEnum (const char *name, int id, int value, const ATOMX_TBEnum &var, bool readonly, const char *group, void *userData = 0);
	bool addEnum (const char *name, int id, int value, bool readonly, const char *group, void *userData, ...);
	bool addButton (const char *name, int id, const char *label, const char *group, void *userData = 0);
	bool addButton (const char *name, const char *label, const char *group, PFNButtonCallback callback, void *userdata);
	bool addText (const char *name, const char *label, const char *group);
	bool addSeparator (const char *name, const char *group);
	void remove (const char *name);
	void setI (const char *name, int value, bool post=true);
	void setU (const char *name, unsigned value, bool post=true);
	void setB (const char *name, bool value, bool post=true);
	void setF (const char *name, float value, bool post=true);
	void setS (const char *name, const char *value, bool post=true);
	void set2F (const char *name, const ATOM_Vector2f &value, bool post=true);
	void set3F (const char *name, const ATOM_Vector3f &value, bool post=true);
	void set4F (const char *name, const ATOM_Vector4f &value, bool post=true);
	int  getI (const char *name) const;
	unsigned getU (const char *name) const;
	bool getB (const char *name) const;
	float getF (const char *name) const;
	const char *getS (const char *name) const;
	ATOM_Vector3f get3F (const char *name) const;
	ATOM_Vector4f get4F (const char *name) const;
	void setVarUserData (const char *name, void *userdata);
	void setVarLabel (const char *name, const char *label);
	void setVarHelp (const char *name, const char *msg);
	void setVarGroup (const char *name, const char *group);
	void setVarVisible (const char *name, bool visible);
	void setVarMinMax (const char *name, float minvalue, float maxvalue);
	void setVarStep (const char *name, float step);
	void setVarPrecision (const char *name, int precision);
	void setVarHex (const char *name, bool hex);
	void setVarBoolString (const char *name, const char *trueStr, const char *falseStr);
	void setVarReadFromBarFunc (const char *name, PFNReadFromBarFunc func, void *userData);
	void setVarWriteToBarFunc (const char *name, PFNWriteToBarFunc func, void *userData);
	void setGroupOpenState (const char *name, bool opened);
	void setBarColor (float r, float g, float b, float a);
	void setBarPosition (int x, int y);
	void setBarSize (int w, int h);
	void setBarLabel (const char *label);
	void setBarVisible (bool visible);
	void setBarIconifiable (bool b);
	void setBarMovable (bool b);
	void setBarResizable (bool b);
	void setBarTopMost (bool b);
	void setBarBottomMost (bool b);
	void setBarFontResizable (bool b);
	void setBarRefreshPeriod (float sec);
	void setTop (void);
	void setBottom (void);
	bool isTop (void) const;
	bool isBottom (void) const;
	const char *getName (void) const;
	void refresh (void);
	bool isNotifyingEnabled (void) const;
	void enableNotifying (bool enable);
	void setValueChangedCallback (ValueChangedCallback *callback);
	ValueChangedCallback *getValueChangedCallback (void) const;
	void setCommandCallback (CommandCallback *callback);
	CommandCallback *getCommandCallback (void) const;

public:
	void onValueChanged (ATOMX_TWValueChangedEvent *event);
	void onCommand (ATOMX_TWCommandEvent *event);

public:
	static bool initialize (void *device, int width, int height);
	static void terminate (void);
	static void handleDeviceLost (void);
	static void handleDeviceReset (int width, int height);
	static void draw (void);
	static void setBarVisible (const char *barName, bool visible);
	static bool processWMEvent (ATOM_AppWMEvent *event);

private:
	static bool _initialized;
	void *_bar;
	bool _enableNotifying;
	ATOM_STRING _barName;
	ATOM_MAP<ATOM_STRING, VariableBase*> _variables;
	ATOM_VECTOR<ButtonCallbackInfo*> _buttonCallbacks;
	ValueChangedCallback *_valueChangedCallback;
	CommandCallback *_commandCallback;

	ATOM_DECLARE_EVENT_MAP_NOPARENT(ATOMX_TweakBar)
};

#endif // __ATOMX_TWBAR_H
