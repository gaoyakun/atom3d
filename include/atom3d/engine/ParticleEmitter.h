/********************************************************************
	created:	2008/12/02
	created:	2:12:2008   13:16
	filename: 	ATOM_ParticleEmitter.h
	author:		Dashi Bai
	
	history:	
*********************************************************************/
#pragma once

#define DECLARE_EMITTER_PROPERTY(CLASS, PARAM, GETOR, SETOR) \
	inline void SETOR(const CLASS& val){ PARAM = val; } \
	inline const CLASS& GETOR() const { return PARAM; }

//! ����������
enum
{
	EMITTERTYPE_NULL = 0,	//!< �շ�����
	EMITTERTYPE_SPHERE,		//!< ���Ƿ�����
	EMITTERTYPE_RING,		//!< ��״������
	EMITTERTYPE_CYLINDER,	//!< ��״������
	EMITTERTYPE_TAPER,	//!< ׶״������
	EMITTERTYPE_COMHESIVE, //!�ھ۷�����
};
