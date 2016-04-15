#ifndef __ATOM3D_ENGINE_COMPOSITION_TRACK_H
#define __ATOM3D_ENGINE_COMPOSITION_TRACK_H

#include "composition_keyframe.h"
#include "composition_io.h"

class ATOM_CompositionTrack: public ATOM_ReferenceObj
{
public:
	virtual void apply (long time, ATOM_Node *node) = 0;
	virtual unsigned getNumKeys (void) const = 0;
	virtual long getKeyTime (unsigned index) const = 0;
	virtual void saveToXML (ATOM_TiXmlElement *xml) const = 0;
	virtual void loadFromXML (const ATOM_TiXmlElement *xml) = 0;
};

template <class KeyFrameType>
class ATOM_CompositionTrackT: public ATOM_CompositionTrack
{
public:
	typedef typename KeyFrameType::value_type value_type;
	typedef typename KeyFrameType::linear_policy_type linear_policy_type;
	typedef KeyFrameType keyframe_type;

public:
	ATOM_CompositionTrackT (void)
	{
		setWaveType (ATOM_WAVE_TYPE_LINEAR);
		setAddress (ATOM_CURVE_ADDRESS_CLAMP);
		_defaultWaveType = ATOM_WAVE_TYPE_LINEAR;
	}

	ATOM_CompositionTrackT (ATOM_WaveType waveType)
	{
		setWaveType (waveType);
		setAddress (ATOM_CURVE_ADDRESS_CLAMP);
		_defaultWaveType = waveType;
	}

public:
	virtual void apply (long time, ATOM_Node *node)
	{
		if (_track.getNumKeys() > 0)
		{
			if (!_result)
			{
				_result = ATOM_NEW(keyframe_type);
			}

			_result->setValue (_track.eval (time));
			node->accept (*_result);
		}
	}

	virtual unsigned getNumKeys (void) const
	{
		return _track.getNumKeys ();
	}

	virtual long getKeyTime (unsigned index) const
	{
		return _track[index].x;
	}

	virtual void saveToXML (ATOM_TiXmlElement *xml) const
	{
		if (_defaultWaveType != _track.getWaveType())
		{
			xml->SetAttribute ("Wave", int(_track.getWaveType()));
		}

		for (unsigned i = 0; i < _track.getNumKeys(); ++i)
		{
			long time = _track[i].x;
			const value_type &value = _track[i].value();
			ATOM_TiXmlElement e("Key");
			KeyValueXMLWriter<value_type>()(time, value, &e);
			xml->InsertEndChild (e);
		}
	}

	virtual void loadFromXML (const ATOM_TiXmlElement *xml)
	{
		removeAllKeyFrames ();

		for (const ATOM_TiXmlElement *e = xml->FirstChildElement ("Key"); e; e = e->NextSiblingElement ("Key"))
		{
			long time;
			value_type value;
			KeyValueXMLLoader<value_type>()(time, value, e);

			ATOM_AUTOPTR(keyframe_type) keyFrame = ATOM_NEW(keyframe_type);
			keyFrame->setValue (value);

			insertKeyFrame (time, keyFrame.get());
		}

		int waveType;
		if (xml->QueryIntAttribute("Wave", &waveType) == ATOM_TIXML_SUCCESS)
		{
			setWaveType ((ATOM_WaveType)waveType);
		}
	}

public:
	unsigned insertKeyFrame (long time, keyframe_type *keyframe)
	{
		return _track.insertKey (time, keyframe);
	}

	bool getKeyValueByTime (long time, value_type &value) const
	{
		unsigned index = findKeyFrame (time);
		if (index != (unsigned)-1)
		{
			value = getKeyValueByIndex (index);
			return true;
		}
		return false;
	}

	const value_type &getKeyValueByIndex (unsigned index) const
	{
		return _track[index].value();
	}

	unsigned findKeyFrame (long time) const
	{
		return _track.findKey (time);
	}

	void removeKeyFrame (unsigned index)
	{
		_track.removeKey (index);
	}

	void removeAllKeyFrames (void)
	{
		_track.removeAllKeys ();
	}

	void setWaveType (ATOM_WaveType waveType)
	{
		_track.setWaveType (waveType);
	}

	ATOM_WaveType getWaveType (void) const
	{
		return _track.getWaveType ();
	}

	void setAddress (ATOM_CurveAddress address)
	{
		_track.setAddress (address);
	}

	ATOM_CurveAddress getAddress (void) const
	{
		return _track.getAddress ();
	}

	value_type evalSpline (long x) const
	{
		return _track.evalSpline (x);
	}

	value_type evalLinear (long x) const
	{
		return _track.evalLinear (x);
	}

	value_type evalStep (long x) const
	{
		return _track.evalStep (x);
	}

	value_type eval(long x) const
	{
		return _track.eval (x);
	}

private:
	ATOM_AnimationCurve<value_type, ATOM_AUTOPTR(keyframe_type), linear_policy_type> _track;
	ATOM_AUTOPTR(keyframe_type) _result;
	ATOM_WaveType _defaultWaveType;
};

#endif // __ATOM3D_ENGINE_COMPOSITION_TRACK_H
