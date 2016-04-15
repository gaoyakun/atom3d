FORCEINLINE const ATOM_Plane & ATOM_Frustum3d::operator [] (int index) const 
{
	return _planes[index];
}

FORCEINLINE ATOM_Plane & ATOM_Frustum3d::operator [] (int index) 
{
	return _planes[index];
}

FORCEINLINE const ATOM_Plane * ATOM_Frustum3d::planes (void) const 
{
	return _planes;
}

FORCEINLINE ATOM_Plane * ATOM_Frustum3d::planes (void) 
{
	return _planes;
}

FORCEINLINE void ATOM_Frustum3d::fromMatrix (const ATOM_Matrix4x4f &matMVP) 
{
	_planes[LEFT].setA (matMVP(0, 3) + matMVP(0, 0));
	_planes[LEFT].setB (matMVP(1, 3) + matMVP(1, 0));
	_planes[LEFT].setC (matMVP(2, 3) + matMVP(2, 0));
	_planes[LEFT].setD (matMVP(3, 3) + matMVP(3, 0));
	_planes[LEFT].normalize ();

	_planes[RIGHT].setA (matMVP(0, 3) - matMVP(0, 0));
	_planes[RIGHT].setB (matMVP(1, 3) - matMVP(1, 0));
	_planes[RIGHT].setC (matMVP(2, 3) - matMVP(2, 0));
	_planes[RIGHT].setD (matMVP(3, 3) - matMVP(3, 0));
	_planes[RIGHT].normalize ();

	_planes[BOTTOM].setA (matMVP(0, 3) + matMVP(0, 1));
	_planes[BOTTOM].setB (matMVP(1, 3) + matMVP(1, 1));
	_planes[BOTTOM].setC (matMVP(2, 3) + matMVP(2, 1));
	_planes[BOTTOM].setD (matMVP(3, 3) + matMVP(3, 1));
	_planes[BOTTOM].normalize ();

	_planes[TOP].setA (matMVP(0, 3) - matMVP(0, 1));
	_planes[TOP].setB (matMVP(1, 3) - matMVP(1, 1));
	_planes[TOP].setC (matMVP(2, 3) - matMVP(2, 1));
	_planes[TOP].setD (matMVP(3, 3) - matMVP(3, 1));
	_planes[TOP].normalize ();

	_planes[FRONT].setA (matMVP(0, 3) + matMVP(0, 2));
	_planes[FRONT].setB (matMVP(1, 3) + matMVP(1, 2));
	_planes[FRONT].setC (matMVP(2, 3) + matMVP(2, 2));
	_planes[FRONT].setD (matMVP(3, 3) + matMVP(3, 2));
	_planes[FRONT].normalize ();

	_planes[BACK].setA (matMVP(0, 3) - matMVP(0, 2));
	_planes[BACK].setB (matMVP(1, 3) - matMVP(1, 2));
	_planes[BACK].setC (matMVP(2, 3) - matMVP(2, 2));
	_planes[BACK].setD (matMVP(3, 3) - matMVP(3, 2));
	_planes[BACK].normalize ();
}
