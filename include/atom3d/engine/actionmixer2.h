#ifndef __ATOM3D_ENGINE_ACTIONMIXER2_H
#define __ATOM3D_ENGINE_ACTIONMIXER2_H

#if _MSC_VER > 1000
# pragma once
#endif

#include "joint_animation_track.h"

class ATOM_Geode;
//
class ATOM_ENGINE_API ATOM_ActionMixer2: public ATOM_ReferenceObj
{
private:
	struct ActionContext
	{
		unsigned loopCountDesired;
		unsigned loopCountCurrent;
		unsigned fadeTime;
		unsigned fadeTick;
		unsigned startTick;
		unsigned updateTick;
		unsigned cycleTick;
		unsigned duration;
		float lerpFactor;
		float speed;
		bool bindpos;
		bool done;
		bool trackReady;
		ATOM_STRING actionName;
		ATOM_AUTOPTR(ATOM_JointAnimationTrack) track;
	};

	static const unsigned START_CYCLE = (unsigned)(-1);
	static const unsigned START_FADE = (unsigned)(-1);

public:
	ATOM_ActionMixer2 (ATOM_Geode *geode);

public:
	void clearActions (void);
	bool setUpsideAction (const char *actionName, unsigned loopCount, bool forceReset, unsigned fadeTime, float speed);
	bool setDownsideAction (const char *actionName, unsigned loopCount, bool forceReset, unsigned fadeTime, float speed);
	bool resetUpsideAction (void);
	bool resetDownsideAction (void);
	void setDefaultUpsideActionSpeed (float speed);
	void setDefaultDownsideActionSpeed (float speed);
	void setCurrentUpsideActionSpeed (float speed);
	void setCurrentDownsideActionSpeed (float speed);
	void updateActions (void);
	const ATOM_BBox &getBoundingbox (void) const;
	bool getBoneMatrix (int bone, ATOM_Matrix4x4f &matrix, bool suppressManualTransform);
	bool getBoneMatrixAtTime (int bone, unsigned timeInMs, ATOM_Matrix4x4f &matrix);
	void enableManualBoneTransform (int bone, const ATOM_Matrix4x4f &matrix);
	void disableManualBoneTransform (int bone);
	bool isBoneManualTransformEnabled (int bone) const;
	ATOM_JointAnimationTrack *getUpsideTrack (void) const;
	ATOM_JointAnimationTrack *getDownsideTrack (void) const;
	unsigned getUpsideDesiredLoopCount (void) const;
	unsigned getDownsideDesiredLoopCount (void) const;
	unsigned getUpsideCurrentLoopCount (void) const;
	unsigned getDownsideCurrentLoopCount (void) const;
	unsigned getUpsideStartTick (void) const;
	unsigned getDownsideStartTick (void) const;
	float getUpsideSpeed (void) const;
	float getDownsideSpeed (void) const;
	unsigned getUpsideFadeTime (void) const;
	unsigned getDownsideFadeTime (void) const;
	bool isUpsideAnimating (void) const;
	bool isDownsideAnimating (void) const;
	bool isActionReady (void) const;
	bool isBoundingboxOk (void) const;
	const ATOM_VECTOR<ATOM_Matrix3x4f> &getJointMatrices(void) const;
	//--- wangjian added ---//
	bool isUppersideActionExist(void) const;
	bool isDownsideActionExist(void) const;
	//----------------------//

private:
	bool setAction (const char *actionname, unsigned loopCount, bool forceReset, ActionContext &context, unsigned fadeTime, float speed);
	bool resetAction (ActionContext &context);
	void copyTransforms (const ActionContext &context);
	unsigned getActionDuration (const char *actionname);
	void updateActionCycle (ActionContext &context, const ATOM_FrameStamp &frameStamp);
	void updateActionStatus (ActionContext &context, const ATOM_FrameStamp &frameStamp);
	void computeJointTransforms (const ActionContext &context);
	void checkTrackReady (void);
	void ensureJointMatReady (ATOM_Skeleton *skeleton);

private:
	ATOM_Geode *_geode;
	ActionContext _upsideContext;
	ActionContext _downsideContext;
	bool _boundingboxOk;
	unsigned _forceUpdateStamp;
	unsigned _jointMatUpdateStamp;
	float _upsideActionSpeed;
	float _downsideActionSpeed;
	ATOM_BBox _boundingBox;
	ATOM_STRING _defaultAction;
	ATOM_VECTOR<ATOM_JointTransformInfo> _lastTransforms;
	ATOM_VECTOR<ATOM_JointTransformInfo> _currentTransforms;
	ATOM_MAP<int, ATOM_Matrix4x4f> _manualTransforms;
	ATOM_VECTOR<ATOM_Matrix3x4f> _jointMatrices;
};

#endif // __ATOM3D_ENGINE_ACTIONMIXER2_H
