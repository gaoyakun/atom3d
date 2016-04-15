#ifndef __ATOM3D_STUDIO_TIMELINE_H
#define __ATOM3D_STUDIO_TIMELINE_H

#include "editor.h"

#define ID_TIMELINE				(PLUGIN_ID_START + 100)
#define ID_TIMELINE_PLAY		(PLUGIN_ID_START + 101)
#define ID_TIMELINE_STOP		(PLUGIN_ID_START + 102)
#define ID_TIMELINE_DURATION	(PLUGIN_ID_START + 103)
#define ID_TIMELINE_CURRENT		(PLUGIN_ID_START + 104)

#define TRANSFORMMODE_NONE		0
#define TRANSFORMMODE_TRANSLATE	1
#define TRANSFORMMODE_SCALE		2
#define TRANSFORMMODE_ROTATION	3

class TimeChangeEvent: public ATOM_Event
{
public:
	unsigned time;

	ATOM_DECLARE_EVENT(TimeChangeEvent)
};

class TimeLine: public ATOM_EventTrigger
{
public:
	TimeLine (ATOM_Widget *parent, const ATOM_Rect2Di &rect, int style, int id, unsigned duration);
	~TimeLine (void);

public:
	unsigned getTimeSliceIndex (unsigned time) const;
	bool isKeyFrame (unsigned timeSliceIndex) const;
	void addKeyFrame (unsigned timeSliceIndex);
	void addKeyFrame (unsigned timeSliceIndex, ATOM_CompositionKeyFrameValue *keyFrameValue);
	void removeKeyFrame (unsigned timeSliceIndex);
	void removeAllKeyFrames (void);
	unsigned getNumKeyFrames (void) const;
	unsigned getKeyFrameSlice (unsigned keyFrameIndex) const;
	void resize (const ATOM_Rect2Di &rect);
	void setDuration (unsigned duration);
	unsigned getDuration (void) const;
	void setTimeSlice (unsigned timeSlice);
	unsigned getTimeSlice (void) const;
	void setActor (ATOM_CompositionActor *actor);
	ATOM_CompositionActor *getActor (void) const;
	unsigned getCurrentSlice (void) const;
	unsigned sliceIndexToTime (unsigned timeSliceIndex) const;
	unsigned timeToSliceIndex (unsigned time) const;
	void setCurrentSlice (unsigned index, ATOM_CompositionNode *node);
	void updateKeyFrame (int transformMode);
	void show (bool b);
	ATOM_CompositionKeyFrameValue *getKeyFrameValue (void) const;

public:
	void onPaint (ATOM_WidgetDrawClientEvent *event);
	void onLButtonDown (ATOM_WidgetLButtonDownEvent *event);
	void onDragStart (ATOM_WidgetDragStartEvent *event);
	void onDragDrop (ATOM_WidgetDragDropEvent *event);

private:
	ATOM_Panel *_panel;
	ATOM_Label *_timeLine;
	ATOM_Button *_btnPlay;
	ATOM_Button *_btnStop;
	ATOM_Edit *_editDuration;
	ATOM_Edit *_editCurrent;
	ATOM_AUTOPTR(ATOM_GUIImageList) _imageList;
	unsigned _duration;
	unsigned _timeSlice;
	unsigned _currentSlice;
	ATOM_AUTOPTR(ATOM_CompositionActor) _actor;
	ATOM_AUTOPTR(ATOM_CompositionKeyFrameValue) _keyFrameValue;
	ATOM_VECTOR<unsigned> _keyFrames;
	ATOM_DECLARE_EVENT_MAP(TimeLine, ATOM_EventTrigger)
};

#endif // __ATOM3D_STUDIO_TIMELINE_H
