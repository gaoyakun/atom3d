#include "StdAfx.h"
#include "scriptbind_traits.h"

ATOM_Vector2f Vector2f_Constructor (float x, float y)
{
	return ATOM_Vector2f(x, y);
}

ATOM_Vector3f Vector3f_Constructor (float x, float y, float z)
{
	return ATOM_Vector3f(x, y, z);
}

ATOM_Vector4f Vector4f_Constructor (float x, float y, float z, float w)
{
	return ATOM_Vector4f(x, y, z, w);
}

template <class VecType>
float Vector_Dot (const VecType &v1, const VecType &v2)
{
	return dotProduct (v1, v2);
}

template <class VecType>
VecType Vector_Add (VecType *v1, VecType *v2)
{
	return (*v1) + (*v2);
}

template <class VecType>
VecType Vector_Sub (VecType *v1, VecType *v2)
{
	return (*v1) - (*v2);
}

template <class VecType>
VecType Vector_Mul (VecType *v1, VecType *v2)
{
	return (*v1) * (*v2);
}

template <class VecType>
VecType Vector_Div (VecType *v1, VecType*v2)
{
	return (*v1) / (*v2);
}

template <class VecType>
VecType Vector_Neg (VecType *v1)
{
	return -(*v1);
}

template <class VecType>
bool Vector_Eq (VecType *v1, VecType *v2)
{
	return (*v1) == (*v2);
}

template <class VecType>
float Vector_Index (VecType *v1, int index)
{
	return (*v1)[index];
}

template <class VecType>
void Vector_NewIndex (VecType *v1, int index, float val)
{
	(*v1)[index] = val;
}

bool BBox_Intersect (ATOM_BBox *bbox, const ATOM_Vector3f &lineBegin, const ATOM_Vector3f &lineVector, ATOM_Vector3f *intersectionPoint)
{
    return bbox->intersect(lineBegin, lineVector, *intersectionPoint);
}

ATOM_Matrix3x3f Matrix33_Constructor (float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22)
{
	return ATOM_Matrix3x3f(m00, m01, m02, m10, m11, m12, m20, m21, m22);
}

ATOM_Matrix3x4f Matrix43_Constructor (float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23)
{
	return ATOM_Matrix3x4f(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23);
}

ATOM_Matrix4x4f Matrix44_Constructor (float m00, float m01, float m02, float m03, float m10, float m11, float m12, float m13, float m20, float m21, float m22, float m23, float m30, float m31, float m32, float m33)
{
	return ATOM_Matrix4x4f(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33);
}

template <class MatrixType>
float Matrix_Index (MatrixType *m, int index)
{
	return m->m[index];
}

template <class MatrixType>
void Matrix_NewIndex (MatrixType *m, int index, float val)
{
	m->m[index] = val;
}

template <class MatrixType, class VecType>
VecType Matrix_GetRow (MatrixType *m, int row)
{
	return m->getRow (row);
}

template <class MatrixType, class VecType>
void Matrix_SetRow (MatrixType *m, int row, const VecType &vec)
{
	m->setRow (row, vec);
}

template <class MatrixType, class VecType>
VecType Matrix_GetCol (MatrixType *m, int col)
{
	return m->getCol (col);
}

template <class MatrixType, class VecType>
void Matrix_SetCol (MatrixType *m, int col, const VecType &vec)
{
	m->setCol (col, vec);
}

template <class MatrixType, class VecType>
VecType Matrix_GetRow3 (MatrixType *m, int row)
{
	return m->getRow3 (row);
}

template <class MatrixType, class VecType>
void Matrix_SetRow3 (MatrixType *m, int row, const VecType &vec)
{
	m->setRow3 (row, vec);
}

template <class MatrixType, class VecType>
VecType Matrix_GetCol3 (MatrixType *m, int col)
{
	return m->getCol3 (col);
}

template <class MatrixType, class VecType>
void Matrix_SetCol3 (MatrixType *m, int col, const VecType &vec)
{
	m->setCol3 (col, vec);
}

template <class MatrixType>
MatrixType Matrix_Mul (MatrixType *m1, MatrixType *m2)
{
	return (*m1) >> (*m2);
}

ATOM_Vector3f Matrix33_Transform (ATOM_Matrix3x3f *m, ATOM_Vector3f *v)
{
	return m->transform (*v);
}

template <class MatrixType, class VecType>
VecType Matrix_TransformPoint(MatrixType *m, VecType *v)
{
	return m->transformPoint (*v);
}

template <class MatrixType, class VecType>
VecType Matrix_TransformPointAffine(MatrixType *m, VecType *v)
{
	return m->transformPointAffine (*v);
}

template <class MatrixType, class VecType>
VecType Matrix_TransformVector(MatrixType *m, VecType *v)
{
	return m->transformVector (*v);
}

template <class MatrixType, class VecType>
VecType Matrix_TransformVectorAffine(MatrixType *m, VecType *v)
{
	return m->transformVectorAffine (*v);
}

void Matrix43_Set (ATOM_Matrix3x4f *m, float m00, float m01, float m02, float m03,
				   float m10, float m11, float m12, float m13,
				   float m20, float m21, float m22, float m23)
{
	m->set (m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23);
}

void Matrix43_SetM33 (ATOM_Matrix3x4f *m, ATOM_Matrix3x3f *m33)
{
	m->set (*m33);
}

void Matrix43_SetM33T (ATOM_Matrix3x4f *m, ATOM_Matrix3x3f *m33, ATOM_Vector3f *t)
{
	m->set (*m33, *t);
}

ATOM_Matrix4x4f Matrix43_ToM44 (ATOM_Matrix3x4f *m)
{
	ATOM_Matrix4x4f result;
	m->toMatrix44 (result);
	return result;
}

ATOM_Quaternion Quaternion_Constructor (float x, float y, float z, float w)
{
	return ATOM_Quaternion(x, y, z, w);
}

float Quaternion_ToAxisAngle(ATOM_Quaternion *q, ATOM_Vector3f *axis)
{
	float f;
	q->toAxisAngle (f, *axis);
	return f;
}

void Quaternion_FromMatrix33 (ATOM_Quaternion *q, ATOM_Matrix3x3f *m)
{
	q->fromMatrix (*m);
}

void Quaternion_FromMatrix44 (ATOM_Quaternion *q, ATOM_Matrix4x4f *m)
{
	q->fromMatrix (*m);
}

ATOM_Matrix3x3f Quaternion_ToMatrix33 (ATOM_Quaternion *q)
{
	ATOM_Matrix3x3f m;
	q->toMatrix (m);
	return m;
}

ATOM_Matrix4x4f Quaternion_ToMatrix44 (ATOM_Quaternion *q)
{
	ATOM_Matrix4x4f m;
	q->toMatrix (m);
	return m;
}

ATOM_Vector3f Quaternion_RotateVector(ATOM_Quaternion *q, ATOM_Vector3f *v)
{
	ATOM_Vector3f result;
	q->rotateVector (*v, result);
	return result;
}

ATOM_BBox BBox_Constructor (const ATOM_Vector3f &minPoint, const ATOM_Vector3f &maxPoint)
{
	return ATOM_BBox (minPoint, maxPoint);
}

bool BBox_ContainsBox (ATOM_BBox *bbox, ATOM_BBox *other)
{
	return bbox->contains (*other);
}

bool BBox_ContainsPoint (ATOM_BBox *bbox, ATOM_Vector3f *point)
{
	return bbox->contains (*point);
}

int BBox_ClipToBox (ATOM_BBox *bbox, ATOM_BBox *other)
{
	return bbox->getClipState (*other);
}

int BBox_ClipToMatrix (ATOM_BBox *bbox, ATOM_Matrix4x4f *matrix)
{
	return bbox->getClipState (*matrix);
}

int BBox_ClipToMatrixWithContrib (ATOM_BBox *bbox, ATOM_Matrix4x4f *matrix, float contrib)
{
	return bbox->getClipState (*matrix, contrib);
}

void BBox_Extend (ATOM_BBox *bbox, ATOM_Vector3f *v)
{
	bbox->extend (*v);
}

ATOM_ColorARGB ColorARGB_Constructor (float r, float g, float b, float a)
{
	return ATOM_ColorARGB(r, g, b, a);
}

unsigned BitsAnd (unsigned a, unsigned b)
{
	return a & b;
}

unsigned BitsOr (unsigned a, unsigned b)
{
	return a | b;
}

unsigned BitsNot (unsigned a)
{
	return ~a;
}

unsigned BitsXOR (unsigned a, unsigned b)
{
	return a ^ b;
}

unsigned BitsSHR (unsigned a, int b)
{
	return a >> b;
}

unsigned BitsSHL (unsigned a, int b)
{
	return a << b;
}

int BitsSAR (int a, int b)
{
	return a >> b;
}

int BitsSAL (int a, int b)
{
	return a << b;
}

ATOM_SCRIPT_BEGIN_FUNCTION_TABLE(MiscFunctions)
	ATOM_DECLARE_FUNCTION(ATOM_BitsAnd, BitsAnd)
	ATOM_DECLARE_FUNCTION(ATOM_BitsOr, BitsOr)
	ATOM_DECLARE_FUNCTION(ATOM_BitsNot, BitsNot)
	ATOM_DECLARE_FUNCTION(ATOM_BitsXOR, BitsXOR)
	ATOM_DECLARE_FUNCTION(ATOM_BitsSHR, BitsSHR)
	ATOM_DECLARE_FUNCTION(ATOM_BitsSHL, BitsSHL)
	ATOM_DECLARE_FUNCTION(ATOM_BitsSAR, BitsSAR)
	ATOM_DECLARE_FUNCTION(ATOM_BitsSAL, BitsSAL)
ATOM_SCRIPT_END_FUNCTION_TABLE

ATOM_SCRIPT_BEGIN_TYPE_TABLE(MiscTypes)

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_Vector2f, ATOM_Vector2f)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_Vector2f, x, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_Vector2f, y, float)
		ATOM_DECLARE_CONSTRUCTOR(Vector2f_Constructor)
		ATOM_DECLARE_METHOD(set, ATOM_Vector2f::set)
		ATOM_DECLARE_METHOD(almostEqual, ATOM_Vector2f::almostEqual)
		ATOM_DECLARE_METHOD(getLength, ATOM_Vector2f::getLength)
		ATOM_DECLARE_METHOD(getSquaredLength, ATOM_Vector2f::getSquaredLength)
		ATOM_DECLARE_METHOD(normalize, ATOM_Vector2f::normalize)
		ATOM_DECLARE_METHOD(normalizeFrom, ATOM_Vector2f::normalizeFrom)
		ATOM_DECLARE_ADD_OP(Vector_Add<ATOM_Vector2f>)
		ATOM_DECLARE_SUB_OP(Vector_Sub<ATOM_Vector2f>)
		ATOM_DECLARE_MUL_OP(Vector_Mul<ATOM_Vector2f>)
		ATOM_DECLARE_DIV_OP(Vector_Div<ATOM_Vector2f>)
		ATOM_DECLARE_NEG_OP(Vector_Neg<ATOM_Vector2f>)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_Vector2f>)
		ATOM_DECLARE_INDEX_METHOD(Vector_Index<ATOM_Vector2f>)
		ATOM_DECLARE_NEWINDEX_METHOD(Vector_NewIndex<ATOM_Vector2f>)
		ATOM_DECLARE_STATIC_METHOD(dotProduct, Vector_Dot<ATOM_Vector2f>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_Vector3f, ATOM_Vector3f)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_Vector3f, x, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_Vector3f, y, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(z, ATOM_Vector3f, z, float)
		ATOM_DECLARE_CONSTRUCTOR(Vector3f_Constructor)
		ATOM_DECLARE_METHOD(set, ATOM_Vector3f::set)
		ATOM_DECLARE_METHOD(almostEqual, ATOM_Vector3f::almostEqual)
		ATOM_DECLARE_METHOD(getLength, ATOM_Vector3f::getLength)
		ATOM_DECLARE_METHOD(getSquaredLength, ATOM_Vector3f::getSquaredLength)
		ATOM_DECLARE_METHOD(normalize, ATOM_Vector3f::normalize)
		ATOM_DECLARE_METHOD(normalizeFrom, ATOM_Vector3f::normalizeFrom)
		ATOM_DECLARE_ADD_OP(Vector_Add<ATOM_Vector3f>)
		ATOM_DECLARE_SUB_OP(Vector_Sub<ATOM_Vector3f>)
		ATOM_DECLARE_MUL_OP(Vector_Mul<ATOM_Vector3f>)
		ATOM_DECLARE_DIV_OP(Vector_Div<ATOM_Vector3f>)
		ATOM_DECLARE_NEG_OP(Vector_Neg<ATOM_Vector3f>)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_Vector3f>)
		ATOM_DECLARE_INDEX_METHOD(Vector_Index<ATOM_Vector3f>)
		ATOM_DECLARE_NEWINDEX_METHOD(Vector_NewIndex<ATOM_Vector3f>)
		ATOM_DECLARE_STATIC_METHOD(dotProduct, Vector_Dot<ATOM_Vector3f>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_Vector4f, ATOM_Vector4f)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_Vector4f, x, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_Vector4f, y, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(z, ATOM_Vector4f, z, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(w, ATOM_Vector4f, w, float)
		ATOM_DECLARE_CONSTRUCTOR(Vector4f_Constructor)
		ATOM_DECLARE_METHOD(set, ATOM_Vector4f::set)
		ATOM_DECLARE_METHOD(almostEqual, ATOM_Vector4f::almostEqual)
		ATOM_DECLARE_METHOD(getLength, ATOM_Vector4f::getLength)
		ATOM_DECLARE_METHOD(getSquaredLength, ATOM_Vector4f::getSquaredLength)
		ATOM_DECLARE_METHOD(normalize, ATOM_Vector4f::normalize)
		ATOM_DECLARE_METHOD(normalizeFrom, ATOM_Vector4f::normalizeFrom)
		ATOM_DECLARE_ADD_OP(Vector_Add<ATOM_Vector4f>)
		ATOM_DECLARE_SUB_OP(Vector_Sub<ATOM_Vector4f>)
		ATOM_DECLARE_MUL_OP(Vector_Mul<ATOM_Vector4f>)
		ATOM_DECLARE_DIV_OP(Vector_Div<ATOM_Vector4f>)
		ATOM_DECLARE_NEG_OP(Vector_Neg<ATOM_Vector4f>)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_Vector4f>)
		ATOM_DECLARE_INDEX_METHOD(Vector_Index<ATOM_Vector4f>)
		ATOM_DECLARE_NEWINDEX_METHOD(Vector_NewIndex<ATOM_Vector4f>)
		ATOM_DECLARE_STATIC_METHOD(dotProduct, Vector_Dot<ATOM_Vector4f>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_Matrix3x3f, ATOM_Matrix3x3f)
		ATOM_DECLARE_RAW_ATTRIBUTE(m00, ATOM_Matrix3x3f, m00, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m01, ATOM_Matrix3x3f, m01, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m02, ATOM_Matrix3x3f, m02, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m10, ATOM_Matrix3x3f, m10, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m11, ATOM_Matrix3x3f, m11, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m12, ATOM_Matrix3x3f, m12, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m20, ATOM_Matrix3x3f, m20, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m21, ATOM_Matrix3x3f, m21, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m22, ATOM_Matrix3x3f, m22, float)
		ATOM_DECLARE_CONSTRUCTOR(Matrix33_Constructor)
		ATOM_DECLARE_METHOD(set, ATOM_Matrix3x3f::set)
		ATOM_DECLARE_METHOD(almostEqual, ATOM_Matrix3x3f::almostEqual)
		ATOM_DECLARE_METHOD(getRow, (Matrix_GetRow<ATOM_Matrix3x3f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(setRow, (Matrix_SetRow<ATOM_Matrix3x3f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(getCol, (Matrix_GetCol<ATOM_Matrix3x3f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(setCol, (Matrix_SetCol<ATOM_Matrix3x3f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(fill, ATOM_Matrix3x3f::fill)
		ATOM_DECLARE_METHOD(isNAN, ATOM_Matrix3x3f::isNAN)
		ATOM_DECLARE_METHOD(getDeterminant, ATOM_Matrix3x3f::getDeterminant)
		ATOM_DECLARE_METHOD(transpose, ATOM_Matrix3x3f::transpose)
		ATOM_DECLARE_METHOD(transposeFrom, ATOM_Matrix3x3f::transposeFrom)
		ATOM_DECLARE_METHOD(invert, ATOM_Matrix3x3f::invert)
		ATOM_DECLARE_METHOD(invertFrom, ATOM_Matrix3x3f::invertFrom)
		ATOM_DECLARE_METHOD(invertTranspose, ATOM_Matrix3x3f::invertTranspose)
		ATOM_DECLARE_METHOD(invertTransposeFrom, ATOM_Matrix3x3f::invertTransposeFrom)
		ATOM_DECLARE_METHOD(transform, Matrix33_Transform)
		ATOM_DECLARE_METHOD(makeIdentity, ATOM_Matrix3x3f::makeIdentity)
		ATOM_DECLARE_METHOD(makeRotateX, ATOM_Matrix3x3f::makeRotateX)
		ATOM_DECLARE_METHOD(makeRotateY, ATOM_Matrix3x3f::makeRotateY)
		ATOM_DECLARE_METHOD(makeRotateZ, ATOM_Matrix3x3f::makeRotateZ)
		ATOM_DECLARE_METHOD(makeRotateAngleAxis, ATOM_Matrix3x3f::makeRotateAngleAxis)
		ATOM_DECLARE_METHOD(makeRotateAngleNormalizedAxis, ATOM_Matrix3x3f::makeRotateAngleNormalizedAxis)
		ATOM_DECLARE_METHOD(makeScale, ATOM_Matrix3x3f::makeScale)
		ATOM_DECLARE_STATIC_METHOD(getIdentityMatrix, ATOM_Matrix3x3f::getIdentityMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateXMatrix, ATOM_Matrix3x3f::getRotateXMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateYMatrix, ATOM_Matrix3x3f::getRotateYMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateZMatrix, ATOM_Matrix3x3f::getRotateZMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateAngleAxisMatrix, ATOM_Matrix3x3f::getRotateAngleAxisMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateAngleNormalizedAxisMatrix, ATOM_Matrix3x3f::getRotateAngleNormalizedAxisMatrix)
		ATOM_DECLARE_STATIC_METHOD(getScaleMatrix, ATOM_Matrix3x3f::getScaleMatrix)
		ATOM_DECLARE_ADD_OP(Vector_Add<ATOM_Matrix3x3f>)
		ATOM_DECLARE_SUB_OP(Vector_Sub<ATOM_Matrix3x3f>)
		ATOM_DECLARE_MUL_OP(Vector_Mul<ATOM_Matrix3x3f>)
		ATOM_DECLARE_DIV_OP(Vector_Div<ATOM_Matrix3x3f>)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_Matrix3x3f>)
		ATOM_DECLARE_CONCAT_OP(Matrix_Mul<ATOM_Matrix3x3f>)
		ATOM_DECLARE_INDEX_METHOD(Matrix_Index<ATOM_Matrix3x3f>)
		ATOM_DECLARE_NEWINDEX_METHOD(Matrix_NewIndex<ATOM_Matrix3x3f>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_Matrix3x4f, ATOM_Matrix3x4f)
		ATOM_DECLARE_RAW_ATTRIBUTE(m00, ATOM_Matrix3x4f, m00, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m01, ATOM_Matrix3x4f, m01, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m02, ATOM_Matrix3x4f, m02, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m03, ATOM_Matrix3x4f, m03, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m10, ATOM_Matrix3x4f, m10, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m11, ATOM_Matrix3x4f, m11, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m12, ATOM_Matrix3x4f, m12, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m13, ATOM_Matrix3x4f, m13, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m20, ATOM_Matrix3x4f, m20, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m21, ATOM_Matrix3x4f, m21, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m22, ATOM_Matrix3x4f, m22, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m23, ATOM_Matrix3x4f, m23, float)
		ATOM_DECLARE_CONSTRUCTOR(Matrix43_Constructor)
		ATOM_DECLARE_METHOD(set, Matrix43_Set)
		ATOM_DECLARE_METHOD(setM33, Matrix43_SetM33)
		ATOM_DECLARE_METHOD(setM33T, Matrix43_SetM33T)
		ATOM_DECLARE_METHOD(almostEqual, ATOM_Matrix3x4f::almostEqual)
		ATOM_DECLARE_METHOD(getRow, (Matrix_GetRow<ATOM_Matrix3x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(setRow, (Matrix_SetRow<ATOM_Matrix3x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(getCol, (Matrix_GetCol<ATOM_Matrix3x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(setCol, (Matrix_SetCol<ATOM_Matrix3x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(fill, ATOM_Matrix3x3f::fill)
		ATOM_DECLARE_METHOD(getUpper3, ATOM_Matrix3x4f::getUpper3)
		ATOM_DECLARE_METHOD(setUpper3, ATOM_Matrix3x4f::setUpper3)
		ATOM_DECLARE_METHOD(isNAN, ATOM_Matrix3x4f::isNAN)
		ATOM_DECLARE_METHOD(toMatrix44, Matrix43_ToM44)
		ATOM_DECLARE_METHOD(transformPoint, (Matrix_TransformPoint<ATOM_Matrix3x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(transformPointAffine, (Matrix_TransformPointAffine<ATOM_Matrix3x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(transformPoint4, (Matrix_TransformPoint<ATOM_Matrix3x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(transformPointAffine4, (Matrix_TransformPointAffine<ATOM_Matrix3x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(transformVector, (Matrix_TransformVector<ATOM_Matrix3x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(transformVectorAffine, (Matrix_TransformVectorAffine<ATOM_Matrix3x4f, ATOM_Vector3f>))
		ATOM_DECLARE_ADD_OP(Vector_Add<ATOM_Matrix3x4f>)
		ATOM_DECLARE_SUB_OP(Vector_Sub<ATOM_Matrix3x4f>)
		ATOM_DECLARE_MUL_OP(Vector_Mul<ATOM_Matrix3x4f>)
		ATOM_DECLARE_DIV_OP(Vector_Div<ATOM_Matrix3x4f>)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_Matrix3x4f>)
		ATOM_DECLARE_CONCAT_OP(Matrix_Mul<ATOM_Matrix3x4f>)
		ATOM_DECLARE_INDEX_METHOD(Matrix_Index<ATOM_Matrix3x4f>)
		ATOM_DECLARE_NEWINDEX_METHOD(Matrix_NewIndex<ATOM_Matrix3x4f>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_Matrix4x4f, ATOM_Matrix4x4f)
		ATOM_DECLARE_RAW_ATTRIBUTE(m00, ATOM_Matrix4x4f, m00, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m01, ATOM_Matrix4x4f, m01, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m02, ATOM_Matrix4x4f, m02, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m03, ATOM_Matrix4x4f, m03, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m10, ATOM_Matrix4x4f, m10, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m11, ATOM_Matrix4x4f, m11, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m12, ATOM_Matrix4x4f, m12, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m13, ATOM_Matrix4x4f, m13, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m20, ATOM_Matrix4x4f, m20, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m21, ATOM_Matrix4x4f, m21, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m22, ATOM_Matrix4x4f, m22, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m23, ATOM_Matrix4x4f, m23, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m30, ATOM_Matrix4x4f, m30, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m31, ATOM_Matrix4x4f, m31, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m32, ATOM_Matrix4x4f, m32, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(m33, ATOM_Matrix4x4f, m33, float)
		ATOM_DECLARE_CONSTRUCTOR(Matrix44_Constructor)

		ATOM_DECLARE_METHOD(set, ATOM_Matrix4x4f::set)
		ATOM_DECLARE_METHOD(almostEqual, ATOM_Matrix3x3f::almostEqual)
		ATOM_DECLARE_METHOD(getRow, (Matrix_GetRow<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(setRow, (Matrix_SetRow<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(getRow3, (Matrix_GetRow3<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(setRow3, (Matrix_SetRow3<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(getCol, (Matrix_GetCol<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(setCol, (Matrix_SetCol<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(getCol3, (Matrix_GetCol3<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(setCol3, (Matrix_SetCol3<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(getUpper3, ATOM_Matrix4x4f::getUpper3)
		ATOM_DECLARE_METHOD(setUpper3, ATOM_Matrix4x4f::setUpper3)
		ATOM_DECLARE_METHOD(fill, ATOM_Matrix4x4f::fill)
		ATOM_DECLARE_METHOD(isNAN, ATOM_Matrix4x4f::isNAN)
		ATOM_DECLARE_METHOD(getDeterminant, ATOM_Matrix4x4f::getDeterminant)
		ATOM_DECLARE_METHOD(transpose, ATOM_Matrix4x4f::transpose)
		ATOM_DECLARE_METHOD(transposeFrom, ATOM_Matrix4x4f::transposeFrom)
		ATOM_DECLARE_METHOD(invert, ATOM_Matrix4x4f::invert)
		ATOM_DECLARE_METHOD(invertAffine, ATOM_Matrix4x4f::invertAffine)
		ATOM_DECLARE_METHOD(invertFrom, ATOM_Matrix4x4f::invertFrom)
		ATOM_DECLARE_METHOD(invertAffineFrom, ATOM_Matrix4x4f::invertAffineFrom)
		ATOM_DECLARE_METHOD(invertTranspose, ATOM_Matrix4x4f::invertTranspose)
		ATOM_DECLARE_METHOD(invertTransposeFrom, ATOM_Matrix4x4f::invertTransposeFrom)
		ATOM_DECLARE_METHOD(invertTransposeAffine, ATOM_Matrix4x4f::invertTransposeAffine)
		ATOM_DECLARE_METHOD(invertTransposeAffineFrom, ATOM_Matrix4x4f::invertTransposeAffineFrom)
		ATOM_DECLARE_METHOD(makeIdentity, ATOM_Matrix4x4f::makeIdentity)
		ATOM_DECLARE_METHOD(makeRotateX, ATOM_Matrix4x4f::makeRotateX)
		ATOM_DECLARE_METHOD(makeRotateY, ATOM_Matrix4x4f::makeRotateY)
		ATOM_DECLARE_METHOD(makeRotateZ, ATOM_Matrix4x4f::makeRotateZ)
		ATOM_DECLARE_METHOD(makeRotateAngleAxis, ATOM_Matrix4x4f::makeRotateAngleAxis)
		ATOM_DECLARE_METHOD(makeRotateAngleNormalizedAxis, ATOM_Matrix4x4f::makeRotateAngleNormalizedAxis)
		ATOM_DECLARE_METHOD(makeScale, ATOM_Matrix4x4f::makeScale)
		ATOM_DECLARE_METHOD(makeTranslate, ATOM_Matrix4x4f::makeTranslate)
		ATOM_DECLARE_METHOD(makeLookatLH, ATOM_Matrix4x4f::makeLookatLH)
		ATOM_DECLARE_METHOD(makeLookatRH, ATOM_Matrix4x4f::makeLookatRH)
		ATOM_DECLARE_METHOD(makePerspectiveFovLH, ATOM_Matrix4x4f::makePerspectiveFovLH)
		ATOM_DECLARE_METHOD(makePerspectiveFovRH, ATOM_Matrix4x4f::makePerspectiveFovRH)
		ATOM_DECLARE_METHOD(makePerspectiveFrustumLH, ATOM_Matrix4x4f::makePerspectiveFrustumLH)
		ATOM_DECLARE_METHOD(makePerspectiveFrustumRH, ATOM_Matrix4x4f::makePerspectiveFrustumRH)
		ATOM_DECLARE_METHOD(makeOrthoLH, ATOM_Matrix4x4f::makeOrthoLH)
		ATOM_DECLARE_METHOD(makeOrthoRH, ATOM_Matrix4x4f::makeOrthoRH)
		ATOM_DECLARE_METHOD(makeOrthoFrustumLH, ATOM_Matrix4x4f::makeOrthoFrustumLH)
		ATOM_DECLARE_METHOD(makeOrthoFrustumRH, ATOM_Matrix4x4f::makeOrthoFrustumRH)
		ATOM_DECLARE_METHOD(makeBillboard, ATOM_Matrix4x4f::makeBillboard)
		ATOM_DECLARE_METHOD(makeReflection, ATOM_Matrix4x4f::makeReflection)
		ATOM_DECLARE_STATIC_METHOD(getIdentityMatrix, ATOM_Matrix4x4f::getIdentityMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateXMatrix, ATOM_Matrix4x4f::getRotateXMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateYMatrix, ATOM_Matrix4x4f::getRotateYMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateZMatrix, ATOM_Matrix4x4f::getRotateZMatrix)
		ATOM_DECLARE_STATIC_METHOD(getRotateAngleAxisMatrix, ATOM_Matrix4x4f::getRotateMatrixAngleAxis)
		ATOM_DECLARE_STATIC_METHOD(getRotateAngleNormalizedAxisMatrix, ATOM_Matrix4x4f::getRotateMatrixAngleNormalizedAxis)
		ATOM_DECLARE_STATIC_METHOD(getScaleMatrix, ATOM_Matrix4x4f::getScaleMatrix)
		ATOM_DECLARE_STATIC_METHOD(getTranslateMatrix, ATOM_Matrix4x4f::getTranslateMatrix)
		ATOM_DECLARE_STATIC_METHOD(getLookatLHMatrix, ATOM_Matrix4x4f::getLookatLHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getLookatRHMatrix, ATOM_Matrix4x4f::getLookatRHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getPerspectiveFovLHMatrix, ATOM_Matrix4x4f::getPerspectiveFovLHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getPerspectiveFovRHMatrix, ATOM_Matrix4x4f::getPerspectiveFovRHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getPerspectiveFrustumLHMatrix, ATOM_Matrix4x4f::getPerspectiveFrustumLHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getPerspectiveFrustumRHMatrix, ATOM_Matrix4x4f::getPerspectiveFrustumRHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getOrthoLHMatrix, ATOM_Matrix4x4f::getOrthoLHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getOrthoRHMatrix, ATOM_Matrix4x4f::getOrthoRHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getOrthoFrustumLHMatrix, ATOM_Matrix4x4f::getOrthoFrustumLHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getOrthoFrustumRHMatrix, ATOM_Matrix4x4f::getOrthoFrustumRHMatrix)
		ATOM_DECLARE_STATIC_METHOD(getBillboardMatrix, ATOM_Matrix4x4f::getBillboardMatrix)
		ATOM_DECLARE_STATIC_METHOD(getReflectionMatrix, ATOM_Matrix4x4f::getReflectionMatrix)
		ATOM_DECLARE_METHOD(transformPoint, (Matrix_TransformPoint<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(transformPointAffine, (Matrix_TransformPointAffine<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(transformPoint4, (Matrix_TransformPoint<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(transformPointAffine4, (Matrix_TransformPointAffine<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(transformVector, (Matrix_TransformVector<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(transformVectorAffine, (Matrix_TransformVectorAffine<ATOM_Matrix4x4f, ATOM_Vector3f>))
		ATOM_DECLARE_METHOD(transformVector4, (Matrix_TransformVector<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_METHOD(transformVectorAffine4, (Matrix_TransformVectorAffine<ATOM_Matrix4x4f, ATOM_Vector4f>))
		ATOM_DECLARE_ADD_OP(Vector_Add<ATOM_Matrix4x4f>)
		ATOM_DECLARE_SUB_OP(Vector_Sub<ATOM_Matrix4x4f>)
		ATOM_DECLARE_MUL_OP(Vector_Mul<ATOM_Matrix4x4f>)
		ATOM_DECLARE_DIV_OP(Vector_Div<ATOM_Matrix4x4f>)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_Matrix4x4f>)
		ATOM_DECLARE_CONCAT_OP(Matrix_Mul<ATOM_Matrix4x4f>)
		ATOM_DECLARE_INDEX_METHOD(Matrix_Index<ATOM_Matrix4x4f>)
		ATOM_DECLARE_NEWINDEX_METHOD(Matrix_NewIndex<ATOM_Matrix4x4f>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_Quaternion, ATOM_Quaternion)
		ATOM_DECLARE_RAW_ATTRIBUTE(x, ATOM_Quaternion, x, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(y, ATOM_Quaternion, y, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(z, ATOM_Quaternion, z, float)
		ATOM_DECLARE_RAW_ATTRIBUTE(w, ATOM_Quaternion, w, float)
		ATOM_DECLARE_CONSTRUCTOR(Quaternion_Constructor)
		ATOM_DECLARE_METHOD(set, ATOM_Quaternion::set)
		ATOM_DECLARE_METHOD(normalize, ATOM_Quaternion::normalize)
		ATOM_DECLARE_METHOD(normalizeFrom, ATOM_Quaternion::normalizeFrom)
		ATOM_DECLARE_METHOD(identity, ATOM_Quaternion::identity)
		ATOM_DECLARE_METHOD(conjugate, ATOM_Quaternion::conjugate)
		ATOM_DECLARE_METHOD(conjugateFrom, ATOM_Quaternion::conjugateFrom)
		ATOM_DECLARE_METHOD(fromAxisAngle, ATOM_Quaternion::fromAxisAngle)
		ATOM_DECLARE_METHOD(toAxisAngle, Quaternion_ToAxisAngle)
		ATOM_DECLARE_METHOD(fromMatrix33, Quaternion_FromMatrix33)
		ATOM_DECLARE_METHOD(toMatrix33, Quaternion_ToMatrix33)
		ATOM_DECLARE_METHOD(fromMatrix44, Quaternion_FromMatrix44)
		ATOM_DECLARE_METHOD(toMatrix44, Quaternion_ToMatrix44)
		ATOM_DECLARE_METHOD(setRotationArc, ATOM_Quaternion::setRotationArc)
		ATOM_DECLARE_METHOD(rotateVector, Quaternion_RotateVector)
		ATOM_DECLARE_STATIC_METHOD(slerp, ATOM_Quaternion::slerp)
		ATOM_DECLARE_STATIC_METHOD(fastSlerp, ATOM_Quaternion::fastSlerp)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_Quaternion>)
		ATOM_DECLARE_CONCAT_OP(Matrix_Mul<ATOM_Quaternion>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_BBox, ATOM_BBox)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipLeft, ATOM_BBox::ClipLeft)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipRight, ATOM_BBox::ClipRight)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipBottom, ATOM_BBox::ClipBottom)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipTop, ATOM_BBox::ClipTop)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipNear, ATOM_BBox::ClipNear)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipFar, ATOM_BBox::ClipFar)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipOutside, ATOM_BBox::ClipOutside)
		ATOM_DECLARE_STATIC_INT_CONSTANT(ClipInside, ATOM_BBox::ClipInside)
		ATOM_DECLARE_STATIC_INT_CONSTANT(Clipped, ATOM_BBox::Clipped)
		ATOM_DECLARE_CONSTRUCTOR(BBox_Constructor)
		ATOM_DECLARE_METHOD(getMin, ATOM_BBox::getMin)
		ATOM_DECLARE_METHOD(getMax, ATOM_BBox::getMax)
		ATOM_DECLARE_METHOD(setMin, ATOM_BBox::setMin)
		ATOM_DECLARE_METHOD(setMax, ATOM_BBox::setMax)
		ATOM_DECLARE_METHOD(getCenter, ATOM_BBox::getCenter)
		ATOM_DECLARE_METHOD(getExtents, ATOM_BBox::getExtents)
		ATOM_DECLARE_METHOD(getSize, ATOM_BBox::getSize)
		ATOM_DECLARE_METHOD(getDiagonalSize, ATOM_BBox::getDiagonalSize)
		ATOM_DECLARE_METHOD(transform, ATOM_BBox::transform)
		ATOM_DECLARE_METHOD(intersects, ATOM_BBox::intersects)
		ATOM_DECLARE_METHOD(containsBox, BBox_ContainsBox)
		ATOM_DECLARE_METHOD(containsPoint, BBox_ContainsPoint)
		ATOM_DECLARE_METHOD(clipToBox, BBox_ClipToBox)
		ATOM_DECLARE_METHOD(clipToMatrix, BBox_ClipToMatrix)
		ATOM_DECLARE_METHOD(clipToMatrixWithContrib, BBox_ClipToMatrixWithContrib)
		ATOM_DECLARE_METHOD(intersect, BBox_Intersect)
		ATOM_DECLARE_METHOD(beginExtend, ATOM_BBox::beginExtend)
		ATOM_DECLARE_METHOD(extend, BBox_Extend)
		ATOM_DECLARE_METHOD(isValid, ATOM_BBox::isValid)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_BBox>)
	ATOM_SCRIPT_END_CLASS ()

	ATOM_SCRIPT_BEGIN_CLASS_NOPARENT(ATOM_ColorARGB, ATOM_ColorARGB)
		ATOM_DECLARE_CONSTRUCTOR(ColorARGB_Constructor)
		ATOM_DECLARE_METHOD(setRaw, ATOM_ColorARGB::setRaw)
		ATOM_DECLARE_METHOD(getRaw, ATOM_ColorARGB::getRaw)
		ATOM_DECLARE_METHOD(setFloats, ATOM_ColorARGB::setFloats)
		ATOM_DECLARE_METHOD(getFloatR, ATOM_ColorARGB::getFloatR)
		ATOM_DECLARE_METHOD(getFloatG, ATOM_ColorARGB::getFloatG)
		ATOM_DECLARE_METHOD(getFloatB, ATOM_ColorARGB::getFloatB)
		ATOM_DECLARE_METHOD(getFloatA, ATOM_ColorARGB::getFloatA)
		ATOM_DECLARE_METHOD(setBytes, ATOM_ColorARGB::setBytes)
		ATOM_DECLARE_METHOD(getByteR, ATOM_ColorARGB::getByteR)
		ATOM_DECLARE_METHOD(getByteG, ATOM_ColorARGB::getByteG)
		ATOM_DECLARE_METHOD(getByteB, ATOM_ColorARGB::getByteB)
		ATOM_DECLARE_METHOD(getByteA, ATOM_ColorARGB::getByteA)
		ATOM_DECLARE_EQ_OP(Vector_Eq<ATOM_ColorARGB>)
	ATOM_SCRIPT_END_CLASS ()

ATOM_SCRIPT_END_TYPE_TABLE

void __misc_bind_script (ATOM_Script *script)
{
	ATOM_SCRIPT_REGISTER_FUNCTION_TABLE(script, MiscFunctions);
	ATOM_SCRIPT_REGISTER_TYPE_TABLE(script, MiscTypes);
}
