#include <windows.h>
#include "skinanim_sse.h"

#define assert_16_byte_aligned(p) assert((((UINT_PTR)(p)) & 15) == 0)
#define ALIGN16(x) __declspec(align(16)) x
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



ALIGN4_INIT1(float SIMD_SP_one, 1.f);
ALIGN4_INIT1(float SIMD_SP_negone, -1.f);
ALIGN4_INIT4(unsigned long SIMD_SP_quatsign, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN, 0);
ALIGN4_INIT4(unsigned long SIMD_SP_quat2mat_x0, IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN);
ALIGN4_INIT4(unsigned long SIMD_SP_quat2mat_x1, IEEE_SP_SIGN, IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN);
ALIGN4_INIT4(unsigned long SIMD_SP_quat2mat_x2, IEEE_SP_ZERO, IEEE_SP_SIGN, IEEE_SP_SIGN, IEEE_SP_SIGN);
ALIGN4_INIT4(unsigned long SIMD_SP_clearFirstThree, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF );

void slerpJointQuatsEx (JointTransformInfo *dst, const JointTransformInfo *q1, const JointTransformInfo *q2, float t, unsigned numJoints, const unsigned *indices)
{
	for (unsigned n = 0; n < numJoints; ++n)
	{
		unsigned i = indices[n];
#if 0
		dst[i] = q1[i];
#else
		dst[i].rotate = ATOM_Quaternion::fastSlerp (q1[i].rotate, q2[i].rotate, t);
		dst[i].scale = q1[i].scale + (q2[i].scale - q1[i].scale) * t;
		dst[i].translate = q1[i].translate + (q2[i].translate - q1[i].translate) * t;
#endif
	}
}

void slerpJointQuats (JointTransformInfo *dst, const JointTransformInfo *q1, const JointTransformInfo *q2, float t, unsigned numJoints)
{
	for (unsigned i = 0; i < numJoints; ++i)
	{
#if 0
		dst[i] = q1[i];
#else
		dst[i].rotate = ATOM_Quaternion::fastSlerp (q1[i].rotate, q2[i].rotate, t);
		dst[i].scale = q1[i].scale + (q2[i].scale - q1[i].scale) * t;
		dst[i].translate = q1[i].translate + (q2[i].translate - q1[i].translate) * t;
#endif
	}
}

void convertJointQuatToJointMat (ATOM_Matrix3x4f *mats, const JointTransformInfo *quats, unsigned numJoints)
{
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
}

void transformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint)
{
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
}

void untransformSkeleton (ATOM_Matrix3x4f *jointMats, const int *parents, int firstJoint, int lastJoint)
{
#if 1
	for (int i = lastJoint; i >= firstJoint; i--)
	{
		ATOM_Matrix4x4f parentMatrixInverse;
		jointMats[parents[i]].toMatrix44 (parentMatrixInverse);
		parentMatrixInverse.invertAffine ();

		ATOM_Matrix4x4f selfMatrix;
		jointMats[i].toMatrix44 (selfMatrix);

		ATOM_Matrix4x4f localMatrix = parentMatrixInverse >> selfMatrix;
		jointMats[i] = localMatrix;
	}
	return;
#endif
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
}

void transformJoint (ATOM_Matrix3x4f *result, const ATOM_Matrix3x4f *joints1, const ATOM_Matrix3x4f *joints2, int numJoints)
{
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
}



