#include "StdAfx.h"
#include "skinanim_sse.h"

#if ATOM3D_COMPILER_MSVC

// disable stack trace
#ifndef ATOM_NO_STACK_TRACE
#define ATOM_NO_STACK_TRACE
#endif

#define assert_16_byte_aligned(p) ATOM_ASSERT((((UINT_PTR)(p)) & 15) == 0)
#define ALIGN16(x) ATOM_ALIGN(16) x
#define ALIGN4_INIT1(X, I) ALIGN16(static X[4]) = { I, I, I, I }
#define ALIGN4_INIT4(X, I1, I2, I3, I4) ALIGN16(static X[4]) = { I1, I2, I3, I4 }
#define R_SHUFFLE_PS(x, y, z, w) (((w)&3)<<6 | ((z)&3)<<4 | ((y)&3)<<2 | ((x)&3))
#define R_SHUFFLE_D(x, y, z, w) (((w)&3)<<6 | ((z)&3)<<4 | ((y)&3)<<2 | ((x)&3))

#define IEEE_SP_ZERO 0
#define IEEE_SP_SIGN ((unsigned long) (1<<31))

#define JOINTQUAT_SIZE (12 * 4)
#define JOINTQUAT_Q_OFFSET (0 * 4)
#define JOINTQUAT_T_OFFSET (4 * 4)
#define JOINTQUAT_S_OFFSET (8 * 4)
#define JOINTMAT_SIZE (4 * 4 * 3)
#define VERTEXWEIGHT_SIZE (3 * 4)
#define VERTEXWEIGHT_W_OFFSET (0 * 4)
#define VERTEXWEIGHT_I_OFFSET (1 * 4)
#define VERTEXWEIGHT_O_OFFSET (2 * 4)
#define VERTEXSIZE (3 * 4)
#define BASEVERTEXSIZE (4 * 4)
#define JOINTSCALE_SIZE (4 * 4)

#define TRANSPOSE_4x4( reg0, reg1, reg2, reg3, reg4 ) \
__asm movaps reg4, reg2 /* reg4 = 8, 9, 10, 11 */ \
__asm unpcklps reg2, reg3 /* reg2 = 8, 12, 9, 13 */ \
__asm unpckhps reg4, reg3 /* reg4 = 10, 14, 11, 15 */ \
__asm movaps reg3, reg0 /* reg3 = 0, 1, 2, 3 */ \
__asm unpcklps reg0, reg1 /* reg0 = 0, 4, 1, 5 */ \
__asm unpckhps reg3, reg1 /* reg3 = 2, 6, 3, 7 */ \
__asm movaps reg1, reg0 /* reg1 = 0, 4, 1, 5 */ \
__asm shufps reg0, reg2, R_SHUFFLE_PS( 0, 1, 0, 1 ) /* reg0 = 0, 4, 8, 12 */ \
__asm shufps reg1, reg2, R_SHUFFLE_PS( 2, 3, 2, 3 ) /* reg1 = 1, 5, 9, 13 */ \
__asm movaps reg2, reg3 /* reg2 = 2, 6, 3, 7 */ \
__asm shufps reg2, reg4, R_SHUFFLE_PS( 0, 1, 0, 1 ) /* reg2 = 2, 6, 10, 14 */

ALIGN4_INIT1(float SIMD_SP_one, 1.f);
ALIGN4_INIT1(float SIMD_SP_negone, -1.f);
ALIGN4_INIT1(float SIMD_SP_sin_c0, -2.39e-08f );
ALIGN4_INIT1(float SIMD_SP_sin_c1, 2.7526e-06f );
ALIGN4_INIT1(float SIMD_SP_sin_c2, -1.98409e-04f );
ALIGN4_INIT1(float SIMD_SP_sin_c3, 8.3333315e-03f );
ALIGN4_INIT1(float SIMD_SP_sin_c4, -1.666666664e-01f );
ALIGN4_INIT1(float SIMD_SP_atan_c0, 0.0028662257f );
ALIGN4_INIT1(float SIMD_SP_atan_c1, -0.0161657367f );
ALIGN4_INIT1(float SIMD_SP_atan_c2, 0.0429096138f );
ALIGN4_INIT1(float SIMD_SP_atan_c3, -0.0752896400f );
ALIGN4_INIT1(float SIMD_SP_atan_c4, 0.1065626393f );
ALIGN4_INIT1(float SIMD_SP_atan_c5, -0.1420889944f );
ALIGN4_INIT1(float SIMD_SP_atan_c6, 0.1999355085f );
ALIGN4_INIT1(float SIMD_SP_atan_c7, -0.3333314528f );
ALIGN4_INIT1(float SIMD_SP_halfPI, ATOM_HalfPi);
ALIGN4_INIT1(float SIMD_SP_rsqrt_c0, 3.0f );
ALIGN4_INIT1(float SIMD_SP_rsqrt_c1, -0.5f );
ALIGN4_INIT1(float SIMD_SP_tiny, 1e-10f );
ALIGN4_INIT1(float SIMD_SP_zero, 0.0f );
ALIGN4_INIT1(unsigned long SIMD_SP_absMask, (unsigned long) ~( 1 << 31 ) );
ALIGN4_INIT1(unsigned long SIMD_SP_signBit, IEEE_SP_SIGN );
ALIGN4_INIT4(unsigned long SIMD_SP_quatsign, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN, 0);
ALIGN4_INIT4(unsigned long SIMD_SP_quat2mat_x0, IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN);
ALIGN4_INIT4(unsigned long SIMD_SP_quat2mat_x1, IEEE_SP_SIGN, IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN);
ALIGN4_INIT4(unsigned long SIMD_SP_quat2mat_x2, IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN);
ALIGN4_INIT4(unsigned long SIMD_SP_clearFirstThree, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF );

#if 0
void slerpJointFastEx (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *joints, const ATOM_JointTransformInfo *blendJoints, const float lerp, int numJoints, const unsigned *index)
{
	int i;

	if ( lerp <= 0.0f ) {
		for ( i = 0; i < numJoints; i++ ) {
			int j = index[i];
			dst[j] = joints[j];
		}
		return;
	} else if ( lerp >= 1.0f ) {
		for ( i = 0; i < numJoints; i++ ) {
			int j = index[i];
			dst[j] = blendJoints[j];
		}
		return;
	}

	for ( i = 0; i <= numJoints - 4; i += 4 ) {
		ALIGN16( float jointVert0[4] );
		ALIGN16( float jointVert1[4] );
		ALIGN16( float jointVert2[4] );
		ALIGN16( float jointScale0[4] );
		ALIGN16( float jointScale1[4] );
		ALIGN16( float jointScale2[4] );
		ALIGN16( float blendVert0[4] );
		ALIGN16( float blendVert1[4] );
		ALIGN16( float blendVert2[4] );
		ALIGN16( float blendScale0[4] );
		ALIGN16( float blendScale1[4] );
		ALIGN16( float blendScale2[4] );
		ALIGN16( float jointQuat0[4] );
		ALIGN16( float jointQuat1[4] );
		ALIGN16( float jointQuat2[4] );
		ALIGN16( float jointQuat3[4] );
		ALIGN16( float blendQuat0[4] );
		ALIGN16( float blendQuat1[4] );
		ALIGN16( float blendQuat2[4] );
		ALIGN16( float blendQuat3[4] );

		for ( int j = 0; j < 4; j++ ) {
			int n = index[i+j];

			jointVert0[j] = joints[n].translate[0];
			jointVert1[j] = joints[n].translate[1];
			jointVert2[j] = joints[n].translate[2];

			jointScale0[j] = joints[n].scale[0];
			jointScale1[j] = joints[n].scale[1];
			jointScale2[j] = joints[n].scale[2];

			blendVert0[j] = blendJoints[n].translate[0];
			blendVert1[j] = blendJoints[n].translate[1];
			blendVert2[j] = blendJoints[n].translate[2];

			blendScale0[j] = blendJoints[n].scale[0];
			blendScale1[j] = blendJoints[n].scale[1];
			blendScale2[j] = blendJoints[n].scale[2];

			jointQuat0[j] = joints[n].rotate.x;
			jointQuat1[j] = joints[n].rotate.y;
			jointQuat2[j] = joints[n].rotate.z;
			jointQuat3[j] = joints[n].rotate.w;

			blendQuat0[j] = blendJoints[n].rotate.x;
			blendQuat1[j] = blendJoints[n].rotate.y;
			blendQuat2[j] = blendJoints[n].rotate.z;
			blendQuat3[j] = blendJoints[n].rotate.w;
		}

#if 1
		__asm {
			// lerp translation
			movss		xmm7, lerp
			shufps		xmm7, xmm7, R_SHUFFLE_PS( 0, 0, 0, 0 )
			movaps		xmm0, blendVert0
			subps		xmm0, jointVert0
			mulps		xmm0, xmm7
			addps		xmm0, jointVert0
			movaps		jointVert0, xmm0
			movaps		xmm1, blendVert1
			subps		xmm1, jointVert1
			mulps		xmm1, xmm7
			addps		xmm1, jointVert1
			movaps		jointVert1, xmm1
			movaps		xmm2, blendVert2
			subps		xmm2, jointVert2
			mulps		xmm2, xmm7
			addps		xmm2, jointVert2
			movaps		jointVert2, xmm2

			// lerp scale
			movss		xmm7, lerp
			shufps		xmm7, xmm7, R_SHUFFLE_PS( 0, 0, 0, 0 )
			movaps		xmm0, blendScale0
			subps		xmm0, jointScale0
			mulps		xmm0, xmm7
			addps		xmm0, jointScale0
			movaps		jointScale0, xmm0
			movaps		xmm1, blendScale1
			subps		xmm1, jointScale1
			mulps		xmm1, xmm7
			addps		xmm1, jointScale1
			movaps		jointScale1, xmm1
			movaps		xmm2, blendScale2
			subps		xmm2, jointScale2
			mulps		xmm2, xmm7
			addps		xmm2, jointScale2
			movaps		jointScale2, xmm2

			// lerp quaternions
			movaps		xmm0, jointQuat0
			mulps		xmm0, blendQuat0
			movaps		xmm1, jointQuat1
			mulps		xmm1, blendQuat1
			addps		xmm0, xmm1
			movaps		xmm2, jointQuat2
			mulps		xmm2, blendQuat2
			addps		xmm0, xmm2
			movaps		xmm3, jointQuat3
			mulps		xmm3, blendQuat3
			addps		xmm0, xmm3					// xmm0 = cosom

			movaps		xmm1, xmm0
			movaps		xmm2, xmm0
			andps		xmm1, SIMD_SP_signBit	// xmm1 = signBit
			xorps		xmm0, xmm1
			mulps		xmm2, xmm2

			xorps		xmm4, xmm4
			movaps		xmm3, SIMD_SP_one
			subps		xmm3, xmm2					// xmm3 = scale0
			cmpeqps		xmm4, xmm3
			andps		xmm4, SIMD_SP_tiny			// if values are zero replace them with a tiny number
			andps		xmm3, SIMD_SP_absMask		// make sure the values are positive
			orps		xmm3, xmm4

			movaps		xmm2, xmm3
			rsqrtps		xmm4, xmm2
			mulps		xmm2, xmm4
			mulps		xmm2, xmm4
			subps		xmm2, SIMD_SP_rsqrt_c0
			mulps		xmm4, SIMD_SP_rsqrt_c1
			mulps		xmm2, xmm4
			mulps		xmm3, xmm2					// xmm3 = sqrt( scale0 )

			// omega0 = atan2( xmm3, xmm0 )
			movaps		xmm4, xmm0
			minps		xmm0, xmm3
			maxps		xmm3, xmm4
			cmpeqps		xmm4, xmm0

			rcpps		xmm5, xmm3
			mulps		xmm3, xmm5
			mulps		xmm3, xmm5
			addps		xmm5, xmm5
			subps		xmm5, xmm3					// xmm5 = 1 / y or 1 / x
			mulps		xmm0, xmm5					// xmm0 = x / y or y / x
			movaps		xmm3, xmm4
			andps		xmm3, SIMD_SP_signBit
			xorps		xmm0, xmm3					// xmm0 = -x / y or y / x
			andps		xmm4, SIMD_SP_halfPI		// xmm4 = HALF_PI or 0.0f
			movaps		xmm3, xmm0
			mulps		xmm3, xmm3					// xmm3 = s
			movaps		xmm5, SIMD_SP_atan_c0
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_atan_c1
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_atan_c2
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_atan_c3
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_atan_c4
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_atan_c5
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_atan_c6
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_atan_c7
			mulps		xmm5, xmm3
			addps		xmm5, SIMD_SP_one
			mulps		xmm5, xmm0
			addps		xmm5, xmm4					// xmm5 = omega0

			movaps		xmm6, xmm7					// xmm6 = lerp
			mulps		xmm6, xmm5					// xmm6 = omega1
			subps		xmm5, xmm6					// xmm5 = omega0

			// scale0 = sin( xmm5 ) * xmm2
			// scale1 = sin( xmm6 ) * xmm2
			movaps		xmm3, xmm5
			movaps		xmm7, xmm6
			mulps		xmm3, xmm3
			mulps		xmm7, xmm7
			movaps		xmm4, SIMD_SP_sin_c0
			movaps		xmm0, SIMD_SP_sin_c0
			mulps		xmm4, xmm3
			mulps		xmm0, xmm7
			addps		xmm4, SIMD_SP_sin_c1
			addps		xmm0, SIMD_SP_sin_c1
			mulps		xmm4, xmm3
			mulps		xmm0, xmm7
			addps		xmm4, SIMD_SP_sin_c2
			addps		xmm0, SIMD_SP_sin_c2
			mulps		xmm4, xmm3
			mulps		xmm0, xmm7
			addps		xmm4, SIMD_SP_sin_c3
			addps		xmm0, SIMD_SP_sin_c3
			mulps		xmm4, xmm3
			mulps		xmm0, xmm7
			addps		xmm4, SIMD_SP_sin_c4
			addps		xmm0, SIMD_SP_sin_c4
			mulps		xmm4, xmm3
			mulps		xmm0, xmm7
			addps		xmm4, SIMD_SP_one
			addps		xmm0, SIMD_SP_one
			mulps		xmm5, xmm4
			mulps		xmm6, xmm0
			mulps		xmm5, xmm2					// xmm5 = scale0
			mulps		xmm6, xmm2					// xmm6 = scale1

			xorps		xmm6, xmm1

			movaps		xmm0, jointQuat0
			mulps		xmm0, xmm5
			movaps		xmm1, blendQuat0
			mulps		xmm1, xmm6
			addps		xmm0, xmm1
			movaps		jointQuat0, xmm0

			movaps		xmm1, jointQuat1
			mulps		xmm1, xmm5
			movaps		xmm2, blendQuat1
			mulps		xmm2, xmm6
			addps		xmm1, xmm2
			movaps		jointQuat1, xmm1

			movaps		xmm2, jointQuat2
			mulps		xmm2, xmm5
			movaps		xmm3, blendQuat2
			mulps		xmm3, xmm6
			addps		xmm2, xmm3
			movaps		jointQuat2, xmm2

			movaps		xmm3, jointQuat3
			mulps		xmm3, xmm5
			movaps		xmm4, blendQuat3
			mulps		xmm4, xmm6
			addps		xmm3, xmm4
			movaps		jointQuat3, xmm3
		}

#else

		jointVert0[0] += lerp * ( blendVert0[0] - jointVert0[0] );
		jointVert0[1] += lerp * ( blendVert0[1] - jointVert0[1] );
		jointVert0[2] += lerp * ( blendVert0[2] - jointVert0[2] );
		jointVert0[3] += lerp * ( blendVert0[3] - jointVert0[3] );

		jointVert1[0] += lerp * ( blendVert1[0] - jointVert1[0] );
		jointVert1[1] += lerp * ( blendVert1[1] - jointVert1[1] );
		jointVert1[2] += lerp * ( blendVert1[2] - jointVert1[2] );
		jointVert1[3] += lerp * ( blendVert1[3] - jointVert1[3] );

		jointVert2[0] += lerp * ( blendVert2[0] - jointVert2[0] );
		jointVert2[1] += lerp * ( blendVert2[1] - jointVert2[1] );
		jointVert2[2] += lerp * ( blendVert2[2] - jointVert2[2] );
		jointVert2[3] += lerp * ( blendVert2[3] - jointVert2[3] );

		ALIGN16( float cosom[4] );
		ALIGN16( float sinom[4] );
		ALIGN16( float omega0[4] );
		ALIGN16( float omega1[4] );
		ALIGN16( float scale0[4] );
		ALIGN16( float scale1[4] );
		ALIGN16( unsigned long signBit[4] );

		cosom[0] = jointQuat0[0] * blendQuat0[0];
		cosom[1] = jointQuat0[1] * blendQuat0[1];
		cosom[2] = jointQuat0[2] * blendQuat0[2];
		cosom[3] = jointQuat0[3] * blendQuat0[3];

		cosom[0] += jointQuat1[0] * blendQuat1[0];
		cosom[1] += jointQuat1[1] * blendQuat1[1];
		cosom[2] += jointQuat1[2] * blendQuat1[2];
		cosom[3] += jointQuat1[3] * blendQuat1[3];

		cosom[0] += jointQuat2[0] * blendQuat2[0];
		cosom[1] += jointQuat2[1] * blendQuat2[1];
		cosom[2] += jointQuat2[2] * blendQuat2[2];
		cosom[3] += jointQuat2[3] * blendQuat2[3];

		cosom[0] += jointQuat3[0] * blendQuat3[0];
		cosom[1] += jointQuat3[1] * blendQuat3[1];
		cosom[2] += jointQuat3[2] * blendQuat3[2];
		cosom[3] += jointQuat3[3] * blendQuat3[3];

		signBit[0] = (*(unsigned long *)&cosom[0]) & ( 1 << 31 );
		signBit[1] = (*(unsigned long *)&cosom[1]) & ( 1 << 31 );
		signBit[2] = (*(unsigned long *)&cosom[2]) & ( 1 << 31 );
		signBit[3] = (*(unsigned long *)&cosom[3]) & ( 1 << 31 );

		(*(unsigned long *)&cosom[0]) ^= signBit[0];
		(*(unsigned long *)&cosom[1]) ^= signBit[1];
		(*(unsigned long *)&cosom[2]) ^= signBit[2];
		(*(unsigned long *)&cosom[3]) ^= signBit[3];

		scale0[0] = 1.0f - cosom[0] * cosom[0];
		scale0[1] = 1.0f - cosom[1] * cosom[1];
		scale0[2] = 1.0f - cosom[2] * cosom[2];
		scale0[3] = 1.0f - cosom[3] * cosom[3];

		scale0[0] = ( scale0[0] <= 0.0f ) ? SIMD_SP_tiny[0] : scale0[0];
		scale0[1] = ( scale0[1] <= 0.0f ) ? SIMD_SP_tiny[1] : scale0[1];
		scale0[2] = ( scale0[2] <= 0.0f ) ? SIMD_SP_tiny[2] : scale0[2];
		scale0[3] = ( scale0[3] <= 0.0f ) ? SIMD_SP_tiny[3] : scale0[3];

		sinom[0] = idMath::RSqrt( scale0[0] );
		sinom[1] = idMath::RSqrt( scale0[1] );
		sinom[2] = idMath::RSqrt( scale0[2] );
		sinom[3] = idMath::RSqrt( scale0[3] );

		scale0[0] *= sinom[0];
		scale0[1] *= sinom[1];
		scale0[2] *= sinom[2];
		scale0[3] *= sinom[3];

		omega0[0] = SSE_ATanPositive( scale0[0], cosom[0] );
		omega0[1] = SSE_ATanPositive( scale0[1], cosom[1] );
		omega0[2] = SSE_ATanPositive( scale0[2], cosom[2] );
		omega0[3] = SSE_ATanPositive( scale0[3], cosom[3] );

		omega1[0] = lerp * omega0[0];
		omega1[1] = lerp * omega0[1];
		omega1[2] = lerp * omega0[2];
		omega1[3] = lerp * omega0[3];

		omega0[0] -= omega1[0];
		omega0[1] -= omega1[1];
		omega0[2] -= omega1[2];
		omega0[3] -= omega1[3];

		scale0[0] = SSE_SinZeroHalfPI( omega0[0] ) * sinom[0];
		scale0[1] = SSE_SinZeroHalfPI( omega0[1] ) * sinom[1];
		scale0[2] = SSE_SinZeroHalfPI( omega0[2] ) * sinom[2];
		scale0[3] = SSE_SinZeroHalfPI( omega0[3] ) * sinom[3];

		scale1[0] = SSE_SinZeroHalfPI( omega1[0] ) * sinom[0];
		scale1[1] = SSE_SinZeroHalfPI( omega1[1] ) * sinom[1];
		scale1[2] = SSE_SinZeroHalfPI( omega1[2] ) * sinom[2];
		scale1[3] = SSE_SinZeroHalfPI( omega1[3] ) * sinom[3];

		(*(unsigned long *)&scale1[0]) ^= signBit[0];
		(*(unsigned long *)&scale1[1]) ^= signBit[1];
		(*(unsigned long *)&scale1[2]) ^= signBit[2];
		(*(unsigned long *)&scale1[3]) ^= signBit[3];

		jointQuat0[0] = scale0[0] * jointQuat0[0] + scale1[0] * blendQuat0[0];
		jointQuat0[1] = scale0[1] * jointQuat0[1] + scale1[1] * blendQuat0[1];
		jointQuat0[2] = scale0[2] * jointQuat0[2] + scale1[2] * blendQuat0[2];
		jointQuat0[3] = scale0[3] * jointQuat0[3] + scale1[3] * blendQuat0[3];

		jointQuat1[0] = scale0[0] * jointQuat1[0] + scale1[0] * blendQuat1[0];
		jointQuat1[1] = scale0[1] * jointQuat1[1] + scale1[1] * blendQuat1[1];
		jointQuat1[2] = scale0[2] * jointQuat1[2] + scale1[2] * blendQuat1[2];
		jointQuat1[3] = scale0[3] * jointQuat1[3] + scale1[3] * blendQuat1[3];

		jointQuat2[0] = scale0[0] * jointQuat2[0] + scale1[0] * blendQuat2[0];
		jointQuat2[1] = scale0[1] * jointQuat2[1] + scale1[1] * blendQuat2[1];
		jointQuat2[2] = scale0[2] * jointQuat2[2] + scale1[2] * blendQuat2[2];
		jointQuat2[3] = scale0[3] * jointQuat2[3] + scale1[3] * blendQuat2[3];

		jointQuat3[0] = scale0[0] * jointQuat3[0] + scale1[0] * blendQuat3[0];
		jointQuat3[1] = scale0[1] * jointQuat3[1] + scale1[1] * blendQuat3[1];
		jointQuat3[2] = scale0[2] * jointQuat3[2] + scale1[2] * blendQuat3[2];
		jointQuat3[3] = scale0[3] * jointQuat3[3] + scale1[3] * blendQuat3[3];

#endif

		for ( int j = 0; j < 4; j++ ) {
			int n = index[i+j];

			dst[n].translate[0] = jointVert0[j];
			dst[n].translate[1] = jointVert1[j];
			dst[n].translate[2] = jointVert2[j];
			dst[n].translate[3] = 1.f;

#if 0
			dst[n].scale = joints[n].scale + lerp * ( blendJoints[n].scale - joints[n].scale );
#else
			dst[n].scale[0] = jointScale0[j];
			dst[n].scale[1] = jointScale1[j];
			dst[n].scale[2] = jointScale2[j];
			dst[n].scale[3] = 1.f;
#endif

#if 0
			dst[n].rotate = ATOM_Quaternion::slerp (joints[n].rotate, blendJoints[n].rotate, lerp);
#else
			dst[n].rotate.x = jointQuat0[j];
			dst[n].rotate.y = jointQuat1[j];
			dst[n].rotate.z = jointQuat2[j];
			dst[n].rotate.w = jointQuat3[j];
#endif
		}
	}

	for ( ; i < numJoints; i++ ) {
		int n = index[i];

		dst[n].translate = joints[n].translate + lerp * ( blendJoints[n].translate - joints[n].translate );
		dst[n].scale = joints[n].scale + lerp * ( blendJoints[n].scale - joints[n].scale );
		dst[n].rotate = ATOM_Quaternion::slerp (joints[n].rotate, blendJoints[n].rotate, lerp);
	}
}
#endif

/*
void slerpJointFast (ATOM_JointTransformInfo *joints, const ATOM_JointTransformInfo *blendJoints, const float lerp, int numJoints, const unsigned *index)
{
	ATOM_STACK_TRACE(slerpJointFast);
	assert_16_byte_aligned( joints );
	assert_16_byte_aligned( blendJoints );
	assert_16_byte_aligned( JOINTQUAT_Q_OFFSET );
	assert_16_byte_aligned( JOINTQUAT_T_OFFSET );
	ALIGN16( float jointQuat0[4]; )
	ALIGN16( float jointQuat1[4]; )
	ALIGN16( float jointQuat2[4]; )
	ALIGN16( float jointQuat3[4]; )
	ALIGN16( float blendQuat0[4]; )
	ALIGN16( float blendQuat1[4]; )
	ALIGN16( float blendQuat2[4]; )
	ALIGN16( float blendQuat3[4]; )
	int a0, a1, a2, a3;
	__asm {
		movss xmm7, lerp
		cmpnless xmm7, SIMD_SP_zero
		movmskps ecx, xmm7
		test ecx, 1
		jz done1
		mov eax, numJoints
		shl eax, 2
		mov esi, joints
		mov edi, blendJoints
		mov edx, index
		add edx, eax
		neg eax
		jz done1
		movss xmm7, lerp
		cmpnltss xmm7, SIMD_SP_one
		movmskps ecx, xmm7
		test ecx, 1
		jz lerpJoints
	loopCopy:
		mov ecx, [edx+eax]
		shl ecx, 4
		lea ecx, [ecx+ecx*2]
		add eax, 1*4
		movaps xmm0, [edi+ecx+JOINTQUAT_Q_OFFSET]
		movaps xmm1, [edi+ecx+JOINTQUAT_T_OFFSET]
		movaps xmm2, [edi+ecx+JOINTQUAT_S_OFFSET]
		movaps [esi+ecx+JOINTQUAT_Q_OFFSET], xmm0
		movaps [esi+ecx+JOINTQUAT_T_OFFSET], xmm1
		movaps [esi+ecx+JOINTQUAT_S_OFFSET], xmm2
		jl loopCopy
		jmp done1
	lerpJoints:
		add eax, 4*4
		jge done4
	loopJoint4:
		movss xmm3, lerp
		shufps xmm3, xmm3, R_SHUFFLE_PS( 0, 0, 0, 0 )
		mov ecx, [edx+eax-4*4]
		shl ecx, 4
		lea ecx, [ecx+ecx*2]
		mov a0, ecx
		// lerp first translations
		movaps xmm7, [edi+ecx+JOINTQUAT_T_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		movaps [esi+ecx+JOINTQUAT_T_OFFSET], xmm7
		// lerp first scales
		movaps xmm7, [edi+ecx+JOINTQUAT_S_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		movaps [esi+ecx+JOINTQUAT_S_OFFSET], xmm7
		// load first quaternions
		movaps xmm0, [esi+ecx+JOINTQUAT_Q_OFFSET]
		movaps xmm4, [edi+ecx+JOINTQUAT_Q_OFFSET]
		mov ecx, [edx+eax-3*4]
		shl ecx, 4
		lea ecx, [ecx+ecx*2]
		mov a1, ecx
		// lerp second translations
		movaps xmm7, [edi+ecx+JOINTQUAT_T_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		movaps [esi+ecx+JOINTQUAT_T_OFFSET], xmm7
		// lerp second scales
		movaps xmm7, [edi+ecx+JOINTQUAT_S_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		movaps [esi+ecx+JOINTQUAT_S_OFFSET], xmm7
		// load second quaternions
		movaps xmm1, [esi+ecx+JOINTQUAT_Q_OFFSET]
		movaps xmm5, [edi+ecx+JOINTQUAT_Q_OFFSET]
		mov ecx, [edx+eax-2*4]
		shl ecx, 4
		lea ecx, [ecx+ecx*2]
		mov a2, ecx
		// lerp third translations
		movaps xmm7, [edi+ecx+JOINTQUAT_T_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		movaps [esi+ecx+JOINTQUAT_T_OFFSET], xmm7
		// lerp third scales
		movaps xmm7, [edi+ecx+JOINTQUAT_S_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		movaps [esi+ecx+JOINTQUAT_S_OFFSET], xmm7
		// load third quaternions
		movaps xmm2, [esi+ecx+JOINTQUAT_Q_OFFSET]
		movaps xmm6, [edi+ecx+JOINTQUAT_Q_OFFSET]
		mov ecx, [edx+eax-1*4]
		shl ecx, 4
		lea ecx, [ecx+ecx*2]
		mov a3, ecx
		// lerp fourth translations
		movaps xmm7, [edi+ecx+JOINTQUAT_T_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		movaps [esi+ecx+JOINTQUAT_T_OFFSET], xmm7
		// lerp fourth scales
		movaps xmm7, [edi+ecx+JOINTQUAT_S_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		movaps [esi+ecx+JOINTQUAT_S_OFFSET], xmm7
		// load fourth quaternions
		movaps xmm3, [esi+ecx+JOINTQUAT_Q_OFFSET]
		TRANSPOSE_4x4( xmm0, xmm1, xmm2, xmm3, xmm7 )
		movaps jointQuat0, xmm0
		movaps jointQuat1, xmm1
		movaps jointQuat2, xmm2
		movaps jointQuat3, xmm3
		movaps xmm7, [edi+ecx+JOINTQUAT_Q_OFFSET]
		TRANSPOSE_4x4( xmm4, xmm5, xmm6, xmm7, xmm3 )
		movaps blendQuat0, xmm4
		movaps blendQuat1, xmm5
		movaps blendQuat2, xmm6
		movaps blendQuat3, xmm7
		// lerp quaternions
		mulps xmm0, xmm4
		mulps xmm1, xmm5
		addps xmm0, xmm1
		mulps xmm2, xmm6
		addps xmm0, xmm2
		movaps xmm3, jointQuat3
		mulps xmm3, blendQuat3
		addps xmm0, xmm3 // xmm0 = cosom
		movaps xmm1, xmm0
		movaps xmm2, xmm0
		andps xmm1, SIMD_SP_signBit // xmm1 = signBit
		xorps xmm0, xmm1
		mulps xmm2, xmm2
		xorps xmm4, xmm4
		movaps xmm3, SIMD_SP_one
		subps xmm3, xmm2 // xmm3 = scale0
		cmpeqps xmm4, xmm3
		andps xmm4, SIMD_SP_tiny // if values are zero replace them with a tiny number
		andps xmm3, SIMD_SP_absMask // make sure the values are positive
		orps xmm3, xmm4
		movaps xmm2, xmm3
		rsqrtps xmm4, xmm2
		mulps xmm2, xmm4
		mulps xmm2, xmm4
		subps xmm2, SIMD_SP_rsqrt_c0
		mulps xmm4, SIMD_SP_rsqrt_c1
		mulps xmm2, xmm4
		mulps xmm3, xmm2 // xmm3 = sqrt( scale0 )
		// omega0 = atan2( xmm3, xmm0 )
		movaps xmm4, xmm0
		minps xmm0, xmm3
		maxps xmm3, xmm4
		cmpeqps xmm4, xmm0
		rcpps xmm5, xmm3
		mulps xmm3, xmm5
		mulps xmm3, xmm5
		addps xmm5, xmm5
		subps xmm5, xmm3 // xmm5 = 1 / y or 1 / x
		mulps xmm0, xmm5 // xmm0 = x / y or y / x
		movaps xmm3, xmm4
		andps xmm3, SIMD_SP_signBit
		xorps xmm0, xmm3 // xmm0 = -x / y or y / x
		andps xmm4, SIMD_SP_halfPI // xmm4 = HALF_PI or 0.0f
		movaps xmm3, xmm0
		mulps xmm3, xmm3 // xmm3 = s
		movaps xmm5, SIMD_SP_atan_c0
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_atan_c1
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_atan_c2
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_atan_c3
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_atan_c4
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_atan_c5
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_atan_c6
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_atan_c7
		mulps xmm5, xmm3
		addps xmm5, SIMD_SP_one
		mulps xmm5, xmm0
		addps xmm5, xmm4 // xmm5 = omega0
		movss xmm6, lerp // xmm6 = lerp
		shufps xmm6, xmm6, R_SHUFFLE_PS( 0, 0, 0, 0 )
		mulps xmm6, xmm5 // xmm6 = omega1
		subps xmm5, xmm6 // xmm5 = omega0
		// scale0 = sin( xmm5 ) * xmm2
		// scale1 = sin( xmm6 ) * xmm2
		movaps xmm3, xmm5
		movaps xmm7, xmm6
		mulps xmm3, xmm3
		mulps xmm7, xmm7
		movaps xmm4, SIMD_SP_sin_c0
		movaps xmm0, SIMD_SP_sin_c0
		mulps xmm4, xmm3
		mulps xmm0, xmm7
		addps xmm4, SIMD_SP_sin_c1
		addps xmm0, SIMD_SP_sin_c1
		mulps xmm4, xmm3
		mulps xmm0, xmm7
		addps xmm4, SIMD_SP_sin_c2
		addps xmm0, SIMD_SP_sin_c2
		mulps xmm4, xmm3
		mulps xmm0, xmm7
		addps xmm4, SIMD_SP_sin_c3
		addps xmm0, SIMD_SP_sin_c3
		mulps xmm4, xmm3
		mulps xmm0, xmm7
		addps xmm4, SIMD_SP_sin_c4
		addps xmm0, SIMD_SP_sin_c4
		mulps xmm4, xmm3
		mulps xmm0, xmm7
		addps xmm4, SIMD_SP_one
		addps xmm0, SIMD_SP_one
		mulps xmm5, xmm4
		mulps xmm6, xmm0
		mulps xmm5, xmm2 // xmm5 = scale0
		mulps xmm6, xmm2 // xmm6 = scale1
		xorps xmm6, xmm1
		movaps xmm0, jointQuat0
		mulps xmm0, xmm5
		movaps xmm1, blendQuat0
		mulps xmm1, xmm6
		addps xmm0, xmm1
		movaps xmm1, jointQuat1
		mulps xmm1, xmm5
		movaps xmm2, blendQuat1
		mulps xmm2, xmm6
		addps xmm1, xmm2
		movaps xmm2, jointQuat2
		mulps xmm2, xmm5
		movaps xmm3, blendQuat2
		mulps xmm3, xmm6
		addps xmm2, xmm3
		movaps xmm3, jointQuat3
		mulps xmm3, xmm5
		movaps xmm4, blendQuat3
		mulps xmm4, xmm6
		addps xmm3, xmm4
		add eax, 4*4
		// transpose xmm0, xmm1, xmm2, xmm3 to memory
		movaps xmm7, xmm0
		movaps xmm6, xmm2
		unpcklps xmm0, xmm1
		unpcklps xmm2, xmm3
		mov ecx, a0
		movlps [esi+ecx+JOINTQUAT_Q_OFFSET+0], xmm0
		movlps [esi+ecx+JOINTQUAT_Q_OFFSET+8], xmm2
		mov ecx, a1
		movhps [esi+ecx+JOINTQUAT_Q_OFFSET+0], xmm0
		movhps [esi+ecx+JOINTQUAT_Q_OFFSET+8], xmm2
		unpckhps xmm7, xmm1
		unpckhps xmm6, xmm3
		mov ecx, a2
		movlps [esi+ecx+JOINTQUAT_Q_OFFSET+0], xmm7
		movlps [esi+ecx+JOINTQUAT_Q_OFFSET+8], xmm6
		mov ecx, a3
		movhps [esi+ecx+JOINTQUAT_Q_OFFSET+0], xmm7
		movhps [esi+ecx+JOINTQUAT_Q_OFFSET+8], xmm6
		jle loopJoint4
	done4:
		sub eax, 4*4
		jz done1
	loopJoint1:
		movss xmm3, lerp
		shufps xmm3, xmm3, R_SHUFFLE_PS( 0, 0, 0, 0 )
		mov ecx, [edx+eax]
		shl ecx, 4
		lea ecx, [ecx+ecx*2]
		// lerp first translations
		movaps xmm7, [edi+ecx+JOINTQUAT_T_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_T_OFFSET]
		movaps [esi+ecx+JOINTQUAT_T_OFFSET], xmm7
		// lerp first translations
		movaps xmm7, [edi+ecx+JOINTQUAT_S_OFFSET]
		subps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		mulps xmm7, xmm3
		addps xmm7, [esi+ecx+JOINTQUAT_S_OFFSET]
		movaps [esi+ecx+JOINTQUAT_S_OFFSET], xmm7
		// load first quaternions
		movaps xmm0, [esi+ecx+JOINTQUAT_Q_OFFSET]
		movaps xmm1, [edi+ecx+JOINTQUAT_Q_OFFSET]
		movaps jointQuat0, xmm0
		movaps blendQuat0, xmm1
		// lerp quaternions
		mulps xmm1, xmm0
		movhlps xmm0, xmm1
		addps xmm1, xmm0
		movaps xmm0, xmm1
		shufps xmm0, xmm0, R_SHUFFLE_PS( 1, 0, 2, 3 )
		addss xmm0, xmm1 // xmm0 = cosom
		movss xmm1, xmm0
		movss xmm2, xmm0
		andps xmm1, SIMD_SP_signBit // xmm1 = signBit
		xorps xmm0, xmm1
		mulss xmm2, xmm2
		xorps xmm4, xmm4
		movss xmm3, SIMD_SP_one
		subss xmm3, xmm2 // xmm3 = scale0
		cmpeqss xmm4, xmm3
		andps xmm4, SIMD_SP_tiny // if values are zero replace them with a tiny number
		andps xmm3, SIMD_SP_absMask // make sure the values are positive
		orps xmm3, xmm4
		movss xmm2, xmm3
		rsqrtss xmm4, xmm2
		mulss xmm2, xmm4
		mulss xmm2, xmm4
		subss xmm2, SIMD_SP_rsqrt_c0
		mulss xmm4, SIMD_SP_rsqrt_c1
		mulss xmm2, xmm4
		mulss xmm3, xmm2 // xmm3 = sqrt( scale0 )
		// omega0 = atan2( xmm3, xmm0 )
		movss xmm4, xmm0
		minss xmm0, xmm3
		maxss xmm3, xmm4
		cmpeqss xmm4, xmm0
		rcpss xmm5, xmm3
		mulss xmm3, xmm5
		mulss xmm3, xmm5
		addss xmm5, xmm5
		subss xmm5, xmm3 // xmm5 = 1 / y or 1 / x
		mulss xmm0, xmm5 // xmm0 = x / y or y / x
		movss xmm3, xmm4
		andps xmm3, SIMD_SP_signBit
		xorps xmm0, xmm3 // xmm0 = -x / y or y / x
		andps xmm4, SIMD_SP_halfPI // xmm4 = HALF_PI or 0.0f
		movss xmm3, xmm0
		mulss xmm3, xmm3 // xmm3 = s
		movss xmm5, SIMD_SP_atan_c0
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_atan_c1
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_atan_c2
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_atan_c3
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_atan_c4
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_atan_c5
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_atan_c6
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_atan_c7
		mulss xmm5, xmm3
		addss xmm5, SIMD_SP_one
		mulss xmm5, xmm0
		addss xmm5, xmm4 // xmm5 = omega0
		movss xmm6, lerp // xmm6 = lerp
		mulss xmm6, xmm5 // xmm6 = omega1
		subss xmm5, xmm6 // xmm5 = omega0
		// scale0 = sin( xmm5 ) * xmm2
		// scale1 = sin( xmm6 ) * xmm2
		movss xmm3, xmm5
		movss xmm7, xmm6
		mulss xmm3, xmm3
		mulss xmm7, xmm7
		movss xmm4, SIMD_SP_sin_c0
		movss xmm0, SIMD_SP_sin_c0
		mulss xmm4, xmm3
		mulss xmm0, xmm7
		addss xmm4, SIMD_SP_sin_c1
		addss xmm0, SIMD_SP_sin_c1
		mulss xmm4, xmm3
		mulss xmm0, xmm7
		addss xmm4, SIMD_SP_sin_c2
		addss xmm0, SIMD_SP_sin_c2
		mulss xmm4, xmm3
		mulss xmm0, xmm7
		addss xmm4, SIMD_SP_sin_c3
		addss xmm0, SIMD_SP_sin_c3
		mulss xmm4, xmm3
		mulss xmm0, xmm7
		addss xmm4, SIMD_SP_sin_c4
		addss xmm0, SIMD_SP_sin_c4
		mulss xmm4, xmm3
		mulss xmm0, xmm7
		addss xmm4, SIMD_SP_one
		addss xmm0, SIMD_SP_one
		mulss xmm5, xmm4
		mulss xmm6, xmm0
		mulss xmm5, xmm2 // xmm5 = scale0
		mulss xmm6, xmm2 // xmm6 = scale1
		xorps xmm6, xmm1
		shufps xmm5, xmm5, R_SHUFFLE_PS( 0, 0, 0, 0 )
		mulps xmm5, jointQuat0
		shufps xmm6, xmm6, R_SHUFFLE_PS( 0, 0, 0, 0 )
		mulps xmm6, blendQuat0
		addps xmm5, xmm6
		movaps [esi+ecx+JOINTQUAT_Q_OFFSET], xmm5
		add eax, 1*4
		jl loopJoint1
	done1:
	}
}
*/

void slerpJointQuatsEx (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *q1, const ATOM_JointTransformInfo *q2, float t, unsigned numJoints, const unsigned *indices)
{
	ATOM_STACK_TRACE(slerpJointQuatsEx);
	for (unsigned n = 0; n < numJoints; ++n)
	{
		unsigned i = indices[n];
		//t = 0.f;
#if 0
		dst[i] = q1[i];
#else
		if (t == 0.f)
		{
			dst[i] = q1[i];
		}
		else if (t == 1.f)
		{
			dst[i] = q2[i];
		}
		else
		{
			dst[i].rotate = ATOM_Quaternion::fastSlerp (q1[i].rotate, q2[i].rotate, t);
			dst[i].scale = q1[i].scale + (q2[i].scale - q1[i].scale) * t;
			dst[i].translate = q1[i].translate + (q2[i].translate - q1[i].translate) * t;
		}
		//ATOM_LOGGER::log ("%f, %f\n", q1[4].translate.getVector3().getLength(), q2[4].translate.getVector3().getLength());
#endif
	}
}

void slerpJointQuats (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *q1, const ATOM_JointTransformInfo *q2, float t, unsigned numJoints)
{
	ATOM_STACK_TRACE(slerpJointQuats);
	for (unsigned i = 0; i < numJoints; ++i)
	{
#if 0
		dst[i] = q1[i];
#else
		dst[i].rotate = ATOM_Quaternion::slerp (q1[i].rotate, q2[i].rotate, t);
		dst[i].scale = q1[i].scale + (q2[i].scale - q1[i].scale) * t;
		dst[i].translate = q1[i].translate + (q2[i].translate - q1[i].translate) * t;
#endif
	}
}

void convertJointQuatToJointMat (ATOM_Matrix3x4f *mats, const ATOM_JointTransformInfo *quats, unsigned numJoints)
{
#if 1||defined(_M_IA64)||defined(_M_AMD64)
	for (int i = 0; i < numJoints; ++i)
	{
		const ATOM_JointTransformInfo &info = quats[i];
		ATOM_Matrix4x4f m = ATOM_Matrix4x4f::getIdentityMatrix();
		info.rotate.toMatrix (m);
		m.setRow3 (3, info.translate.x, info.translate.y, info.translate.z);
		m >>= ATOM_Matrix4x4f::getScaleMatrix (info.scale.getVector3());
		mats[i] = m;
	}
#else
	ATOM_STACK_TRACE(convertJointQuatToJointMat);
	assert_16_byte_aligned(mats);
	assert_16_byte_aligned(quats);

	__asm
	{
		mov eax, numJoints
		imul eax, JOINTQUAT_SIZE
		mov esi, quats
		mov edi, mats

		add esi, eax
		neg eax
		jz done

loopQuat:
		align 16
		movaps xmm0, [esi+eax+JOINTQUAT_Q_OFFSET]	// xmm0 = qx qy qz qw
		movaps xmm6, [esi+eax+JOINTQUAT_T_OFFSET]   // xmm6 = tx ty tz tw
		xorps xmm0, SIMD_SP_quatsign

		add edi, JOINTMAT_SIZE

		movaps xmm1, xmm0	// xmm1 = qx qy qz qw
		addps xmm1, xmm1	// xmm1 = qx2 qy2 qz2 qw2

		pshufd xmm2, xmm0, R_SHUFFLE_D(1, 0, 0, 1)	// xmm2 = qy qx qx qy
		pshufd xmm3, xmm1, R_SHUFFLE_D(1, 1, 2, 2)  // xmm3 = qy2 qy2 qz2 qz2
		mulps xmm2, xmm3	// xmm2 = yy2 xy2 xz2 yz2

		pshufd xmm4, xmm0, R_SHUFFLE_D(2, 3, 3, 3)	// xmm4 = qz qw qw qw
		pshufd xmm5, xmm1, R_SHUFFLE_D(2, 2, 1, 0)	// xmm5 = qz2 qz2 qy2 qx2
		mulps xmm4, xmm5	// xmm4 = zz2 wz2 wy2 wx2

		mulss xmm0, xmm1	// xmm0 = xx2 y2 z2 w2

		movss xmm7, SIMD_SP_one	// xmm7 = 1 0 0 0
		subss xmm7, xmm0	// xmm7 = -xx2+1 0 0 0
		subss xmm7, xmm2	// xmm7 = -xx2-yy2+1 0 0 0
		shufps xmm7, xmm6, R_SHUFFLE_PS(0, 1, 2, 3)	// xmm7 = -xx2-yy2+1 0 tz tw

		xorps xmm2, SIMD_SP_quat2mat_x0	// xmm2 = yy2 -xy2 -xz2 -yz2
		xorps xmm4, SIMD_SP_quat2mat_x1 // xmm4 = -zz2 wz2 -wy2 -wx2
		addss xmm4, SIMD_SP_one	// xmm4 = -zz2+1 wz2 -wy2 -wx2
		movaps xmm3, xmm4		// xmm3 = -zz2+1 wz2 -wy2 -wx2
		subps xmm3, xmm2		// xmm3 = -yy2-zz2+1 xy2+wz2 xz2-wy2 yz2-wx2
		mulps xmm3, [esi+eax+JOINTQUAT_S_OFFSET]
		movaps [edi-JOINTMAT_SIZE+0*16+0*4], xmm3
		movss [edi-JOINTMAT_SIZE+0*16+3*4], xmm6

		movss xmm2, xmm0	// xmm2 = xx2 -xy2 -xz2 -yz2
		xorps xmm4, SIMD_SP_quat2mat_x2 // xmm4 = -zz2+1 -wz2 wy2 wx2
		subps xmm4, xmm2	// xmm4 = -xx2-zz2+1 xy2-wz2 xz2+wy2 yz2+wx2
		shufps xmm6, xmm6, R_SHUFFLE_PS(1, 2, 3, 0)	// xmm6 = ty tz tw tx
		shufps xmm4, xmm4, R_SHUFFLE_PS(1, 0, 3, 2) // xmm4 = xy2-wz2 -xx2-zz2+1 yz2+wx2 xz2+wy2
		mulps xmm4, [esi+eax+JOINTQUAT_S_OFFSET]
		movaps [edi-JOINTMAT_SIZE+1*16+0*4], xmm4
		movss [edi-JOINTMAT_SIZE+1*16+3*4], xmm6

		movhlps xmm3, xmm4	// xmm3 = yz2+wx2 xz2+wy2 xz2-wy2 yz2-wx2
		shufps xmm3, xmm7, R_SHUFFLE_PS(1, 3, 0, 2)	// xmm3 = xz2+wy2 yz2-wx2 -xx2-yy2+1 tz
		mulps xmm3, [esi+eax+JOINTQUAT_S_OFFSET]
		movaps [edi-JOINTMAT_SIZE+2*16+0*4], xmm3

		add eax, JOINTQUAT_SIZE

		jl loopQuat
done:
	}
#endif
}

void transformMatrices (ATOM_Matrix3x4f *m1, const ATOM_Matrix3x4f *m2)
{
}

void transformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint)
{
	ATOM_STACK_TRACE(transformSkeleton);
#if 1||defined(_M_IA64)||defined(_M_AMD64)
	for (int i = firstJoint; i <= lastJoint; ++i)
	{
		jointMats[i] <<= jointMats[parents[i]];
	}
#else

	assert_16_byte_aligned( jointMats );

	__asm
	{
		mov ecx, firstJoint
		mov eax, lastJoint
		sub eax, ecx
		jl done
		shl ecx, 2 // ecx = firstJoint * 4
		mov edi, parents
		add edi, ecx // edx = &parents[firstJoint]
		lea ecx, [ecx+ecx*2]
		shl ecx, 2 // ecx = firstJoint * JOINTMAT_SIZE
		mov esi, jointMats // esi = jointMats
		shl eax, 2 // eax = ( lastJoint - firstJoint ) * 4
		add edi, eax
		neg eax

		loopJoint:

		mov edx, [edi+eax]
		movaps xmm0, [esi+ecx+ 0] // xmm0 = m0, m1, m2, t0
		lea edx, [edx+edx*2]
		movaps xmm1, [esi+ecx+16] // xmm1 = m2, m3, m4, t1
		shl edx, 4 // edx = parents[i] * JOINTMAT_SIZE
		movaps xmm2, [esi+ecx+32] // xmm2 = m5, m6, m7, t2
		movaps xmm7, [esi+edx+ 0]
		pshufd xmm4, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm4, xmm0
		pshufd xmm5, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm5, xmm1
		addps xmm4, xmm5
		add ecx, JOINTMAT_SIZE
		add eax, 4
		pshufd xmm6, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm6, xmm2
		addps xmm4, xmm6
		andps xmm7, SIMD_SP_clearFirstThree
		addps xmm4, xmm7
		movaps [esi+ecx-JOINTMAT_SIZE+ 0], xmm4
		movaps xmm3, [esi+edx+16]
		pshufd xmm5, xmm3, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm5, xmm0
		pshufd xmm6, xmm3, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm6, xmm1
		addps xmm5, xmm6
		pshufd xmm4, xmm3, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm4, xmm2
		addps xmm5, xmm4
		andps xmm3, SIMD_SP_clearFirstThree
		addps xmm5, xmm3
		movaps [esi+ecx-JOINTMAT_SIZE+16], xmm5
		movaps xmm7, [esi+edx+32]
		pshufd xmm6, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm6, xmm0
		pshufd xmm4, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm4, xmm1
		addps xmm6, xmm4
		pshufd xmm3, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm3, xmm2
		addps xmm6, xmm3
		andps xmm7, SIMD_SP_clearFirstThree
		addps xmm6, xmm7
		movaps [esi+ecx-JOINTMAT_SIZE+32], xmm6
		jle loopJoint

		done:
	}
#endif
}

void untransformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	for (int i = lastJoint; i >= firstJoint; i--)
	{
		ATOM_Matrix4x4f mParentInverse;
		jointMats[parents[i]].toMatrix44 (mParentInverse);
		mParentInverse.invertAffine ();
		ATOM_Matrix4x4f mSelf;
		jointMats[i].toMatrix44 (mSelf);
		jointMats[i] = mParentInverse >> mSelf;
	}
#else
	ATOM_STACK_TRACE(untransformSkeleton);
	assert_16_byte_aligned( jointMats );
	__asm
	{
		mov edx, firstJoint
		mov eax, lastJoint
		mov ecx, eax
		sub eax, edx
		jl done
		mov esi, jointMats // esi = jointMats
		lea ecx, [ecx+ecx*2]
		shl ecx, 4 // ecx = lastJoint * JOINTMAT_SIZE
		shl edx, 2
		mov edi, parents
		add edi, edx // edi = &parents[firstJoint]
		shl eax, 2 // eax = ( lastJoint - firstJoint ) * 4

		loopJoint:
		mov edx, [edi+eax]
		movaps xmm0, [esi+ecx+ 0] // xmm0 = m0, m1, m2, t0
		lea edx, [edx+edx*2]
		movaps xmm1, [esi+ecx+16] // xmm1 = m2, m3, m4, t1
		shl edx, 4 // edx = parents[i] * JOINTMAT_SIZE
		movaps xmm2, [esi+ecx+32] // xmm2 = m5, m6, m7, t2
		movss xmm7, [esi+edx+12]
		pshufd xmm7, xmm7, R_SHUFFLE_D( 1, 2, 3, 0 )
		subps xmm0, xmm7
		movss xmm6, [esi+edx+28]
		pshufd xmm6, xmm6, R_SHUFFLE_D( 1, 2, 3, 0 )
		subps xmm1, xmm6
		movss xmm5, [esi+edx+44]
		pshufd xmm5, xmm5, R_SHUFFLE_D( 1, 2, 3, 0 )
		subps xmm2, xmm5
		sub ecx, JOINTMAT_SIZE
		sub eax, 4
		movaps xmm7, [esi+edx+ 0]
		pshufd xmm3, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm3, xmm0
		pshufd xmm4, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm4, xmm0
		pshufd xmm5, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm5, xmm0
		movaps xmm7, [esi+edx+16]
		pshufd xmm0, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm0, xmm1
		addps xmm3, xmm0
		pshufd xmm6, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm6, xmm1
		addps xmm4, xmm6
		pshufd xmm0, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm0, xmm1
		addps xmm5, xmm0
		movaps xmm7, [esi+edx+32]
		pshufd xmm6, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm6, xmm2
		addps xmm3, xmm6
		movaps [esi+ecx+JOINTMAT_SIZE+ 0], xmm3
		pshufd xmm1, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm1, xmm2
		addps xmm4, xmm1
		movaps [esi+ecx+JOINTMAT_SIZE+16], xmm4
		pshufd xmm6, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm6, xmm2
		addps xmm5, xmm6
		movaps [esi+ecx+JOINTMAT_SIZE+32], xmm5
		jge loopJoint

		done:
	}
#endif
}

void transformJoint (ATOM_Matrix3x4f *result, const ATOM_Matrix3x4f *joints1, const ATOM_Matrix3x4f *joints2, int numJoints)
{
#if 1||defined(_M_IA64)||defined(_M_AMD64)
	for (int i = 0; i < numJoints; ++i)
	{
		result[i] = joints2[i] << joints1[i];
	}
#else
	ATOM_STACK_TRACE(transformJoint);
	assert_16_byte_aligned( result );
	assert_16_byte_aligned( joints1 );
	assert_16_byte_aligned( joints2 );
	__asm
	{
		mov eax, numJoints
		test eax, eax
		jz done
		mov ecx, joints1
		mov edx, joints2
		mov edi, result
		imul eax, JOINTMAT_SIZE
		add ecx, eax
		add edx, eax
		add edi, eax
		neg eax

		loopJoint:
		movaps xmm0, [edx+eax+ 0]
		movaps xmm1, [edx+eax+16]
		movaps xmm2, [edx+eax+32]
		movaps xmm7, [ecx+eax+ 0]
		pshufd xmm3, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm3, xmm0
		pshufd xmm4, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm4, xmm1
		addps xmm3, xmm4
		add eax, JOINTMAT_SIZE
		pshufd xmm5, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm5, xmm2
		addps xmm3, xmm5
		andps xmm7, SIMD_SP_clearFirstThree
		addps xmm3, xmm7
		movaps [edi+eax-JOINTMAT_SIZE+0], xmm3
		movaps xmm7, [ecx+eax-JOINTMAT_SIZE+16]
		pshufd xmm3, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm3, xmm0
		pshufd xmm4, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm4, xmm1
		addps xmm3, xmm4
		pshufd xmm5, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm5, xmm2
		addps xmm3, xmm5
		andps xmm7, SIMD_SP_clearFirstThree
		addps xmm3, xmm7
		movaps [edi+eax-JOINTMAT_SIZE+16], xmm3
		movaps xmm7, [ecx+eax-JOINTMAT_SIZE+32]
		pshufd xmm3, xmm7, R_SHUFFLE_D( 0, 0, 0, 0 )
		mulps xmm3, xmm0
		pshufd xmm4, xmm7, R_SHUFFLE_D( 1, 1, 1, 1 )
		mulps xmm4, xmm1
		addps xmm3, xmm4
		pshufd xmm5, xmm7, R_SHUFFLE_D( 2, 2, 2, 2 )
		mulps xmm5, xmm2
		addps xmm3, xmm5
		andps xmm7, SIMD_SP_clearFirstThree
		addps xmm3, xmm7
		movaps [edi+eax-JOINTMAT_SIZE+32], xmm3
		jl loopJoint

		done:
	}
#endif
}

void transformVerts (ATOM_Vector3f *verts,
					 const int numVerts,
					 const ATOM_Matrix3x4f *jointMats,
					 const ATOM_Mesh::SkeletonVertex *base,
					 const ATOM_Mesh::VertexWeight *weights,
					 const unsigned *remaps)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	// TODO
#else
	ATOM_STACK_TRACE(transformVerts);
	assert_16_byte_aligned (jointMats);
	assert_16_byte_aligned (base);

	__asm
	{
		mov eax, numVerts
		test eax, eax
		jz done
		imul eax, VERTEXSIZE

		mov ecx, verts
		mov edx, weights
		mov esi, base
		mov edi, jointMats

		add ecx, eax
		neg eax

loopVert:
		align 16
		push ecx
		movss xmm0, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm0 = weight, 0, 0, 0

		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		mov ecx, remaps
		mov ebx, dword ptr [ecx+ebx*4]	// ebx = remaps[vertexweight.jointIndex]

		shufps xmm0, xmm0, R_SHUFFLE_PS(0, 0, 0, 0)	// xmm0 = weight, weight, weight, weight

		imul ebx, JOINTMAT_SIZE	// ebx = byte offset of jointMat sizeof(ATOM_Matrix3x4f) * remaps[vertexweight.jointIndex]
		pop ecx

		add edx, VERTEXWEIGHT_SIZE
		movaps xmm1, xmm0	// xmm1 = weight, weight, weight, weight
		add esi, 3*BASEVERTEXSIZE
		movaps xmm2, xmm0	// xmm2 = weight, weight, weight, weight

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		mulps xmm0, [edi+ebx+0]	// xmm0 = weighted (m0, m1, m2, t0)
		mulps xmm1, [edi+ebx+16]	// xmm1 = weighted (m3, m4, m5, t1)
		mulps xmm2, [edi+ebx+32]	// xmm2 = weighted (m6, m7, m8, t2)

		je doneWeight

loopWeight:
		align 16
		movss xmm3, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm3 = weight, 0, 0, 0
		push ecx
		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		mov ecx, remaps
		mov ebx, dword ptr [ecx+ebx*4]	// ebx = remaps[vertexweight.jointIndex]
		imul ebx, JOINTMAT_SIZE		// ebx = byte offset of jointMats

		shufps xmm3, xmm3, R_SHUFFLE_PS(0, 0, 0, 0) // xmm3 = weight, weight, weight, weight

		pop ecx
		movaps xmm4, xmm3	// xmm4 = weight, weight, weight, weight
		movaps xmm5, xmm3	// xmm5 = weight, weight, weight, weight

		add edx, VERTEXWEIGHT_SIZE

		mulps xmm3, [edi+ebx+0]
		mulps xmm4, [edi+ebx+16]
		mulps xmm5, [edi+ebx+32]

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		addps xmm0, xmm3
		addps xmm1, xmm4
		addps xmm2, xmm5

		jne loopWeight

doneWeight:
		add eax, VERTEXSIZE

		// transform vertices

		movaps xmm3, [esi-3*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ecx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6

		movss [ecx+eax-VERTEXSIZE+8], xmm7

		jl loopVert
done:
	}
#endif
}

void transformVerts (ATOM_Vector3f *verts,
					 const int numVerts,
					 const ATOM_Matrix3x4f *jointMats,
					 const ATOM_Mesh::SkeletonVertex *base,
					 const ATOM_Mesh::VertexWeight *weights)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	// TODO
#else
	ATOM_STACK_TRACE(transformVerts);
	assert_16_byte_aligned (jointMats);
	assert_16_byte_aligned (base);

	__asm
	{
		mov eax, numVerts
		test eax, eax
		jz done
		imul eax, VERTEXSIZE

		mov ecx, verts
		mov edx, weights
		mov esi, base
		mov edi, jointMats

		add ecx, eax
		neg eax

loopVert:
		align 16
		movss xmm0, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm0 = weight, 0, 0, 0

		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		shufps xmm0, xmm0, R_SHUFFLE_PS(0, 0, 0, 0)	// xmm0 = weight, weight, weight, weight
		imul ebx, JOINTMAT_SIZE // ebx = byte offset of jointScales sizeof(ATOM_Vector4f) * remaps[vertexweight.jointIndex]

		add edx, VERTEXWEIGHT_SIZE
		movaps xmm1, xmm0	// xmm1 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		add esi, 3*BASEVERTEXSIZE
		movaps xmm2, xmm0	// xmm2 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		mulps xmm0, [edi+ebx+0]	// xmm0 = weighted (m0, m1, m2, t0)
		mulps xmm1, [edi+ebx+16]	// xmm1 = weighted (m3, m4, m5, t1)
		mulps xmm2, [edi+ebx+32]	// xmm2 = weighted (m6, m7, m8, t2)

		je doneWeight

loopWeight:
		align 16
		movss xmm3, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm3 = weight, 0, 0, 0

		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		imul ebx, JOINTMAT_SIZE		// ebx = byte offset of jointMats
		shufps xmm3, xmm3, R_SHUFFLE_PS(0, 0, 0, 0) // xmm3 = weight, weight, weight, weight

		movaps xmm4, xmm3	// xmm4 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		movaps xmm5, xmm3	// xmm5 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		add edx, VERTEXWEIGHT_SIZE

		mulps xmm3, [edi+ebx+0]
		mulps xmm4, [edi+ebx+16]
		mulps xmm5, [edi+ebx+32]

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		addps xmm0, xmm3
		addps xmm1, xmm4
		addps xmm2, xmm5

		jne loopWeight

doneWeight:
		add eax, VERTEXSIZE

		// transform vertices

		movaps xmm3, [esi-3*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ecx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6

		movss [ecx+eax-VERTEXSIZE+8], xmm7

		jl loopVert
done:
	}
#endif
}

void transformVertsAndNormals (ATOM_Vector3f *verts,
							   ATOM_Vector3f *normals,
							   const int numVerts,
							   const ATOM_Matrix3x4f *jointMats,
							   const ATOM_Mesh::SkeletonVertex *base,
							   const ATOM_Mesh::VertexWeight *weights,
							   const unsigned *remaps)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	// TODO
#else
	ATOM_STACK_TRACE(transformVertsAndNormals);
	assert_16_byte_aligned (jointMats);
	assert_16_byte_aligned (base);

	void *n;

	__asm
	{
		mov eax, numVerts
		test eax, eax
		jz done
		imul eax, VERTEXSIZE

		mov ecx, verts
		mov edx, weights
		mov esi, base
		mov edi, jointMats

		add ecx, eax

		mov ebx, normals
		add ebx, eax
		mov n, ebx

		neg eax

loopVert:
		align 16
		push ecx
		movss xmm0, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm0 = weight, 0, 0, 0

		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		mov ecx, remaps
		mov ebx, dword ptr [ecx+ebx*4]	// ebx = remaps[vertexweight.jointIndex]

		shufps xmm0, xmm0, R_SHUFFLE_PS(0, 0, 0, 0)	// xmm0 = weight, weight, weight, weight

		imul ebx, JOINTMAT_SIZE	// ebx = byte offset of jointMat sizeof(ATOM_Matrix3x4f) * remaps[vertexweight.jointIndex]
		pop ecx

		add edx, VERTEXWEIGHT_SIZE
		movaps xmm1, xmm0	// xmm1 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		add esi, 3*BASEVERTEXSIZE
		movaps xmm2, xmm0	// xmm2 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		mulps xmm0, [edi+ebx+0]	// xmm0 = weighted (m0, m1, m2, t0)
		mulps xmm1, [edi+ebx+16]	// xmm1 = weighted (m3, m4, m5, t1)
		mulps xmm2, [edi+ebx+32]	// xmm2 = weighted (m6, m7, m8, t2)

		je doneWeight

loopWeight:
		align 16
		movss xmm3, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm3 = weight, 0, 0, 0
		push ecx
		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		mov ecx, remaps
		mov ebx, dword ptr [ecx+ebx*4]	// ebx = remaps[vertexweight.jointIndex]
		imul ebx, JOINTMAT_SIZE		// ebx = byte offset of jointMats

		shufps xmm3, xmm3, R_SHUFFLE_PS(0, 0, 0, 0) // xmm3 = weight, weight, weight, weight

		pop ecx
		movaps xmm4, xmm3	// xmm4 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		movaps xmm5, xmm3	// xmm5 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		add edx, VERTEXWEIGHT_SIZE

		mulps xmm3, [edi+ebx+0]
		mulps xmm4, [edi+ebx+16]
		mulps xmm5, [edi+ebx+32]

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		addps xmm0, xmm3
		addps xmm1, xmm4
		addps xmm2, xmm5

		jne loopWeight

doneWeight:
		add eax, VERTEXSIZE

		// transform vertices

		movaps xmm3, [esi-3*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ecx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6

		movss [ecx+eax-VERTEXSIZE+8], xmm7

		// transform normals
		mov ebx, n

		movaps xmm3, [esi-2*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ebx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6
		movss [ebx+eax-VERTEXSIZE+8], xmm7

		jl loopVert
done:
	}
#endif
}

void transformVertsAndNormals (ATOM_Vector3f *verts,
							   ATOM_Vector3f *normals,
							   const int numVerts,
							   const ATOM_Matrix3x4f *jointMats,
							   const ATOM_Mesh::SkeletonVertex *base,
							   const ATOM_Mesh::VertexWeight *weights)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	// TODO
#else
	ATOM_STACK_TRACE(transformVertsAndNormals);
	assert_16_byte_aligned (jointMats);
	assert_16_byte_aligned (base);

	void *n;

	__asm
	{
		mov eax, numVerts
		test eax, eax
		jz done
		imul eax, VERTEXSIZE

		mov ecx, verts
		mov edx, weights
		mov esi, base
		mov edi, jointMats

		add ecx, eax

		mov ebx, normals
		add ebx, eax
		mov n, ebx

		neg eax

loopVert:
		align 16
		movss xmm0, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm0 = weight, 0, 0, 0

		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex

		shufps xmm0, xmm0, R_SHUFFLE_PS(0, 0, 0, 0)	// xmm0 = weight, weight, weight, weight

		imul ebx, JOINTMAT_SIZE	// ebx = byte offset of jointMat sizeof(ATOM_Matrix3x4f) * remaps[vertexweight.jointIndex]

		add edx, VERTEXWEIGHT_SIZE
		movaps xmm1, xmm0	// xmm1 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		add esi, 3*BASEVERTEXSIZE
		movaps xmm2, xmm0	// xmm2 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		mulps xmm0, [edi+ebx+0]	// xmm0 = weighted (m0, m1, m2, t0)
		mulps xmm1, [edi+ebx+16]	// xmm1 = weighted (m3, m4, m5, t1)
		mulps xmm2, [edi+ebx+32]	// xmm2 = weighted (m6, m7, m8, t2)

		je doneWeight

loopWeight:
		align 16

		movss xmm3, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm3 = weight, 0, 0, 0
		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		imul ebx, JOINTMAT_SIZE		// ebx = byte offset of jointMats
		shufps xmm3, xmm3, R_SHUFFLE_PS(0, 0, 0, 0) // xmm3 = weight, weight, weight, weight

		movaps xmm4, xmm3	// xmm4 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		movaps xmm5, xmm3	// xmm5 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		add edx, VERTEXWEIGHT_SIZE

		mulps xmm3, [edi+ebx+0]
		mulps xmm4, [edi+ebx+16]
		mulps xmm5, [edi+ebx+32]

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		addps xmm0, xmm3
		addps xmm1, xmm4
		addps xmm2, xmm5

		jne loopWeight

doneWeight:
		add eax, VERTEXSIZE

		// transform vertices

		movaps xmm3, [esi-3*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ecx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6

		movss [ecx+eax-VERTEXSIZE+8], xmm7

		// transform normals
		mov ebx, n

		movaps xmm3, [esi-2*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ebx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6
		movss [ebx+eax-VERTEXSIZE+8], xmm7

		jl loopVert
done:
	}
#endif
}

void transformVertsAndNormalsAndTangents (ATOM_Vector3f *verts,
										  ATOM_Vector3f *normals,
										  ATOM_Vector3f *tangents,
										  const int numVerts,
										  const ATOM_Matrix3x4f *jointMats,
										  const ATOM_Mesh::SkeletonVertex *base,
										  const ATOM_Mesh::VertexWeight *weights,
										  const unsigned *remaps)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	// TODO
#else
	ATOM_STACK_TRACE(transformVertsAndNormalsAndTangents);
	assert_16_byte_aligned (jointMats);
	assert_16_byte_aligned (base);

	void *n;
	void *t;

	__asm
	{
		mov eax, numVerts
		test eax, eax
		jz done
		imul eax, VERTEXSIZE

		mov ecx, verts
		mov edx, weights
		mov esi, base
		mov edi, jointMats

		add ecx, eax

		mov ebx, normals
		add ebx, eax
		mov n, ebx

		mov ebx, tangents
		add ebx, eax
		mov t, ebx

		neg eax

loopVert:
		align 16
		push ecx
		movss xmm0, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm0 = weight, 0, 0, 0

		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		mov ecx, remaps
		mov ebx, dword ptr [ecx+ebx*4]	// ebx = remaps[vertexweight.jointIndex]

		shufps xmm0, xmm0, R_SHUFFLE_PS(0, 0, 0, 0)	// xmm0 = weight, weight, weight, weight

		imul ebx, JOINTMAT_SIZE	// ebx = byte offset of jointMat sizeof(ATOM_Matrix3x4f) * remaps[vertexweight.jointIndex]
		pop ecx

		add edx, VERTEXWEIGHT_SIZE
		movaps xmm1, xmm0	// xmm1 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		add esi, 3*BASEVERTEXSIZE
		movaps xmm2, xmm0	// xmm2 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		mulps xmm0, [edi+ebx+0]	// xmm0 = weighted (m0, m1, m2, t0)
		mulps xmm1, [edi+ebx+16]	// xmm1 = weighted (m3, m4, m5, t1)
		mulps xmm2, [edi+ebx+32]	// xmm2 = weighted (m6, m7, m8, t2)

		je doneWeight

loopWeight:
		align 16
		movss xmm3, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm3 = weight, 0, 0, 0
		push ecx
		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		mov ecx, remaps
		mov ebx, dword ptr [ecx+ebx*4]	// ebx = remaps[vertexweight.jointIndex]
		imul ebx, JOINTMAT_SIZE		// ebx = byte offset of jointMats

		shufps xmm3, xmm3, R_SHUFFLE_PS(0, 0, 0, 0) // xmm3 = weight, weight, weight, weight

		pop ecx
		movaps xmm4, xmm3	// xmm4 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		movaps xmm5, xmm3	// xmm5 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		add edx, VERTEXWEIGHT_SIZE

		mulps xmm3, [edi+ebx+0]
		mulps xmm4, [edi+ebx+16]
		mulps xmm5, [edi+ebx+32]

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		addps xmm0, xmm3
		addps xmm1, xmm4
		addps xmm2, xmm5

		jne loopWeight

doneWeight:
		add eax, VERTEXSIZE

		// transform vertices

		movaps xmm3, [esi-3*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ecx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6

		movss [ecx+eax-VERTEXSIZE+8], xmm7

		// transform normals
		mov ebx, n

		movaps xmm3, [esi-2*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ebx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6
		movss [ebx+eax-VERTEXSIZE+8], xmm7

		// transform tangents
		mov ebx, t

		movaps xmm3, [esi-BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ebx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6
		movss [ebx+eax-VERTEXSIZE+8], xmm7

		jl loopVert
done:
	}
#endif
}

void transformVertsAndNormalsAndTangents (ATOM_Vector3f *verts,
										  ATOM_Vector3f *normals,
										  ATOM_Vector3f *tangents,
										  const int numVerts,
										  const ATOM_Matrix3x4f *jointMats,
										  const ATOM_Mesh::SkeletonVertex *base,
										  const ATOM_Mesh::VertexWeight *weights)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	// TODO
#else
	ATOM_STACK_TRACE(transformVertsAndNormalsAndTangents);
	assert_16_byte_aligned (jointMats);
	assert_16_byte_aligned (base);

	void *n;
	void *t;

	__asm
	{
		mov eax, numVerts
		test eax, eax
		jz done
		imul eax, VERTEXSIZE

		mov ecx, verts
		mov edx, weights
		mov esi, base
		mov edi, jointMats

		add ecx, eax

		mov ebx, normals
		add ebx, eax
		mov n, ebx

		mov ebx, tangents
		add ebx, eax
		mov t, ebx

		neg eax

loopVert:
		align 16
		movss xmm0, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm0 = weight, 0, 0, 0

		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex

		shufps xmm0, xmm0, R_SHUFFLE_PS(0, 0, 0, 0)	// xmm0 = weight, weight, weight, weight

		imul ebx, JOINTMAT_SIZE	// ebx = byte offset of jointMat sizeof(ATOM_Matrix3x4f) * remaps[vertexweight.jointIndex]

		add edx, VERTEXWEIGHT_SIZE
		movaps xmm1, xmm0	// xmm1 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		add esi, 3*BASEVERTEXSIZE
		movaps xmm2, xmm0	// xmm2 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		mulps xmm0, [edi+ebx+0]	// xmm0 = weighted (m0, m1, m2, t0)
		mulps xmm1, [edi+ebx+16]	// xmm1 = weighted (m3, m4, m5, t1)
		mulps xmm2, [edi+ebx+32]	// xmm2 = weighted (m6, m7, m8, t2)

		je doneWeight

loopWeight:
		align 16

		movss xmm3, [edx+VERTEXWEIGHT_W_OFFSET]	// xmm3 = weight, 0, 0, 0
		mov ebx, dword ptr [edx+VERTEXWEIGHT_I_OFFSET]	// ebx = vertexweight.jointIndex
		imul ebx, JOINTMAT_SIZE		// ebx = byte offset of jointMats
		shufps xmm3, xmm3, R_SHUFFLE_PS(0, 0, 0, 0) // xmm3 = weight, weight, weight, weight

		movaps xmm4, xmm3	// xmm4 = weight * scalex, weight * scaley, weight * scalez, weight * scalew
		movaps xmm5, xmm3	// xmm5 = weight * scalex, weight * scaley, weight * scalez, weight * scalew

		add edx, VERTEXWEIGHT_SIZE

		mulps xmm3, [edi+ebx+0]
		mulps xmm4, [edi+ebx+16]
		mulps xmm5, [edi+ebx+32]

		cmp dword ptr [edx-VERTEXWEIGHT_SIZE+VERTEXWEIGHT_O_OFFSET], VERTEXWEIGHT_SIZE

		addps xmm0, xmm3
		addps xmm1, xmm4
		addps xmm2, xmm5

		jne loopWeight

doneWeight:
		add eax, VERTEXSIZE

		// transform vertices

		movaps xmm3, [esi-3*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ecx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6

		movss [ecx+eax-VERTEXSIZE+8], xmm7

		// transform normals
		mov ebx, n

		movaps xmm3, [esi-2*BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ebx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6
		movss [ebx+eax-VERTEXSIZE+8], xmm7

		// transform tangents
		mov ebx, t

		movaps xmm3, [esi-BASEVERTEXSIZE]
		movaps xmm4, xmm3
		movaps xmm5, xmm3

		mulps xmm3, xmm0
		mulps xmm4, xmm1
		mulps xmm5, xmm2

		movaps xmm6, xmm3
		unpcklps xmm6, xmm4
		unpckhps xmm3, xmm4
		addps xmm6, xmm3

		movaps xmm7, xmm5
		movlhps xmm5, xmm6
		movhlps xmm6, xmm7
		addps xmm6, xmm5

		movhps [ebx+eax-VERTEXSIZE+0], xmm6
		pshufd xmm7, xmm6, R_SHUFFLE_D(1, 0, 2, 3)
		addss xmm7, xmm6
		movss [ebx+eax-VERTEXSIZE+8], xmm7

		jl loopVert
done:
	}
#endif
}

void transformSkeleton (ATOM_Matrix3x4f *mats, const int *parents, unsigned size)
{
	ATOM_STACK_TRACE(transformSkeleton);
	int parent = 0;
	while (parent < size)
	{
		ATOM_ASSERT(parents[parent] < 0);
		int first = ++parent;
		for (; parent < size && parents[parent] >= 0 ; parent++)
			;
		int last = parent - 1;
		if (first <= last)
		{
			transformSkeleton (mats, parents, first, last);
		}
	}
}

#else

#error Non-SSE version not implemented

void ATOM_ENGINE_API slerpJointFast (ATOM_JointTransformInfo *joints, const ATOM_JointTransformInfo *blendJoints, const float lerp, int numJoints, const unsigned *index)
{
}

void ATOM_ENGINE_API slerpJointFastEx (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *joints, const ATOM_JointTransformInfo *blendJoints, const float t, int numJoints, const unsigned *indices)
{
}

void ATOM_ENGINE_API slerpJointQuatsEx (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *q1, const ATOM_JointTransformInfo *q2, float t, unsigned numJoints, const unsigned *indices)
{
}

void ATOM_ENGINE_API slerpJointQuats (ATOM_JointTransformInfo *dst, const ATOM_JointTransformInfo *q1, const ATOM_JointTransformInfo *q2, float t, unsigned numJoints)
{
}

void ATOM_ENGINE_API convertJointQuatToJointMat (ATOM_Matrix3x4f *mats, const ATOM_JointTransformInfo *quats, unsigned numJoints)
{
}

void ATOM_ENGINE_API transformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint)
{
}

void ATOM_ENGINE_API untransformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint)
{
}

void ATOM_ENGINE_API transformJoint (ATOM_Matrix3x4f *result, const ATOM_Matrix3x4f *joints1, const ATOM_Matrix3x4f *joints2, int numJoints)
{
}

void ATOM_ENGINE_API transformSkeleton (ATOM_Matrix3x4f *mats, const int *parents, unsigned size)
{
}

void transformVerts (ATOM_Vector3f *verts,
					 const int numVerts,
					 const ATOM_Matrix3x4f *jointMats,
					 const ATOM_Mesh::SkeletonVertex *base,
					 const ATOM_Mesh::VertexWeight *weights,
					 const unsigned *remaps)
{
}

void transformVerts (ATOM_Vector3f *verts,
					 const int numVerts,
					 const ATOM_Matrix3x4f *jointMats,
					 const ATOM_Mesh::SkeletonVertex *base,
					 const ATOM_Mesh::VertexWeight *weights)
{
}

void transformVertsAndNormals (ATOM_Vector3f *verts,
							   ATOM_Vector3f *normals,
							   const int numVerts,
							   const ATOM_Matrix3x4f *jointMats,
							   const ATOM_Mesh::SkeletonVertex *base,
							   const ATOM_Mesh::VertexWeight *weights,
							   const unsigned *remaps)
{
}

void transformVertsAndNormals (ATOM_Vector3f *verts,
							   ATOM_Vector3f *normals,
							   const int numVerts,
							   const ATOM_Matrix3x4f *jointMats,
							   const ATOM_Mesh::SkeletonVertex *base,
							   const ATOM_Mesh::VertexWeight *weights)
{
}

void transformVertsAndNormalsAndTangents (ATOM_Vector3f *verts,
										  ATOM_Vector3f *normals,
										  ATOM_Vector3f *tangents,
										  const int numVerts,
										  const ATOM_Matrix3x4f *jointMats,
										  const ATOM_Mesh::SkeletonVertex *base,
										  const ATOM_Mesh::VertexWeight *weights,
										  const unsigned *remaps)
{
}

void transformVertsAndNormalsAndTangents (ATOM_Vector3f *verts,
										  ATOM_Vector3f *normals,
										  ATOM_Vector3f *tangents,
										  const int numVerts,
										  const ATOM_Matrix3x4f *jointMats,
										  const ATOM_Mesh::SkeletonVertex *base,
										  const ATOM_Mesh::VertexWeight *weights)
{
}


#endif
