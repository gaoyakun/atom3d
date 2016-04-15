#include "StdAfx.h"
#include <limits>
#include "../geometry/Plane_ext.h"
#include "LiSP.h"

const float LiSP::Infinity = 9.0e10f;

//const ATOM_Matrix4x4f LiSP::Transform = ATOM_Matrix4x4f
//(
//	1.0f,   .0f,  .0f,  .0f,
//	 .0f,   .0f, -1.0f,  .0f, // y -> z
//	 .0f,  1.0f,  .0f,  .0f, // z -> y
//	 .0f,   -.5f,  1.5f, 1.0f
//);

const ATOM_Matrix4x4f LiSP::Transform = ATOM_Matrix4x4f
(
	1.0f,   .0f,  .0f,  .0f,
	 .0f,   .0f, -1.0f,  .0f, // y -> z
	 .0f,  1.0f,  .0f,  .0f, // z -> y
	 .0f,   0.0f,  0.0f, 1.0f
);

//const ATOM_Matrix4x4f LiSP::Transform = ATOM_Matrix4x4f
//	(
//	1.0f,   .0f,  .0f,  .0f,
//	.0f,   .0f, 1.0f,  .0f, // y -> z
//	.0f,  1.0f,  .0f,  .0f, // z -> y
//	.0f,   .0f,  0.0f, 1.0f
//	);

LiSP::LiSP(unsigned int nbrOfSplits) :
	_lType(PT_Ortho),
	/*_cView(tVec3f(.0f, .0f, .0f), tVec3f(.0f, .0f, 1.0f), tVec3f(.0f, 1.0f, .0f)),
	_lView(tVec3f(.0f, 1.0f, .0f), tVec3f(.0f, .0f, 0.0f), tVec3f(.0f, .0f, -1.0f)),*/
	_mode(NOpt),
	_distances(ModeCOUNT),
	_nParams(ModeCOUNT),
	_paramN(nbrOfSplits, 1.0f),
	_gamma(90.0f),
	_fov(45.0f),
	_eta(-1.0f),
	_lambda(0.5f),
	_pseudoNear(.0f),
	_pseudoFar(.0f),
	_arbScaled(false)
{
	for (unsigned int i = NOpt; i < ModeCOUNT; ++i)
	{
		_nParams[i].resize(nbrOfSplits);
		_distances[i].resize(nbrOfSplits);
	}
}

void LiSP::SetLightProjMat(const ATOM_Matrix4x4f & matrix, ProjctionType lType )
{
	_lProjMat = matrix;
	_lType = lType;
}

void LiSP::SetLightView(const ATOM_Matrix4x4f & matrix/*, const View& view*/)
{
	_lViewMat = matrix;
	_lViewMat.invertAffine();
	/*_lView = view;*/
}

void LiSP::SetCamView(const ATOM_Matrix4x4f & matrix/*, const View & view*/)
{
	_cViewMat = matrix;
	_cViewMat.invertAffine();
	/*_cView = view;*/
}

void LiSP::SetCamProjMat(const ATOM_Matrix4x4f & matrix)
{
	_cProjMat = matrix;
}

void LiSP::SetGamma(float gamma)
{
	_gamma = gamma;
}

void LiSP::SetFieldOfView(float fov)
{
	_fov = fov;
}

void LiSP::SetSplitParam(float lambda)
{
	_lambda = lambda;
}

LiSP::Mode LiSP::GetMode() const
{
	return _mode;
}

void LiSP::SetMode(Mode mode)
{
	_mode = mode;
}

const LiSP::tParamArray& LiSP::GetDistances() const
{
	return _nParams;
}

float LiSP::GetParam() const
{
	return _eta;
}

const LiSP::tPArray& LiSP::GetParamN() const
{
	return _paramN;
}

void LiSP::SetParamN(float n, unsigned int split)
{
	if (split < _paramN.size())
		_paramN[split] = n;
}

void LiSP::SetPseudoNear(float value)
{
	_pseudoNear = value;
}

void LiSP::SetPseudoFar(float value)
{
	_pseudoFar = value;
}

bool LiSP::GetArbScaled() const
{
	return _arbScaled;
}

void LiSP::SetArbScaled(bool enable)
{
	_arbScaled = enable;
}

const ATOM_Matrix4x4f& LiSP::GetLiSPMtx(unsigned int split, 
										const PolygonBody& bodyB,
										const sSimpleFrustm& camFrust,
										const ATOM_Vector3f & lightdir, 
										const ATOM_BBox& sceneAABB,
										const ATOM_Vector2f& zRange)
{
	//ATOM_LOGGER::log ("%s\n", __FUNCTION__);

	ATOM_Matrix4x4f lProjMat = _lProjMat;
	ATOM_Matrix4x4f lViewMat = _lViewMat;

	const unsigned int nbrOfSplits = _paramN.size();
	unsigned int size = bodyB.GetVertexCount();
	const Polygon3d::tVertexArray& vertices = bodyB.GetVertexArray();

	ATOM_Vector3f nearCamPt = GetNearCamPoint(camFrust,/* lightFrust, */sceneAABB);		

	ATOM_Matrix4x4f lViewProjMat = lViewMat << lProjMat;

	ATOM_Scene *currentScene = ATOM_RenderScheme::getCurrentScene ();
	ATOM_Camera * camera = currentScene->getCamera();
	ATOM_Vector3f eye, center, up;
	camera->getCameraPos ( &eye, &center, &up);
	const ATOM_Vector3f viewDir(center - eye);
	const ATOM_Vector3f leftVec(crossProduct(up, viewDir));
	up = crossProduct(viewDir, leftVec);
	center = nearCamPt + viewDir;

	const ATOM_Vector3f center_LS = lViewProjMat >> center;
	const ATOM_Vector3f nearCamPt_LS = lViewProjMat >> nearCamPt;
	ATOM_Vector3f projViewDir_LS(center_LS - nearCamPt_LS);		
	projViewDir_LS.z = .0f;

	// do Light Space Perspective shadow mapping
	// rotate the lightspace so that the proj light view always points upwards
	// calculate a frame matrix that uses the projViewDir[light-space] as up vector
	// look(from position, into the direction of the projected direction, with unchanged up-vector)
	ATOM_Matrix4x4f viewMat;
	viewMat.makeLookatLH( ATOM_Vector3f(.0f), ATOM_Vector3f(.0f) + projViewDir_LS, ATOM_Vector3f(.0f, .0f, -1.0f) );
	viewMat.invertAffine();
	lProjMat = viewMat >> lProjMat;

	lViewProjMat = lProjMat >> lViewMat;

	ATOM_BBox bodyBaabb_LS(FLT_MAX,-FLT_MAX);
	for (unsigned int i = 0; i < size; ++i)
	{
		ATOM_Vector3f transformed = lViewProjMat >> vertices[i];
		bodyBaabb_LS.extend( transformed );
	}

	_distances[NOpt][split].x = GetDistN(lViewProjMat, bodyBaabb_LS, nearCamPt,
		split == 0 ? _pseudoNear : .0f, split == nbrOfSplits - 1 ? _pseudoFar : .0f, split);
	
	_distances[Reparametrized][split].x = GetReparamDistN(lViewProjMat, bodyBaabb_LS, nearCamPt, camFrust,
		split == 0 ? _pseudoNear : .0f, split == nbrOfSplits - 1 ? _pseudoFar : .0f, split);

	_distances[Arbitrary][split].x = GetDistArbN(lViewProjMat, bodyBaabb_LS, nearCamPt, split, zRange);

	_distances[PseudoNear][split].x = GetPseudoNearN(lViewProjMat, bodyBaabb_LS, nearCamPt,
		split == 0 ? _pseudoNear : .0f, split); 

	/*_distances[NOpt][split].x = 1.0f + sqrtf(650.0f);
	float fCosGamma = cosf(_gamma / 180.0f * M_PI);
	float sinGamma = sqrtf( 1.f - fCosGamma * fCosGamma );
	_distances[NOpt][split].x =  _distances[NOpt][split].x / sinGamma;
	_distances[NOpt][split].x += 0.1f;*/

	const float projLen = abs( dotProduct( viewDir, nearCamPt - eye ) );
	const ATOM_Vector3f nearCamCtr( eye + projLen * viewDir );
	const ATOM_Vector3f nearCamCtr_LS = lViewProjMat >>  nearCamCtr;
	
	//tVec3f nearCamPt_LS(vmath::transform(lViewProjMat, nearCamPt));
	//const tVec3f cStart_LS = tVec3f(nearCamPt_LS.x, nearCamPt_LS.y, bodyBaabb_LS.GetMaximum().z);

	// c_start has the x and y coordinate of nearCamCtr_LS, the z coord of B.min() 
	const ATOM_Vector3f cStart_LS = ATOM_Vector3f( nearCamCtr_LS.x, nearCamCtr_LS.y, bodyBaabb_LS.getMin().z/*bodyBaabb_LS.getMax().z*/ );

	for (unsigned int mode = NOpt; mode < ModeCOUNT; ++mode)
	{
		float n = _distances[mode][split].x;
		
		// if n < infinity calculate LiSP matrix
		if (n < std::numeric_limits<float>::infinity())
		{
			// calc depthrange of the perspective transform depth or light space y extents
			float zRange = abs( bodyBaabb_LS.getMax().z - bodyBaabb_LS.getMin().z);
			_distances[mode][split].y = n + zRange;
			
			if ( mode == static_cast<unsigned int>(_mode) )
			{
				n = 0.1f;

				// calc projection center
				ATOM_Vector3f projCenter(cStart_LS - ATOM_Vector3f(.0f, .0f, n));

				ATOM_Matrix4x4f invLViewProjMat = lViewProjMat;
				invLViewProjMat.invert();
				ATOM_Vector3f projCtr = invLViewProjMat >> projCenter;

				// the lispsm perspective transformation
				// here done with a standard frustum call that maps lispPersp onto the unit cube
				ATOM_Matrix4x4f liSPM;
				liSPM.makePerspectiveFrustumLH( -1.0f, 1.0f, -1.0f, 1.0f, n, n + zRange );
				ATOM_Matrix4x4f TranlationM;
				TranlationM.makeTranslate(-projCenter);

				liSPM >>= TranlationM;
				liSPM = Transform >> liSPM;
				ATOM_Matrix4x4f proj;
				proj.makeOrthoFrustumLH(-1,1,-1,1,-1,1);
				liSPM = proj >> liSPM;
				
				_liSPMat = liSPM >> viewMat;
			}
		}
		else
		{
			//float inf = std::numeric_limits<float>::infinity();
			float inf = Infinity;
			_distances[mode][split] = ATOM_Vector2f(inf, inf);
			_nParams[mode][split] = ATOM_Vector2f(inf, inf);

			if (mode == static_cast<unsigned int>(_mode))
				_liSPMat.makeIdentity();
		}
	}

	return _liSPMat;
}

ATOM_Vector3f LiSP::GetNearCamPoint(const sSimpleFrustm & camFrust, 
									/*const sSimpleFrustm * lightFrust, */
									const ATOM_BBox & sceneAABB)
{
	PolygonBody bodyLVS;

	bodyLVS.AddFrustum( camFrust );

	bodyLVS.Clip(sceneAABB);

	//// perspective lightProjection
	//if (_lType == PT_Perspective) 
	//	bodyLVS.Clip( *lightFrust );

	bodyLVS.CreateUniqueVertexArray();

	const Polygon3d::tVertexArray& vertices = bodyLVS.GetVertexArray();
	unsigned int size = bodyLVS.GetVertexCount();

	if (size)
	{
		// transform to eyespace and find the nearest point to camera -> z = max()
		unsigned int index = 0;
		ATOM_Vector3f nearest_point = _cViewMat >> vertices[index];
		ATOM_Vector3f eyespace_point;
		for (unsigned int i = 1; i < size; ++i)
		{
			eyespace_point = _cViewMat >> vertices[i];
			if( eyespace_point.z < nearest_point.z )
			{
				nearest_point = eyespace_point;
				index = i;
			}
		}
		return vertices[index];
	}
	else
	{
		ATOM_Scene *currentScene = ATOM_RenderScheme::getCurrentScene ();
		ATOM_Camera * camera = currentScene->getCamera();
		ATOM_Vector3f eye, center, up;
		camera->getCameraPos ( &eye, &center, &up);
		ATOM_Vector3f viewdir = (center - eye);
		viewdir.normalize();
		return eye + viewdir;
	}
}

void LiSP::CalcZ0Z1_LS(	ATOM_Vector3f & z0_LS, 
						ATOM_Vector3f & z1_LS,
						const ATOM_Matrix4x4f & lViewProjMat, 
						const ATOM_BBox & bodyBaabb_LS, 
						const ATOM_Vector3f & nearCamPt ) const
{
	ATOM_Scene *currentScene = ATOM_RenderScheme::getCurrentScene ();
	ATOM_Camera * camera = currentScene->getCamera();
	ATOM_Vector3f eye, center, up;
	camera->getCameraPos ( &eye, &center, &up);

	// calculate the parallel plane to the near plane through eye
	Plane planeZ0( nearCamPt, center - eye );
	
	// transform into lightspace with transposed inverted lightViewProj
	ATOM_Matrix4x4f invTransLVP = lViewProjMat;
	invTransLVP.invert();
	invTransLVP.transpose();
	planeZ0.Transform( invTransLVP );

	// get the parameters of plane from the plane equation n dot d = 0
	const ATOM_Vector3f & normal = planeZ0.GetNormal();
	const float distance = planeZ0.GetDistance();

	// z0_LS has the x coordinate of nearCamPoint_LS, the y coord of 
	// the plane intersection and the z coord of bodyB_LS.max() 
	const float bMaxZ = bodyBaabb_LS.getMax().z;
	const float bMinZ = bodyBaabb_LS.getMin().z;

	const ATOM_Vector3f nearCamPt_LS( lViewProjMat >> nearCamPt );
	
	z0_LS = ATOM_Vector3f
	(
		nearCamPt_LS.x,
		(distance - normal.z * bMinZ - normal.x * nearCamPt_LS.x) / normal.y,
		bMinZ
	);

	z1_LS = ATOM_Vector3f
	(
		z0_LS.x, z0_LS.y, bMaxZ
	);
}

float LiSP::GetDistN(	const ATOM_Matrix4x4f & lViewProjMat, 
						const ATOM_BBox & bodyBaabb_LS, 
						const ATOM_Vector3f & nearCamPt,
						float nPseudo, 
						float fPseudo, 
						unsigned int split	)
{
	ATOM_Vector3f z0_LS;
	ATOM_Vector3f z1_LS;

	CalcZ0Z1_LS( z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt );

	// transform to world
	ATOM_Matrix4x4f inv_lviewproj = lViewProjMat;
	inv_lviewproj.invert();
	const ATOM_Vector3f z0_WS( inv_lviewproj >> z0_LS );
	const ATOM_Vector3f z1_WS( inv_lviewproj >> z1_LS );

	// transform to eye
	const ATOM_Vector3f z0_ES( _cViewMat >> z0_WS );
	const ATOM_Vector3f z1_ES( _cViewMat >> z1_WS );

	const float n_e = abs(z0_ES.z);
	const float f_e = abs(z1_ES.z);
	const float zRange_LS = abs( bodyBaabb_LS.getMax().z - bodyBaabb_LS.getMin().z );
	const float zRange_ES = abs( f_e - n_e );

	//const float n = zRange_LS / ( sqrt(f_e / n_e) - 1.0f );
	const float n = zRange_LS / (f_e - n_e) * (n_e + sqrt((n_e + nPseudo * (f_e - n_e)) * (f_e - fPseudo * (f_e - n_e))));

	_nParams[NOpt][split].x = zRange_ES / zRange_LS * n;
	_nParams[NOpt][split].y = _nParams[NOpt][split].x + zRange_ES;

	return n;
}

float LiSP::GetReparamDistN(const ATOM_Matrix4x4f & lViewProjMat, 
							const ATOM_BBox & bodyBaabb_LS, 
							const ATOM_Vector3f & nearCamPt,
							const sSimpleFrustm & camFrust, 
							float /*nPseudo*/, 
							float /*fPseudo*/, 
							unsigned int split)
{
	ATOM_Vector3f z0_LS;
	ATOM_Vector3f z1_LS;

	CalcZ0Z1_LS( z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt );

	// transform to world
	ATOM_Matrix4x4f inv_lviewproj = lViewProjMat;
	inv_lviewproj.invert();
	const ATOM_Vector3f z0_WS( inv_lviewproj >> z0_LS );
	const ATOM_Vector3f z1_WS( inv_lviewproj >> z1_LS );

	// transform to eye
	const ATOM_Vector3f z0_ES( _cViewMat >> z0_WS );
	const ATOM_Vector3f z1_ES( _cViewMat >> z1_WS );

	const float theta = _fov / 2.0f;
	const float gamma_a = theta / 3.0f;
	const float gamma_b = theta;
	const float gamma_c = theta + 0.3f * (90.0f - theta);
	const float eta_b = -0.2f;
	const float eta_c = 0.0f;
	const float n_e = abs(z0_ES.z);
	const float f_e = abs(z1_ES.z);

	if (_gamma <= gamma_a)
	{
		_eta = -1;
	}
	else if (_gamma > gamma_a && _gamma <= gamma_b)
	{
		_eta = -1 + (eta_b + 1) * (_gamma - gamma_a) / (gamma_b - gamma_a);
	}
	else if (_gamma > gamma_b && _gamma <= gamma_c)
	{
		_eta = eta_b + (eta_c - eta_b) * sin(90.0f * (_gamma - gamma_b)/(gamma_c - gamma_b) * static_cast<float>(M_PI) / 180.0f);
	}
	else // gamma > gamma_c
	{
		_eta = eta_c;
	}

	const float w_n = ( camFrust.GetCorner(sSimpleFrustm::NearBottomRight) - camFrust.GetCorner(sSimpleFrustm::NearTopRight) ).getLength();
	//const double w_f = w_n * f_e / n_e;
	const float w_s = (f_e - n_e) / cos(theta * static_cast<float>(M_PI) / 180.0f);

	const float w_n_proj = w_n * cos(_gamma * static_cast<float>(M_PI) / 180.0f);
	const float w_s1_proj = _gamma <= theta ? w_s * (1 - cos((theta - _gamma) * static_cast<float>(M_PI) / 180.0f)) : 0.0f;
	const float w_s2_proj = w_s * sin((theta + _gamma) * static_cast<float>(M_PI) / 180.0f);

	const float w_lz = w_n_proj + w_s1_proj + w_s2_proj; // equivalent to w_ly in paper
	const float alpha = f_e / n_e;

	float n = 0.0f;

	if (_eta < 0.0f)
	{
		n = w_lz / (alpha - 1) * (1 + sqrt(alpha) - _eta * (alpha - 1)) / (_eta + 1);
	}
	else
	{
		n = w_lz / (alpha - 1) * (1 + sqrt(alpha)) / (_eta * sqrt(alpha) + 1);
	}

	const float zRange_LS = abs(bodyBaabb_LS.getMax().z - bodyBaabb_LS.getMin().z);
	const float zRange_ES = f_e - n_e;

	_nParams[Reparametrized][split] = ATOM_Vector2f(n, n + zRange_ES);

	return zRange_LS / zRange_ES * n;
}

float LiSP::GetDistArbN(	const ATOM_Matrix4x4f & lViewProjMat, 
							const ATOM_BBox & bodyBaabb_LS, 
							const ATOM_Vector3f & nearCamPt,
							unsigned int split, 
							const ATOM_Vector2f & zRange	)
{
	if (_arbScaled && split > 0)
	{
		const float n = zRange.x;
		const float f = zRange.y;
		const float s = split / static_cast<float>(_paramN.size());
		const float schemeFactor = ((1.0f - _lambda) * (n + (f - n) * s) + _lambda * (n * powf(f / n, s)));
		
		_nParams[Arbitrary][split] = ATOM_Vector2f(schemeFactor * _nParams[Arbitrary][0]);
		_paramN[split] = _nParams[Arbitrary][split].x;

		return _distances[Arbitrary][0].x / _nParams[Arbitrary][0].x * _nParams[Arbitrary][split].x;
	}
	else
	{
		ATOM_Vector3f z0_LS;
		ATOM_Vector3f z1_LS;

		CalcZ0Z1_LS(z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt);

		// transform to world
		ATOM_Matrix4x4f inv_lviewproj = lViewProjMat;
		inv_lviewproj.invert();
		const ATOM_Vector3f z0_WS( inv_lviewproj >> z0_LS );
		const ATOM_Vector3f z1_WS( inv_lviewproj >> z1_LS );

		// transform to eye
		const ATOM_Vector3f z0_ES( _cViewMat >> z0_WS );
		const ATOM_Vector3f z1_ES( _cViewMat >> z1_WS );

		const float n_e = abs(z0_ES.z);
		const float f_e = abs(z1_ES.z);

		const float zRange_LS = abs(bodyBaabb_LS.getMax().z - bodyBaabb_LS.getMin().z);
		const float zRange_ES = f_e - n_e;

		//const float n = max(_paramN[split], n_e);
		const float n = _paramN[split];

		_nParams[Arbitrary][split] = ATOM_Vector2f(n, n + zRange_ES);

		return zRange_LS / zRange_ES * n;
	}
}

float LiSP::GetPseudoNearN(	const ATOM_Matrix4x4f & lViewProjMat, 
							const ATOM_BBox & bodyBaabb_LS, 
							const ATOM_Vector3f & nearCamPt,
							float nPseudo, 
							unsigned int split	)
{
	if (nPseudo >= 2 / 3.0f)
		nPseudo = 2 / 3.0f;

	ATOM_Vector3f z0_LS;
	ATOM_Vector3f z1_LS;

	CalcZ0Z1_LS(z0_LS, z1_LS, lViewProjMat, bodyBaabb_LS, nearCamPt);

	// transform to world
	ATOM_Matrix4x4f inv_lviewproj = lViewProjMat;
	inv_lviewproj.invert();
	const ATOM_Vector3f z0_WS( inv_lviewproj >> z0_LS );
	const ATOM_Vector3f z1_WS( inv_lviewproj >> z1_LS );

	// transform to eye
	const ATOM_Vector3f z0_ES( _cViewMat >> z0_WS );
	const ATOM_Vector3f z1_ES( _cViewMat >> z1_WS );

	const float n_e = abs(z0_ES.z);
	const float f_e = abs(z1_ES.z);
	const float zRange_LS = abs(bodyBaabb_LS.getMax().z - bodyBaabb_LS.getMin().z);
	const float zRange_ES = f_e - n_e;
	
	const float n_0 = n_e / zRange_ES;
	const float n1 = n_0 + sqrt( ( n_0 + nPseudo ) * ( n_0 + 1 ) );
	const float n2 = abs( nPseudo / ( 2 - 3 * nPseudo ) );

	float n = max(n1, n2);

	_nParams[PseudoNear][split] = ATOM_Vector2f(n * zRange_ES, n * zRange_ES + zRange_ES);

	return zRange_LS * n;
}

void LiSP::UpdateLDirVDirAngle( const ATOM_Vector3f & eye, const ATOM_Vector3f & eye_at,
								const ATOM_Vector3f & lightDir )
{
	ATOM_Vector3f cViewDir( eye_at - eye );
	cViewDir.normalize();

	ATOM_Vector3f lViewDir(lightDir);
	lViewDir.normalize();

	_gamma = acos( abs( dotProduct( cViewDir, lViewDir  ) ) ) * static_cast<float>( 180.0 / M_PI );

	//_angleCamDirLightDir = acos(vmath::dot(cViewDir, lViewDir));
}

