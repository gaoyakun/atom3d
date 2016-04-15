#include "StdAfx.h"
#include "actionmixer2.h"
#include "skinanim_sse.h"

ATOM_ActionMixer2::ATOM_ActionMixer2 (ATOM_Geode *geode)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::ATOM_ActionMixer2);

	_geode = geode;
	_upsideContext.trackReady = false;
	_upsideContext.startTick = 0;
	_upsideContext.bindpos = false;
	_upsideContext.done = true;
	_upsideContext.speed = 1.f;
	_downsideContext.trackReady = false;
	_downsideContext.startTick = 0;
	_downsideContext.bindpos = false;
	_downsideContext.done = true;
	_downsideContext.speed = 1.f;
	_boundingboxOk = false;
	_jointMatUpdateStamp = 0;
	_forceUpdateStamp = 0;
	_upsideActionSpeed = 1.f;
	_downsideActionSpeed = 1.f;
}

void ATOM_ActionMixer2::clearActions (void)
{
	_upsideContext.trackReady = false;
	_upsideContext.startTick = 0;
	_upsideContext.bindpos = false;
	_upsideContext.done = true;
	_upsideContext.speed = 1.f;
	_upsideContext.actionName = "";
	_upsideContext.track = nullptr;
	_downsideContext.trackReady = false;
	_downsideContext.startTick = 0;
	_downsideContext.bindpos = false;
	_downsideContext.done = true;
	_downsideContext.speed = 1.f;
	_downsideContext.actionName = "";
	_downsideContext.track = nullptr;
	_boundingboxOk = false;
	_jointMatUpdateStamp = 0;
	_forceUpdateStamp = 0;
	_upsideActionSpeed = 1.f;
	_downsideActionSpeed = 1.f;
}

bool ATOM_ActionMixer2::setUpsideAction (const char *actionName, unsigned loopCount, bool forceReset, unsigned fadeTime, float speed)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::setUpsideAction);

	return setAction (actionName, loopCount, forceReset, _upsideContext, fadeTime, speed);
}

bool ATOM_ActionMixer2::setDownsideAction (const char *actionName, unsigned loopCount, bool forceReset, unsigned fadeTime, float speed)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::setDownsideAction);

	return setAction (actionName, loopCount, forceReset, _downsideContext, fadeTime, speed);
}

bool ATOM_ActionMixer2::isUpsideAnimating (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::isUpsideAnimating);

	return !_upsideContext.done;
}

bool ATOM_ActionMixer2::isDownsideAnimating (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::isDownsideAnimating);

	return !_downsideContext.done;
}

unsigned ATOM_ActionMixer2::getUpsideDesiredLoopCount (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getUpsideDesiredLoopCount);

	return _upsideContext.loopCountDesired;
}

float ATOM_ActionMixer2::getUpsideSpeed (void) const
{
	return _upsideContext.speed;
}

unsigned ATOM_ActionMixer2::getUpsideFadeTime (void) const
{
	return _upsideContext.fadeTime;
}

unsigned ATOM_ActionMixer2::getDownsideFadeTime (void) const
{
	return _downsideContext.fadeTime;
}

float ATOM_ActionMixer2::getDownsideSpeed (void) const
{
	return _downsideContext.speed;
}

unsigned ATOM_ActionMixer2::getDownsideDesiredLoopCount (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getDownsideDesiredLoopCount);

	return _downsideContext.loopCountDesired;
}

unsigned ATOM_ActionMixer2::getUpsideCurrentLoopCount (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getUpsideCurrentLoopCount);

	return _upsideContext.loopCountCurrent;
}

unsigned ATOM_ActionMixer2::getDownsideCurrentLoopCount (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getDownsideCurrentLoopCount);

	return _downsideContext.loopCountCurrent;
}

void ATOM_ActionMixer2::setDefaultUpsideActionSpeed (float speed)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::setDefaultUpsideActionSpeed);

	_upsideActionSpeed = speed;
}

void ATOM_ActionMixer2::setDefaultDownsideActionSpeed (float speed)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::setDefaultDownsideActionSpeed);

	_downsideActionSpeed = speed;
}

void ATOM_ActionMixer2::setCurrentUpsideActionSpeed (float speed)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::setCurrentUpsideActionSpeed);

	_upsideContext.speed = speed;
}

void ATOM_ActionMixer2::setCurrentDownsideActionSpeed (float speed)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::setCurrentDownsideActionSpeed);

	_downsideContext.speed = speed;
}

void ATOM_ActionMixer2::updateActions (void)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::updateActions);

	updateActionCycle (_upsideContext, ATOM_APP->getFrameStamp());
	updateActionCycle (_downsideContext, ATOM_APP->getFrameStamp());

	ATOM_Skeleton *_skeleton = _geode->getSkeleton ();
	if (!_skeleton)
	{
		return;
	}

	if (_currentTransforms.empty ())
	{
		return;
	}
	ensureJointMatReady (_skeleton);
}

bool ATOM_ActionMixer2::setAction (const char *actionName, unsigned loopCount, bool forceReset, ActionContext &context, unsigned fadeTime, float speed)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::setAction);

	const ActionContext &otherContext = (&context == &_upsideContext) ? _downsideContext : _upsideContext;
	ATOM_JointAnimationTrack *track = 0;

	if (actionName)
	{
		track = _geode->getTrackByName (actionName);
		if (track)
		{
			context.trackReady = true;
		}
		//--- wangjian added ---//
		else
		{
			context.trackReady = false;
		}
		//----------------------//
		context.actionName = actionName;
	}
	else
	{
		context.trackReady = true;
		context.actionName.clear ();
	}

	if (speed == 0.f)
	{
		speed = (&context == &_upsideContext) ? _upsideActionSpeed : _downsideActionSpeed;
	}

	const ATOM_FrameStamp &frameStamp = ATOM_APP->getFrameStamp();
	unsigned time = frameStamp.currentTick;
	unsigned cycleTick = (track && !forceReset && !otherContext.done && otherContext.track.get() == track) ?  otherContext.cycleTick : 0;

	if (context.track)
	{
		if (!forceReset && !context.done && context.track.get() == track)
		{
			context.loopCountDesired = loopCount;
			context.loopCountCurrent = 0;
			context.speed = speed;
			context.fadeTime = fadeTime;
			context.startTick = 0;
			return true;
		}
		else
		{
			copyTransforms (context);
			context.loopCountDesired = loopCount;
			context.loopCountCurrent = 0;
			context.fadeTime = fadeTime;
			context.fadeTick = 0;
			context.updateTick = 0;
			context.startTick = 0;
			context.cycleTick = cycleTick;
			context.duration = track ? track->getTotalAnimationTime() : 0;
			context.bindpos = !actionName;
			context.done = false;
			context.speed = speed;
			context.track = track;
		}
	}
	else
	{
		context.loopCountDesired = loopCount;
		context.loopCountCurrent = 0;
		context.fadeTime = fadeTime;
		context.fadeTick = context.done ? 0 : fadeTime;
		context.updateTick = 0;
		context.startTick = 0;
		context.cycleTick = cycleTick;
		context.duration = track ? track->getTotalAnimationTime() : 0;
		context.bindpos = !actionName;
		context.done = false;
		context.speed = speed;
		context.track = track;
	}

	// JointMats need to be update
	_jointMatUpdateStamp = 0;

	updateActionCycle (context, frameStamp);

	_boundingboxOk = _upsideContext.track && _downsideContext.track;
	_forceUpdateStamp = frameStamp.frameStamp;

	return true;
}

void ATOM_ActionMixer2::updateActionCycle (ActionContext &context, const ATOM_FrameStamp &frameStamp)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::updateActionCycle);

	if (context.actionName.empty ())
	{
		//return;
	}

	if (frameStamp.currentTick == context.updateTick )
	{
		return;
	}

	checkTrackReady ();

	if (!context.track)
	{
		return;
	}

	if (context.startTick == 0)
	{
		context.startTick = frameStamp.currentTick;
	}

	bool firstUpdate = context.updateTick == 0;

	if (!firstUpdate)
	{
		unsigned elapsedTick = (frameStamp.currentTick - context.updateTick) * context.speed;
		context.cycleTick += elapsedTick;
		if (context.duration != 0 && context.cycleTick >= context.duration)
		{
			context.loopCountCurrent += context.cycleTick / context.duration;
			context.cycleTick %= context.duration;
		}

		context.fadeTick += frameStamp.elapsedTick;
		if (context.fadeTick > context.fadeTime)
		{
			context.fadeTick = context.fadeTime;
		}
	}

	context.updateTick = frameStamp.currentTick;
	context.lerpFactor = (context.fadeTime == 0) ? 1.f : float(context.fadeTick)/float(context.fadeTime);

	if (context.loopCountDesired != 0 && context.loopCountCurrent >= context.loopCountDesired)
	{
		context.done = true;
	}
	else if (!context.bindpos || firstUpdate)
	{
		computeJointTransforms (context);
	}
}

static const ATOM_VECTOR<ATOM_JointTransformInfo> &getIdentityTransform (unsigned size)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getIdentityTransform);

	static ATOM_VECTOR<ATOM_JointTransformInfo> transforms;
	unsigned currentSize = transforms.size();
	if (size > currentSize)
	{
		transforms.resize (size);
		for (unsigned i = currentSize; i < size; ++i)
		{
			transforms[i].rotate.set (0.f, 0.f, 0.f, 1.f);
			transforms[i].translate.set (0.f, 0.f, 0.f, 1.f);
			transforms[i].scale.set (1.f, 1.f, 1.f, 1.f);
		}
	}
	return transforms;
}

void ATOM_ActionMixer2::computeJointTransforms (const ActionContext &context)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::computeJointTransforms);

	ATOM_Skeleton *_skeleton = _geode->getSkeleton ();
	if (!_skeleton)
	{
		return;
	}

	bool bindpos = context.bindpos;
	if (!context.track)
	{
		bindpos = true;
	}

	if (context.lerpFactor == 0.f)
	{
	}
	else
	{
		const ATOM_VECTOR<unsigned> &mask = (&context == &_upsideContext) ? _skeleton->getUpsideMask () : _skeleton->getDownsideMask ();
		if (bindpos)
		{
			_currentTransforms.resize (_skeleton->getNumJoints());
			const ATOM_VECTOR<ATOM_JointTransformInfo> &transforms = getIdentityTransform (_currentTransforms.size());
			for (unsigned i = 0; i < mask.size(); ++i)
			{
				_currentTransforms[mask[i]] = transforms[mask[i]];
			}
		}
		else
		{
			float frame = context.track->getAnimationFrame (context.cycleTick);

			unsigned frame1 = ATOM_ftol (frame);
			unsigned frame2 = frame1 + 1;
			float factor = frame - frame1;

			unsigned numJoints1, numJoints2;
			const ATOM_JointTransformInfo *transform1 = context.track->getFrameJoints (frame1, &numJoints1);
			const ATOM_JointTransformInfo *transform2 = context.track->getFrameJoints (frame2, &numJoints2);
			ATOM_ASSERT(numJoints1 == numJoints2);

			_currentTransforms.resize (numJoints1);

			if (mask.empty ())
			{
				//slerpJointQuats (&_currentTransforms[0], transform1, transform2, factor, numJoints1);
			}
			else
			{
				//slerpJointFastEx (&vTemp[0], transform1, transform2, factor, mask.size(), &mask[0]);
				slerpJointQuatsEx (&_currentTransforms[0], transform1, transform2, factor, mask.size(), &mask[0]);
			}

			if (context.lerpFactor != 1.f)
			{
				if (mask.empty ())
				{
					//slerpJointQuats (&_currentTransforms[0], &_lastTransforms[0], transform, context.lerpFactor, numJoints);
				}
				else if (_lastTransforms.size() == _currentTransforms.size())
				{
					slerpJointQuatsEx(&_currentTransforms[0], &_lastTransforms[0], &_currentTransforms[0], context.lerpFactor, mask.size(), &mask[0]);
				}
			}
		}
	}
}

void ATOM_ActionMixer2::copyTransforms (const ActionContext &context)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::copyTransforms);

	ATOM_Skeleton *_skeleton = _geode->getSkeleton ();

	if (_skeleton && !_currentTransforms.empty ())
	{
		const ATOM_VECTOR<unsigned> &mask = (&context == &_upsideContext) ? _skeleton->getUpsideMask () : _skeleton->getDownsideMask ();
		_lastTransforms.resize (_currentTransforms.size());

		if (mask.empty ())
		{
			//for (unsigned i = 0; i < _currentTransforms.size(); ++i)
			//{
			//	_lastTransforms[i] = _currentTransforms[i];
			//}
		}
		else
		{
			for (unsigned i = 0; i < mask.size(); ++i)
			{
				unsigned index = mask[i];

				_lastTransforms[index] = _currentTransforms[index];
			}
		}
	}
	else
	{
		_lastTransforms = _currentTransforms;
	}
}

unsigned ATOM_ActionMixer2::getActionDuration (const char *actionname)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getActionDuration);

	ATOM_JointAnimationTrack *track = _geode->getTrackByName (actionname);
	if (!track)
	{
		return 0;
	}
	return track->getTotalAnimationTime ();
}


const ATOM_BBox &ATOM_ActionMixer2::getBoundingbox (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getBoundingbox);

	return _boundingBox;
}

void ATOM_ActionMixer2::enableManualBoneTransform (int joint, const ATOM_Matrix4x4f &matrix)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::enableManualBoneTransform);

	if (joint >= 0)
	{
		ATOM_Skeleton *skeleton = _geode->getSkeleton ();
		if (!skeleton)
		{
			ATOM_ContentStream::waitForAllDone ();
		}
		skeleton = _geode->getSkeleton ();
		if (!skeleton)
		{
			return;
		}

		_manualTransforms[joint] = matrix;
	}
}

void ATOM_ActionMixer2::disableManualBoneTransform (int joint)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::disableManualBoneTransform);

	if (joint >= 0)
	{
		ATOM_MAP<int, ATOM_Matrix4x4f>::iterator it = _manualTransforms.find (joint);

		if (it != _manualTransforms.end ())
		{
			_manualTransforms.erase (it);
		}
	}
}

bool ATOM_ActionMixer2::isBoneManualTransformEnabled (int joint) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::isBoneManualTransformEnabled);

	if (joint >= 0)
	{
		ATOM_MAP<int, ATOM_Matrix4x4f>::const_iterator it = _manualTransforms.find (joint);
		return it != _manualTransforms.end ();
	}
	return false;
}

bool ATOM_ActionMixer2::getBoneMatrixAtTime (int joint, unsigned timeInMs, ATOM_Matrix4x4f &matrix)
{
	ATOM_Skeleton *_skeleton = _geode->getSkeleton ();
	if (!_skeleton)
	{
		return false;
	}

	if (joint < 0 || joint >= _skeleton->getNumAttachPoints())
	{
		return false;
	}

	if (_currentTransforms.empty ())
	{
		_skeleton->getJointMatricesWorld ()[joint].toMatrix44 (matrix);
		return true;
	}

	return false;
}

bool ATOM_ActionMixer2::getBoneMatrix (int joint, ATOM_Matrix4x4f &matrix, bool suppressManualTransform)
{
	ATOM_Skeleton *_skeleton = _geode->getSkeleton ();
	if (!_skeleton)
	{
		return false;
	}

	if (joint < 0 || joint >= _skeleton->getNumAttachPoints())
	{
		return false;
	}

	if (_currentTransforms.empty ())
	{
		_skeleton->getJointMatricesWorld ()[joint].toMatrix44 (matrix);
		return true;
	}

	updateActions ();

	if (!suppressManualTransform)
	{
		ATOM_MAP<int, ATOM_Matrix4x4f>::const_iterator it = _manualTransforms.find (joint);
		if (it != _manualTransforms.end ())
		{
			matrix = it->second;
			return true;
		}
	}

	int parent = _skeleton->getJointParents()[joint];
	if (parent >= 0)
	{
		ATOM_Matrix4x4f parentBoneMatrix;
		if (!getBoneMatrix (parent, parentBoneMatrix, false))
		{
			return false;
		}
		_skeleton->getJointBindMatrix (joint).toMatrix44 (matrix);
		matrix <<= parentBoneMatrix;
	}
	else
	{
		_skeleton->getJointBindMatrix (joint).toMatrix44 (matrix);
	}

	ATOM_ALIGN(16) ATOM_Matrix3x4f relMatrix;
	ATOM_Matrix4x4f relMatrix44;
	convertJointQuatToJointMat (&relMatrix, &_currentTransforms[joint], 1);
	relMatrix.toMatrix44 (relMatrix44);
	matrix = matrix >> relMatrix44;

	return true;
}

ATOM_JointAnimationTrack *ATOM_ActionMixer2::getUpsideTrack (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getUpsideTrack);

	return _upsideContext.track.get();
}

ATOM_JointAnimationTrack *ATOM_ActionMixer2::getDownsideTrack (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::getDownsideTrack);

	return _downsideContext.track.get();
}

bool ATOM_ActionMixer2::resetUpsideAction (void)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::resetUpsideAction);

	return resetAction (_upsideContext);
}

bool ATOM_ActionMixer2::resetDownsideAction (void)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::resetDownsideAction);

	return resetAction (_downsideContext);
}

bool ATOM_ActionMixer2::resetAction (ActionContext &context)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::resetAction);

	if (context.track)
	{
		context.done = false;
		context.loopCountCurrent = 0;
		context.cycleTick = 0;
		context.updateTick = 0;
		context.startTick = 0;
		ActionContext &otherContext = (&context == &_upsideContext) ? _downsideContext : _upsideContext;
		if (context.track == otherContext.track)
		{
			otherContext.cycleTick = 0;
		}
		return true;
	}
	return false;
}

bool ATOM_ActionMixer2::isActionReady (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::isActionReady);

	return _geode->getSkeleton() && _upsideContext.trackReady && _downsideContext.trackReady;
}

//--- wangjian added ---//
bool ATOM_ActionMixer2::isUppersideActionExist(void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::isUppersideActionExist);

	if( !_geode->getSkeleton() )
		return false;

	if( _upsideContext.track && _upsideContext.trackReady )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log(" %s upper side is exist and ready! \n", _upsideContext.track->getName() );
		return true;
	}

	if( !_upsideContext.actionName.empty() && !_upsideContext.trackReady )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log(" %s upper side is exist but not ready! \n", _upsideContext.actionName.c_str() );
		return true;
	}

	return false;
}
bool ATOM_ActionMixer2::isDownsideActionExist(void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::isDownsideActionExist);

	if( !_geode->getSkeleton() )
		return false;

	if( _downsideContext.track && _downsideContext.trackReady )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log(" %s down side is exist and ready! \n", _downsideContext.track->getName() );
		return true;
	}

	if( !_downsideContext.actionName.empty() && !_downsideContext.trackReady )
	{
		if( ATOM_AsyncLoader::isEnableLog() )
			ATOM_LOGGER::log(" %s down side is exist but not ready! \n", _downsideContext.actionName.c_str() );
		return true;
	}

	return false;
}
//----------------------//

void ATOM_ActionMixer2::checkTrackReady (void)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::checkTrackReady);

	if (!_upsideContext.bindpos && !_upsideContext.track && !_upsideContext.actionName.empty())
	{
		_upsideContext.track = _geode->getTrackByName (_upsideContext.actionName.c_str());
		if (_upsideContext.track)
		{
			_upsideContext.duration = _upsideContext.track->getTotalAnimationTime ();
			_upsideContext.trackReady = true;

			if (!_upsideContext.done)
			{
				computeJointTransforms (_upsideContext);
			}
		}
	}

	if (!_downsideContext.bindpos && !_downsideContext.track && !_downsideContext.actionName.empty())
	{
		_downsideContext.track = _geode->getTrackByName (_downsideContext.actionName.c_str());
		if (_downsideContext.track)
		{
			_downsideContext.duration = _downsideContext.track->getTotalAnimationTime ();
			_downsideContext.trackReady = true;

			if (!_downsideContext.done)
			{
				computeJointTransforms (_downsideContext);
			}
		}
	}
}

bool ATOM_ActionMixer2::isBoundingboxOk (void) const
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::isBoundingboxOk);

	return _boundingboxOk;
}

void ATOM_ActionMixer2::ensureJointMatReady (ATOM_Skeleton *_skeleton)
{
	ATOM_STACK_TRACE(ATOM_ActionMixer2::ensureJointMatReady);

	const ATOM_FrameStamp &frameStamp = ATOM_APP->getFrameStamp ();

	if (frameStamp.frameStamp != _jointMatUpdateStamp || _jointMatrices.empty())
	{
		//updateActions ();

		_jointMatrices.resize (_skeleton->getNumJoints());
		convertJointQuatToJointMat (&_jointMatrices[0], &_currentTransforms[0], _currentTransforms.size());

#if 1
		transformJoint (&_jointMatrices[0], &_skeleton->getJointMatrices()[0], &_jointMatrices[0], _jointMatrices.size());
#else
		for (unsigned i = 0; i < _jointMatrices.size(); ++i)
		{
#if 1
			_jointMatrices[i] <<= _skeleton->getJointBindMatrix (i);
			transformJoint (_jointMatrices, _jointMatrices, &_skeleton->getJointMatrices()[0], _jointMatrices.size());
#else
			ATOM_Matrix4x4f m;
			_jointMatrices[i].toMatrix44 (m);
			_jointMatrices[i] = _skeleton->getJointBindMatrix (i) >> m;
#endif
		}
#endif

		for (ATOM_MAP<int, ATOM_Matrix4x4f>::const_iterator it = _manualTransforms.begin(); it != _manualTransforms.end (); ++it)
		{
			int joint = it->first;
			int parent = _skeleton->getJointParents()[joint];

			if (parent >= 0)
			{
				ATOM_Matrix4x4f m1, m2;
				_jointMatrices[parent].toMatrix44 (m1);
				parent = _skeleton->getJointParents()[parent];
				while (parent >= 0)
				{
					_jointMatrices[parent].toMatrix44 (m2);
					m1 = m2 >> m1;
					parent = _skeleton->getJointParents()[parent];
				}
				m1.invertAffine ();
				_jointMatrices[joint] = m1 >> it->second;
			}
			else
			{
				_jointMatrices[joint] = it->second;
			}
		}

		transformSkeleton (&_jointMatrices[0], &_skeleton->getJointParents()[0], _jointMatrices.size());

		_boundingBox.beginExtend ();
#if 1
		for (unsigned i = 0; i < _jointMatrices.size(); ++i)
		{
			_boundingBox.extend (ATOM_Vector3f(_jointMatrices[i].m03, _jointMatrices[i].m13, _jointMatrices[i].m23));
		}
		transformJoint (&_jointMatrices[0], &_jointMatrices[0], &_skeleton->getInverseJointMatricesWorld()[0], _jointMatrices.size());
#else
		for (unsigned i = 0; i < _jointMatrices.size(); ++i)
		{
#if 1
			_boundingBox.extend (ATOM_Vector3f(_jointMatrices[i].m03, _jointMatrices[i].m13, _jointMatrices[i].m23));
			_jointMatrices[i] >>= _skeleton->getInverseJointMatricesWorld ()[i];
#else
			ATOM_Matrix4x4f m;
			_jointMatrices[i].toMatrix44 (m);
			_boundingBox.extend (ATOM_Vector3f(m.m30, m.m31, m.m32));
			_jointMatrices[i] = m >> _skeleton->getInverseJointMatricesWorld ()[i];
#endif
		}
#endif
		_boundingboxOk = true;
		_geode->invalidateBoundingbox ();

		//if (_skeleton->getJointParents().size() > 0)
		//{
		//	// transform joint to global space
		//	transformSkeleton (&_jointMatrices[0], &_skeleton->getJointParents()[0], _jointMatrices.size());
		//}

		_jointMatUpdateStamp = frameStamp.frameStamp;
	}
}

unsigned ATOM_ActionMixer2::getUpsideStartTick (void) const
{
	return _upsideContext.startTick;
}

unsigned ATOM_ActionMixer2::getDownsideStartTick (void) const
{
	return _downsideContext.startTick;
}

const ATOM_VECTOR<ATOM_Matrix3x4f> &ATOM_ActionMixer2::getJointMatrices(void) const
{
	return _jointMatrices;
}

