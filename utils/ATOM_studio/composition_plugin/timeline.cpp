#include "StdAfx.h"
#include "timeline.h"

ATOM_BEGIN_EVENT_MAP(TimeLine, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(TimeLine, ATOM_WidgetDrawClientEvent, onPaint)
	ATOM_EVENT_HANDLER(TimeLine, ATOM_WidgetLButtonDownEvent, onLButtonDown)
	ATOM_EVENT_HANDLER(TimeLine, ATOM_WidgetDragStartEvent, onDragStart)
	ATOM_EVENT_HANDLER(TimeLine, ATOM_WidgetDragDropEvent, onDragDrop)
ATOM_END_EVENT_MAP

static unsigned totalDuration = 10 * 1000;
static const unsigned dragImageId = 1223;

TimeLine::TimeLine (ATOM_Widget *parent, const ATOM_Rect2Di &rect, int style, int id, unsigned duration)
{
	_duration = duration;
	_timeSlice = 30;
	_currentSlice = 0;
	_imageList = ATOM_NEW(ATOM_GUIImageList);
	_imageList->newColorImage (dragImageId, ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f), ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f), ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f), ATOM_ColorARGB(1.f, 0.f, 0.f, 1.f));

	_panel = ATOM_NEW(ATOM_Panel, parent, rect, style&~ATOM_Widget::Border, ATOM_IMAGEID_CONTROL_BKGROUND, ATOM_Widget::ShowNormal);
	_panel->enableEventTransition (true);
	_panel->setLayoutType (ATOM_WidgetLayout::Horizontal);
	_panel->getLayout()->setInnerSpaceBottom (4);
	_panel->getLayout()->setInnerSpaceRight (4);
	_panel->getLayout()->setInnerSpaceTop (4);
	_panel->getLayout()->setInnerSpaceLeft (4);
	_panel->getLayout()->setGap (4);

	_btnPlay = ATOM_NEW(ATOM_Button, _panel, ATOM_Rect2Di(0, 0, 36, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_TIMELINE_PLAY, ATOM_Widget::ShowNormal);
	_btnPlay->setText ("²¥·Å");

	_btnStop = ATOM_NEW(ATOM_Button, _panel, ATOM_Rect2Di(0, 0, 36, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_TIMELINE_STOP, ATOM_Widget::ShowNormal);
	_btnStop->setText ("Í£Ö¹");

	_timeLine = ATOM_NEW(ATOM_Label, _panel, ATOM_Rect2Di(0, 0, -100, -100), ATOM_Widget::Control, id, ATOM_Widget::ShowNormal);
	_timeLine->enableDrag (true);
	_timeLine->enableDrop (true);
	_timeLine->setEventTrigger (this);
	_timeLine->setImageList (_imageList.get());

	char buffer[256];
	sprintf (buffer, "%d", _duration);
	_editDuration = ATOM_NEW(ATOM_Edit, _panel, ATOM_Rect2Di(0, 0, 50, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_TIMELINE_DURATION, ATOM_Widget::ShowNormal);
	_editDuration->setString (buffer);
	_editDuration->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	_editDuration->setBorderMode (ATOM_Widget::Drop);
	_editDuration->setEditType (EDITTYPE_NUMBER);

	_editCurrent = ATOM_NEW(ATOM_Edit, _panel, ATOM_Rect2Di(0, 0, 50, -100), ATOM_Widget::Border|ATOM_Widget::Control, ID_TIMELINE_CURRENT, ATOM_Widget::ShowNormal);
	_editCurrent->setClientImageId (ATOM_IMAGEID_WINDOW_BKGROUND);
	_editCurrent->setBorderMode (ATOM_Widget::Drop);
	_editCurrent->setEditType (EDITTYPE_NUMBER);
}

TimeLine::~TimeLine (void)
{
	_timeLine->setEventTrigger (0);
	ATOM_DELETE(_panel);
}

unsigned TimeLine::getTimeSliceIndex (unsigned time) const
{
	if (time >= _duration)
	{
		time = _duration - 1;
	}

	return time / _timeSlice;
}

void TimeLine::addKeyFrame (unsigned timeSliceIndex)
{
	if (_actor)
	{
		if (std::find (_keyFrames.begin(), _keyFrames.end(), timeSliceIndex) == _keyFrames.end ())
		{
			_keyFrames.push_back (timeSliceIndex);
			std::sort (_keyFrames.begin(), _keyFrames.end());

			if (_actor->getNumKeys() == 0)
			{
				_actor->captureKeyFrameValue (_keyFrameValue.get());
			}
			else
			{
				_actor->evalKeyFrameValue (sliceIndexToTime (timeSliceIndex), _keyFrameValue.get());
			}
			_actor->addKeyFrame (sliceIndexToTime (timeSliceIndex), _keyFrameValue.get());
		}
	}
}

void TimeLine::addKeyFrame (unsigned timeSliceIndex, ATOM_CompositionKeyFrameValue *keyFrameValue)
{
	if (_actor && keyFrameValue)
	{
		if (std::find (_keyFrames.begin(), _keyFrames.end(), timeSliceIndex) == _keyFrames.end ())
		{
			_keyFrames.push_back (timeSliceIndex);
			std::sort (_keyFrames.begin(), _keyFrames.end());

			_actor->addKeyFrame (sliceIndexToTime (timeSliceIndex), keyFrameValue);
		}
	}
}

void TimeLine::removeKeyFrame (unsigned timeSliceIndex)
{
	if (_actor)
	{
		ATOM_VECTOR<unsigned>::iterator it = std::find(_keyFrames.begin(), _keyFrames.end(), timeSliceIndex);
		if (it != _keyFrames.end ())
		{
			_keyFrames.erase (it);
			_actor->removeKeyFrame (sliceIndexToTime (timeSliceIndex));
		}
	}
}

void TimeLine::removeAllKeyFrames (void)
{
	_keyFrames.clear ();
}

unsigned TimeLine::getNumKeyFrames (void) const
{
	return _keyFrames.size();
}

unsigned TimeLine::getKeyFrameSlice (unsigned keyFrameIndex) const
{
	return _keyFrames[keyFrameIndex];
}

void TimeLine::setDuration (unsigned duration)
{
	if (_duration != duration)
	{
		_duration = duration;

		char buffer[256];
		sprintf (buffer, "%d", _duration);
		_editDuration->setString (buffer);
	}
}

unsigned TimeLine::getDuration (void) const
{
	return _duration;
}

void TimeLine::setTimeSlice (unsigned timeSlice)
{
	_timeSlice = timeSlice;
}

unsigned TimeLine::getTimeSlice (void) const
{
	return _timeSlice;
}

bool TimeLine::isKeyFrame (unsigned timeSliceIndex) const
{
	return std::find(_keyFrames.begin(), _keyFrames.end(), timeSliceIndex) != _keyFrames.end();
}

ATOM_CompositionActor *TimeLine::getActor (void) const
{
	return _actor.get();
}

void TimeLine::setActor (ATOM_CompositionActor *actor)
{
	if (actor != _actor.get())
	{
		_actor = actor;
		_keyFrames.clear ();

		if (actor)
		{
			for (unsigned i = 0; i < actor->getNumKeys(); ++i)
			{
				_keyFrames.push_back (timeToSliceIndex(actor->getKeyTime (i)));
			}
			_keyFrameValue = actor->createKeyFrameValue ();

			if (isKeyFrame (_currentSlice))
			{
				actor->evalKeyFrameValue (sliceIndexToTime (_currentSlice), _keyFrameValue.get());
			}
			else
			{
				actor->captureKeyFrameValue (_keyFrameValue.get());
			}
		}
		else
		{
			_keyFrameValue = 0;
		}
	}
}

void TimeLine::onLButtonDown (ATOM_WidgetLButtonDownEvent *event)
{
	unsigned numSlices = _duration / _timeSlice;
	unsigned width = _timeLine->getClientRect().size.w;
	unsigned sliceWidth = (width - numSlices - 1) / numSlices;
	unsigned space = (width - numSlices * (sliceWidth + 1) - 1) / 2;
	int x = event->x - space;
	int n = x / (1 + sliceWidth);
	if ( n < numSlices)
	{
		if (event->shiftState & KEYMOD_CTRL)
		{
			if (isKeyFrame (n))
			{
				removeKeyFrame (n);
			}
			else
			{
				addKeyFrame (n);
			}
		}
		else
		{
			setCurrentSlice (n, 0);
		}
	}
}

void TimeLine::setCurrentSlice (unsigned index, ATOM_CompositionNode *node)
{
	if (index != _currentSlice)
	{
		_currentSlice = index;

		if (_actor && _keyFrameValue)
		{
			if (isKeyFrame (_currentSlice))
			{
				_actor->evalKeyFrameValue (sliceIndexToTime (_currentSlice), _keyFrameValue.get());
			}
			else
			{
				_actor->captureKeyFrameValue (_keyFrameValue.get());
			}
		}

		TimeChangeEvent *event = ATOM_NEW(TimeChangeEvent);
		event->time = sliceIndexToTime(index);
		_panel->getParent()->queueEvent (event, ATOM_APP);
	}
}

void TimeLine::onDragStart (ATOM_WidgetDragStartEvent *event)
{
	unsigned numSlices = _duration / _timeSlice;
	unsigned width = _timeLine->getClientRect().size.w;
	unsigned sliceWidth = (width - numSlices - 1) / numSlices;
	unsigned space = (width - numSlices * (sliceWidth + 1) - 1) / 2;
	int x = event->x - space;
	int n = x / (1 + sliceWidth);
	if ( n < numSlices && isKeyFrame (n))
	{
		int sliceWidth = (_timeLine->getClientRect().size.w - numSlices - 1) / numSlices;
		int sliceHeight = _timeLine->getClientRect().size.h;
		event->dragSource->setIndicatorRect(ATOM_Rect2Di(-sliceWidth/2, -sliceHeight/2, sliceWidth, sliceHeight));
		event->dragSource->setIndicatorImageId(dragImageId);
		event->dragSource->addProperty ("Type", "KM");
		event->dragSource->addProperty ("SourceFrame", n);
	}
	else
	{
		event->allow = false;
	}
}

void TimeLine::onDragDrop (ATOM_WidgetDragDropEvent *event)
{
	if (event->dragSource->getProperty("Type") == "KM")
	{
		unsigned numSlices = _duration / _timeSlice;
		unsigned width = _timeLine->getClientRect().size.w;
		unsigned sliceWidth = (width - numSlices - 1) / numSlices;
		unsigned space = (width - numSlices * (sliceWidth + 1) - 1) / 2;
		int x = event->x - space;
		int n = x / (1 + sliceWidth);
		if ( n < numSlices && !isKeyFrame (n))
		{
			int source = event->dragSource->getProperty("SourceFrame").getI();

			ATOM_AUTOPTR(ATOM_CompositionKeyFrameValue) keyFrameValue = _actor->createKeyFrameValue ();
			_actor->evalKeyFrameValue (sliceIndexToTime (source), keyFrameValue.get());

			if ((event->keymod & KEYMOD_SHIFT) == 0)
			{
				removeKeyFrame (source);
			}
			addKeyFrame (n, keyFrameValue.get());
			setCurrentSlice (n, 0);
		}
	}
}

void TimeLine::onPaint (ATOM_WidgetDrawClientEvent *event)
{
	char buffer[256];
	sprintf (buffer, "%d", _currentSlice * _timeSlice);
	_editCurrent->setString (buffer);

	const ATOM_Size2Di &canvasSize = event->canvas->getSize();
	event->canvas->fillRect (ATOM_Rect2Di(ATOM_Point2Di(0, 0), canvasSize), ATOM_ColorARGB(0.3f, 0.3f, 0.3f, 1.f));
	unsigned numSlices = _duration / _timeSlice;
	if (numSlices == 0) numSlices = 1;
	unsigned sliceWidth = (canvasSize.w - numSlices - 1) / numSlices;
	unsigned space = (canvasSize.w - numSlices * (sliceWidth + 1) - 1) / 2;
	for (unsigned i = 0; i < numSlices; ++i)
	{
		int x = space + i * (sliceWidth + 1);
		int y = 1;
		int w = sliceWidth;
		int h = event->canvas->getSize().h - 2;

		ATOM_ColorARGB c;
		if (!getActor() || i * _timeSlice >= _duration)
		{
			c.setFloats (0.5f, 0.5f, 0.5f, 1.f);
		}
		else if (isKeyFrame (i))
		{
			c.setFloats (1.f, 0.f, 0.f, 1.f);
		}
		else
		{
			c.setFloats (1.f, 1.f, 1.f, 1.f);
		}
		event->canvas->fillRect (ATOM_Rect2Di(x, y, w, h), c);

		if (i == _currentSlice)
		{
			c.setFloats (0.f, 0.5f, 0.f, 1.f);
			event->canvas->drawLine (ATOM_Point2Di(x, y), ATOM_Point2Di(x+w-1, y), c);
			event->canvas->drawLine (ATOM_Point2Di(x+w-1, y), ATOM_Point2Di(x+w-1, y+h-1), c);
			event->canvas->drawLine (ATOM_Point2Di(x+w-1, y+h-1), ATOM_Point2Di(x, y+h-1), c);
			event->canvas->drawLine (ATOM_Point2Di(x, y+h-1), ATOM_Point2Di(x, y), c);
		}
	}

	setAutoCallHost (false);
}

void TimeLine::resize (const ATOM_Rect2Di &rect)
{
	_panel->resize (rect);
}

unsigned TimeLine::getCurrentSlice (void) const
{
	return _currentSlice;
}

unsigned TimeLine::sliceIndexToTime (unsigned timeSliceIndex) const
{
	return timeSliceIndex * _timeSlice;
}

unsigned TimeLine::timeToSliceIndex (unsigned time) const
{
	return time / _timeSlice;
}

void TimeLine::updateKeyFrame (int transformMode)
{
	if (_actor && isKeyFrame (_currentSlice))
	{
		ATOM_Vector3f t = _keyFrameValue->getTranslation();
		ATOM_Vector3f r = _keyFrameValue->getRotation();
		ATOM_Vector3f s = _keyFrameValue->getScale();
		_actor->captureKeyFrameValue (_keyFrameValue.get());

		switch (transformMode)
		{
		case TRANSFORMMODE_SCALE:
			{
#if 1
				_keyFrameValue->setTranslation (t);
				_keyFrameValue->setRotation (r);
#else
				s = _actor->getNode()->getScale();
#endif
				break;
			}
		case TRANSFORMMODE_TRANSLATE:
			{
#if 1
				_keyFrameValue->setScale (s);
				_keyFrameValue->setRotation (r);
#else
				t = _actor->getNode()->getTranslation();
#endif
				break;
			}
		case TRANSFORMMODE_ROTATION:
			{
#if 1
				_keyFrameValue->setTranslation (t);
				_keyFrameValue->setScale (s);
#else
				ATOM_Vector4f r = _actor->getNode()->getRotation ();
				ATOM_Quaternion q(r.x, r.y, r.z, r.w);
				r = q.toEulerXYZ();
#endif
				break;
			}
		default:
			{
				_keyFrameValue->setTranslation (t);
				_keyFrameValue->setScale (s);
				_keyFrameValue->setRotation (r);
				break;
			}
		}
		//_keyFrameValue->setTranslation (t);
		//_keyFrameValue->setScale (s);
		//_keyFrameValue->setRotation (r);
		_actor->addKeyFrame (sliceIndexToTime (_currentSlice), _keyFrameValue.get());
	}
}

ATOM_CompositionKeyFrameValue *TimeLine::getKeyFrameValue (void) const
{
	return _keyFrameValue.get();
}

void TimeLine::show (bool b)
{
	_panel->show (b ? ATOM_Widget::ShowNormal : ATOM_Widget::Hide);
}

