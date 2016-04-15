#ifndef _LIGHT_SPACE_PERSPECTIVE_H_
#define _LIGHT_SPACE_PERSPECTIVE_H_

#pragma once

#include "../ATOM_math.h"
//#include "Frustum.h"
//#include "BoundingBox.h"
#include "../geometry/PolygonBody.h"

class LiSP
{
public:
	enum Mode
	{
		NOpt = 1,
		Reparametrized,
		Arbitrary,
		PseudoNear,
		ModeCOUNT
	};
	enum ProjctionType
	{
		PT_Ortho = 1,
		PT_Perspective = 2,
		PT_COUNT
	};

	typedef ATOM_VECTOR< ATOM_VECTOR< ATOM_Vector2f > > tParamArray;
	typedef ATOM_VECTOR< float > tPArray;

	LiSP(unsigned int nbrOfSplits);

	void SetLightProjMat( const ATOM_Matrix4x4f & matrix, ProjctionType lType = PT_Ortho );
	void SetLightView(const ATOM_Matrix4x4f & matrix/*, const View& view*/);
	void SetCamView(const ATOM_Matrix4x4f& matrix/*, const View& view*/);
	void SetCamProjMat(const ATOM_Matrix4x4f& matrix);
	void SetGamma(float gamma);
	void SetFieldOfView(float fov);
	void SetSplitParam(float lambda);
	void SetPseudoNear(float value);
	void SetPseudoFar(float value);
	
	const tPArray& GetParamN() const;
	void SetParamN(float n, unsigned int split);
	const tParamArray& GetParams() const;
	
	Mode GetMode() const;
	void SetMode(Mode mode);

	float GetParam() const;
	const tParamArray& GetDistances() const;

	bool GetArbScaled() const;
	void SetArbScaled(bool enable);

	const ATOM_Matrix4x4f & GetLiSPMtx(		unsigned int split, 
											const PolygonBody& bodyB,
											const sSimpleFrustm & camFrust,
											const ATOM_Vector3f & lightdir, 
											const ATOM_BBox& sceneAABB, 
											const ATOM_Vector2f& zRange	);

	void UpdateLDirVDirAngle(	const ATOM_Vector3f & eye, const ATOM_Vector3f & eye_at,
								const ATOM_Vector3f & lightDir );

private:
	ATOM_Vector3f GetNearCamPoint(	const sSimpleFrustm & camFrust, 
									/*const sSimpleFrustm * lightFrust, */
									const ATOM_BBox & sceneAABB	);

	float GetDistN(	const ATOM_Matrix4x4f & lViewProjMat, 
					const ATOM_BBox & bodyBaabb_LS, 
					const ATOM_Vector3f & nearCamPt,
					float nPseudo, 
					float fPseudo, 
					unsigned int split	);

	float GetReparamDistN(	const ATOM_Matrix4x4f & lViewProjMat, 
							const ATOM_BBox & bodyBaabb_LS,
							const ATOM_Vector3f & nearCamPt, 
							const sSimpleFrustm & camFrust, 
							float nPseudo, 
							float fPseudo, 
							unsigned int split	);

	float GetDistArbN(	const ATOM_Matrix4x4f & lViewProjMat, 
						const ATOM_BBox & bodyBaabb_LS, 
						const ATOM_Vector3f & nearCamPt, 
						unsigned int split, 
						const ATOM_Vector2f & zRange	);
	float GetPseudoNearN(	const ATOM_Matrix4x4f & lViewProjMat, 
							const ATOM_BBox & bodyBaabb_LS,  
							const ATOM_Vector3f & nearCamPt, 
							float nPseudo,
							unsigned int split	);

	void CalcZ0Z1_LS(	ATOM_Vector3f & z0_LS, 
						ATOM_Vector3f & z1_LS, 
						const ATOM_Matrix4x4f & lViewProjMat,
						const ATOM_BBox & bodyBaabb_LS, 
						const ATOM_Vector3f & nearCamPt	) const;

public:
	static const float Infinity;

private:
	ProjctionType _lType;
	Mode _mode;
	/*View _cView;
	View _lView;*/

	bool _arbScaled;

	float _gamma;
	//float _angleCamDirLightDir;
	float _fov;
	float _eta;
	float _lambda;
	float _pseudoNear;
	float _pseudoFar;

#pragma warning(push)
#pragma warning(disable : 4251)
	static const ATOM_Matrix4x4f Transform;
	ATOM_Matrix4x4f _lViewMat;
	ATOM_Matrix4x4f _lProjMat;
	ATOM_Matrix4x4f _cViewMat;
	ATOM_Matrix4x4f _cProjMat;
	ATOM_Matrix4x4f _liSPMat;
	tParamArray _distances;
	tParamArray _nParams;
	tPArray _paramN;
#pragma warning(pop)
};

#endif