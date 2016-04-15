#include "StdAfx.h"
#include "ribbontrail.h"

ATOM_SCRIPT_INTERFACE_BEGIN(ATOM_RibbonTrail)
	ATOM_ATTRIBUTES_BEGIN(ATOM_RibbonTrail)
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_RibbonTrail, "StartColor", getStartColor, setStartColor, ATOM_Vector4f(1.f), "group=ATOM_RibbonTrail;type=rgba;desc='起始颜色'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_RibbonTrail, "EndColor", getEndColor, setEndColor, ATOM_Vector4f(0.f), "group=ATOM_RibbonTrail;type=rgba;desc='结束颜色'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_RibbonTrail, "StartWidth", getStartWidth, setStartWidth, 10.f, "group=ATOM_RibbonTrail;desc='起始宽度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_RibbonTrail, "EndWidth", getEndWidth, setEndWidth, 0.f, "group=ATOM_RibbonTrail;desc='结束宽度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_RibbonTrail, "Length", getLength, setLength, 100.f, "group=ATOM_RibbonTrail;desc='长度'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_RibbonTrail, "NumSegments", getNumSegments, setNumSegments, 20, "group=ATOM_RibbonTrail;desc='段数'")
		ATOM_ATTRIBUTE_PERSISTENT(ATOM_RibbonTrail, "CollapseTime", getCollapseTime, setCollapseTime, 0.1f, "group=ATOM_RibbonTrail;desc='回缩时间'")
	ATOM_ATTRIBUTES_END()
ATOM_SCRIPT_INTERFACE_END(ATOM_RibbonTrail, ATOM_LineTrail)

ATOM_IMPLEMENT_NODE_FILE_TYPE(ATOM_RibbonTrail)

ATOM_RibbonTrail::ATOM_RibbonTrail (void)
{
	ATOM_STACK_TRACE(ATOM_RibbonTrail::ATOM_RibbonTrail);
	_colorStart.set(1.f, 1.f, 1.f, 1.f);
	_colorEnd.set(0.f, 0.f, 0.f, 0.f);
	_widthStart = 10.f;
	_widthEnd = 0.f;
	_trailLengthDesired = 100.f;
	_numSegments = 20;
	_minSegmentLength = 5.f;
	_needUpdate = false;
	_starttick = 0;
	_targetHitTimeDesired = 0.f;
	_targetHitTime = 0.f;
	_collapseTimeDesired = 0.1f;
	_collapseTime = 0.f;
	_controller = 0;
}

ATOM_RibbonTrail::~ATOM_RibbonTrail (void)
{
	ATOM_STACK_TRACE(ATOM_RibbonTrail::~ATOM_RibbonTrail);
}

void ATOM_RibbonTrail::reset (void)
{
	ATOM_STACK_TRACE(ATOM_RibbonTrail::reset);
	clearInflectionPoints ();
	_starttick = 0;
	_needUpdate = true;
	_targetHitTime = 0.f;
	_collapseTime = 0.f;
}

void ATOM_RibbonTrail::setLength (float length)
{
	ATOM_STACK_TRACE(ATOM_RibbonTrail::setLength);
	if (length != _trailLengthDesired)
	{
		_trailLengthDesired = length;
		_needUpdate = true;
	}
}

float ATOM_RibbonTrail::getLength (void) const
{
	return _trailLengthDesired;
}

void ATOM_RibbonTrail::setNumSegments (unsigned numSegments)
{
	if (numSegments != _numSegments)
	{
		_numSegments = numSegments;
		_needUpdate = true;
	}
}

unsigned ATOM_RibbonTrail::getNumSegments (void) const
{
	return _numSegments;
}

void ATOM_RibbonTrail::setTarget (ATOM_Node *target, float timeInSecond)
{
	_target = target;
	_targetHitTimeDesired = timeInSecond;
	_targetHitTime = 0;
}

void ATOM_RibbonTrail::setController (ATOM_RibbonTrail::Controller *controller)
{
	_controller = controller;
}

ATOM_RibbonTrail::Controller *ATOM_RibbonTrail::getController (void) const
{
	return _controller;
}

void ATOM_RibbonTrail::setCollapseTime (float timeInSecond)
{
	_collapseTimeDesired = timeInSecond;
	_collapseTime = 0.f;
}

float ATOM_RibbonTrail::getCollapseTime (void) const
{
	return _collapseTimeDesired;
}

void ATOM_RibbonTrail::computePointParams (ATOM_LineTrail::InflectionPoint *p, float pos) const
{
	ATOM_Vector4f rgba = _colorStart + (_colorEnd - _colorStart) * pos;
	p->color.setFloats (rgba.x, rgba.y, rgba.z, rgba.w);
	p->width = _widthStart + (_widthEnd - _widthStart) * pos;
}

void ATOM_RibbonTrail::update (ATOM_Camera *camera)
{
	ATOM_STACK_TRACE(ATOM_RibbonTrail::update);
	if (_starttick == 0)
	{
		_starttick = ATOM_APP->getFrameStamp().currentTick;
	}
	else
	{
		updateInflectionPoints (camera, ATOM_APP->getFrameStamp().elapsedTick * 0.001f);
	}
}

void ATOM_RibbonTrail::updateInflectionPoints (ATOM_Camera *camera, float timeElapsedInSecond)
{
	ATOM_STACK_TRACE(ATOM_RibbonTrail::updateInflectionPoints);
	if (!_target)
	{
		return;
	}

	unsigned numPoints = getNumInflectionPoints ();

	bool reached = _targetHitTime == _targetHitTimeDesired;

	if (!reached)
	{
		// 更新目标的当前位置到targetPos
		ATOM_Vector3f targetPos =  _target->getWorldMatrix().getRow3 (3);

		// 更新跟踪目标的时间
		_targetHitTime += timeElapsedInSecond;
		if (_targetHitTime >= _targetHitTimeDesired)
		{
			// 时间到
			_targetHitTime = _targetHitTimeDesired;
		}
		else
		{
			// 时间没到
			// 计算当前运行位置到originPos
			ATOM_Vector3f originPos = (numPoints == 0) ? getWorldMatrix().getRow3(3) : inflectionPoint(0).position;
			// 计算还需多久击中目标
			float timeRemained = _targetHitTimeDesired - _targetHitTime;
			// 计算这次运行距离的比例
			float t = ATOM_saturate (timeElapsedInSecond/timeRemained);
			// 计算此次运行位置
			targetPos = originPos + (targetPos - originPos) * t;
		}

		if (numPoints < 2)
		{
			// 如果是第1次或第2次更新，增加一个拐点
			InflectionPoint p;
			p.position = targetPos;
			insertInflectionPoint (0, p);
			numPoints++;
		}
		else
		{
			// 否则根据最小线段长度来更新拐点
			ATOM_Vector3f &pos0 = inflectionPoint (0).position;
			ATOM_Vector3f &pos1 = inflectionPoint (1).position;
			float d0 = (pos0 - pos1).getLength();
			float d1 = (targetPos - pos0).getLength();
			if (d0 + d1 < _minSegmentLength || d1 < _minSegmentLength * 0.5f)
			{
				// 直接修改首个拐点位置
				pos0 = targetPos;
			}
			else
			{

				// 需要增加一个拐点
				InflectionPoint p;
				p.position = targetPos;
				insertInflectionPoint (0, p);
				numPoints++;
			}
		}
	}

	// 获取整个长度，确定是否需要收缩
	float trailLength = getTrailLength ();
	float collapseLength = 0.f;
	if (reached)
	{
		numPoints = getNumInflectionPoints ();
		if (numPoints > 1 && _collapseTimeDesired > 0.f)
		{
			_collapseTime += timeElapsedInSecond;
			if (_collapseTime >= _collapseTimeDesired)
			{
				collapseLength = trailLength;
			}
			else
			{
				float collapseTimeRemained = _collapseTimeDesired - _collapseTime;
				float t = ATOM_saturate (timeElapsedInSecond / collapseTimeRemained);
				collapseLength = trailLength * t;
			}
		}
	}
	else if (trailLength > _trailLengthDesired)
	{
		collapseLength = trailLength - _trailLengthDesired;
	}

	if (collapseLength > trailLength)
	{
		setNumInflectionPoints (1);
		trailLength = 0.f;
	}
	else if (collapseLength > 0.f)
	{
		trailLength -= collapseLength;
		for (;;)
		{
			if (numPoints < 2)
			{
				break;
			}
			float segmentLength = getSegmentLength (numPoints - 2);
			if (segmentLength <= collapseLength)
			{
				collapseLength -= segmentLength;
				removeInflectionPoint (numPoints - 1);
				--numPoints;
			}
			else
			{
				collapseLength = segmentLength - collapseLength;
				break;
			}
		}
		if (numPoints > 1)
		{
			InflectionPoint &p0 = inflectionPoint(numPoints - 2);
			InflectionPoint &p1 = inflectionPoint(numPoints - 1);
			ATOM_Vector3f v = p0.position - p1.position;
			v.normalize ();
			p1.position = p0.position + v * collapseLength;
			invalidLength (numPoints-1);
		}
	}

	// update color, alpha and width
	trailLength = getTrailLength ();
	if (trailLength > 0.f)
	{
		float pos = 0.f;
		unsigned n = getNumInflectionPoints ();
		for (unsigned i = 0; i < n; ++i)
		{
			computePointParams (&inflectionPoint (i), pos/trailLength);
			pos += getSegmentLength (i);
		}
	}
}

void ATOM_RibbonTrail::setStartColor (const ATOM_Vector4f &color)
{
	_colorStart = color;
}

const ATOM_Vector4f &ATOM_RibbonTrail::getStartColor (void) const
{
	return _colorStart;
}

void ATOM_RibbonTrail::setEndColor (const ATOM_Vector4f &color)
{
	_colorEnd = color;
}

const ATOM_Vector4f &ATOM_RibbonTrail::getEndColor (void) const
{
	return _colorEnd;
}

void ATOM_RibbonTrail::setStartWidth (float val)
{
	_widthStart = val;
}

float ATOM_RibbonTrail::getStartWidth (void) const
{
	return _widthStart;
}

void ATOM_RibbonTrail::setEndWidth (float val)
{
	_widthEnd = val;
}

float ATOM_RibbonTrail::getEndWidth (void) const
{
	return _widthEnd;
}

void ATOM_RibbonTrail::assign(ATOM_Node *other) const
{
	ATOM_LineTrail::assign (other);
	ATOM_RibbonTrail *otherTrail = dynamic_cast<ATOM_RibbonTrail*>(other);
	if (otherTrail)
	{
		otherTrail->setStartColor (_colorStart);
		otherTrail->setEndColor (_colorEnd);
		otherTrail->setStartWidth (_widthStart);
		otherTrail->setEndWidth (_widthEnd);
		otherTrail->setLength (_trailLengthDesired);
		otherTrail->setNumSegments (_numSegments);
		otherTrail->setTarget (_target.get(), _targetHitTimeDesired);
		otherTrail->setCollapseTime (_collapseTimeDesired);
	}
}

bool ATOM_RibbonTrail::onLoad(ATOM_RenderDevice *device)
{
	ATOM_STACK_TRACE(ATOM_RibbonTrail::onLoad);
	if (!getNodeFileName().empty ())
	{
		ATOM_AutoFile f(getNodeFileName().c_str(), ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			return false;
		}

		unsigned size = f->size ();
		char *p = ATOM_NEW_ARRAY(char, size);
		size = f->read (p, size);
		p[size] = '\0';

		ATOM_TiXmlDocument doc;
		doc.Parse (p);
		ATOM_DELETE_ARRAY(p);
		if (doc.Error ())
		{
			ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, device, doc.ErrorDesc());
			return false;
		}

		TiXmlElement *element = doc.RootElement();
		if (!loadAttribute (element))
		{
			return false;
		}
	}

	return ATOM_LineTrail::onLoad (device);
}

bool ATOM_RibbonTrail::save (const char *filename)
{
	ATOM_TiXmlDocument doc;

	ATOM_TiXmlElement eRoot("node");
	if (!writeAttribute (&eRoot))
	{
		return false;
	}

	doc.InsertEndChild (eRoot);

	char nativeFileName[ATOM_VFS::max_filename_length];
	if (!ATOM_GetNativePathName (filename, nativeFileName))
	{
		return false;
	}

	return doc.SaveFile (nativeFileName);
}

