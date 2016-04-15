#ifndef __ATOM_MATH_PERLINNOISE_H
#define __ATOM_MATH_PERLINNOISE_H

#include <stdlib.h>
#include "globals.h"

class PerlinNoise
{
private:
	unsigned _M_prime[3];

public:
	PerlinNoise () 
	{
		_M_prime[0] = 15731;
		_M_prime[1] = 789221;
		_M_prime[2] = 1376312589;
	}

	void NewPrimeNumbers () 
	{
		_M_prime[0] = RandomPrime ();
		_M_prime[1] = RandomPrime ();
		_M_prime[2] = RandomPrime ();
	}

	bool IsPrime (unsigned n) const 
	{
		int sa = ATOM_ftol(ATOM_sqrt(float(n)));

		for (int j = 2; j < sa; ++j)
		{
			if (n % j == 0)
			{
				return false;
			}
		}

		return true;
	}

	unsigned RandomPrime () const 
	{
		unsigned prime = (unsigned)rand();
		while (!IsPrime (prime))
			++prime;
		return prime;
	}

	float Noise (int x) const 
	{
		x = (x << 13) ^ x;
		return 1.0f - ((x * (x * x * _M_prime[0] + _M_prime[1]) + _M_prime[2]) & 0x7fffffff) / 1073741824.f;
	}

	float Noise (int x, int y) const 
	{
		unsigned n = x + y * 57;
		n = (n << 13) ^ n;
		return 1.0f - ((n * ( n * n * _M_prime[0] + _M_prime[1]) + _M_prime[2]) & 0x7fffffff) / 1073741824.f;
	}

	float Noise (int x, int y, int z) const 
	{
		int l = x + y * 57;
		int m = y + z * 57;
		unsigned n = l + m * 57;
		n = (n << 13) ^ n;
		return 1.0f - ((n * (n * n * _M_prime[0] + _M_prime[1]) + _M_prime[2]) & 0x7fffffff) / 1073741824.f;
	}

	float Interpolate (float a, float b, float x) const 
	{
#if 1
		return a + (b - a) * x;
#else
		float t = x * ATOM_Pi;
		float f = (1.f - ATOM_cos(t)) * 0.5f;
		return a * (1.0f - f) + b * f;
#endif
	}

	float SmoothNoise (int x) const 
	{
		return Noise(x) / 2.0f + Noise(x - 1) / 4.0f + Noise(x + 1) / 4.0f;
	}

	float SmoothNoise (int x, int y) const 
	{
		float corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16.0f;
		float sides   = (Noise(x - 1, y    ) + Noise(x + 1, y    ) + Noise(x    , y - 1) + Noise(x    , y + 1)) / 8.0f;
		float center  = Noise(x, y) / 4.0f;
		return corners + sides + center;
	}

	float SmoothNoise (int x, int y, int z) const 
	{
		float corners, sides, center;

		// average of neighbours in z-1
		corners = (Noise(x - 1, y - 1, z - 1) + Noise(x + 1, y - 1, z - 1) + Noise(x - 1, y + 1, z - 1) + Noise(x + 1, y + 1, z - 1 ) ) / 16.0f;
		sides   = (Noise(x - 1, y    , z - 1) + Noise(x + 1, y    , z - 1) + Noise(x    , y - 1, z - 1) + Noise(x    , y + 1, z - 1 ) ) / 8.0f;
		center  = Noise(x, y, z - 1 ) / 4.0f;
		float zm1 = corners + sides + center;

		// average of neighbours in z
		corners = (Noise(x - 1, y - 1, z) + Noise(x + 1, y - 1, z) + Noise(x - 1, y + 1, z) + Noise(x + 1, y + 1, z)) / 16.0f;
		sides   = (Noise(x - 1, y    , z) + Noise(x + 1, y    , z) + Noise(x    , y - 1, z) + Noise(x    , y + 1, z)) / 8.0f;
		center  = Noise(x, y, z) / 4.0f;
		float zo = corners + sides + center;

		// average of neighbours in z+1
		corners = (Noise(x - 1, y - 1, z + 1 ) + Noise(x + 1, y - 1, z + 1) + Noise(x - 1, y + 1, z + 1) + Noise(x + 1, y + 1, z + 1)) / 16.0f;
		sides   = (Noise(x - 1, y    , z + 1 ) + Noise(x + 1, y    , z + 1) + Noise(x    , y - 1, z + 1) + Noise(x    , y + 1, z + 1)) / 8.0f;
		center  = Noise(x, y, z + 1 ) / 4.0f;
		float zp1 = corners + sides + center;

		return zm1 / 4.0f + zo / 2.0f + zp1 / 4.0f;
	}

	float InterpolateNoise (float x) const 
	{
		int ix = ATOM_ftol(x);
		float fract = x - float(ix);
		
		float v1 = SmoothNoise(ix);
		float v2 = SmoothNoise(ix + 1);
		
		return Interpolate(v1, v2, fract);
	}

	float InterpolateNoise (float x, float y) const 
	{
		int ix = ATOM_ftol(x);
		float fractX = x - float(ix);

		int iy = ATOM_ftol(y);
		float fractY = y - float(iy);

		float v1 = SmoothNoise(ix    , iy);
		float v2 = SmoothNoise(ix + 1, iy);
		float v3 = SmoothNoise(ix    , iy + 1);
		float v4 = SmoothNoise(ix + 1, iy + 1);

		float i1 = Interpolate(v1, v2, fractX);
		float i2 = Interpolate(v3, v4, fractX);

		return Interpolate(i1, i2, fractY);
	}

	float InterpolateNoise (float x, float y, float z) const 
	{
		int ix = ATOM_ftol(x);
		float fractX = x - float(ix);

		int iy = ATOM_ftol(y);
		float fractY = y - float(iy);

		int iz = ATOM_ftol(z);
		float fractZ = z - float(iz);

		float v1 = SmoothNoise(ix    , iy    , iz);
		float v2 = SmoothNoise(ix + 1, iy    , iz);
		float v3 = SmoothNoise(ix    , iy + 1, iz);
		float v4 = SmoothNoise(ix + 1, iy + 1, iz);
		float v5 = SmoothNoise(ix    , iy    , iz + 1);
		float v6 = SmoothNoise(ix + 1, iy    , iz + 1);
		float v7 = SmoothNoise(ix    , iy + 1, iz + 1);
		float v8 = SmoothNoise(ix + 1, iy + 1, iz + 1);

		float i1 = Interpolate(v1, v2, fractX);
		float i2 = Interpolate(v3, v4, fractX);
		float i3 = Interpolate(v5, v6, fractX);
		float i4 = Interpolate(v7, v8, fractX);

		float i5 = Interpolate(i1, i2, fractY );
		float i6 = Interpolate(i3, i4, fractY );

		return Interpolate(i5, i6, fractZ );
	}

	float PerlinNoise1D (float x, float amp, float freq) const 
	{
		return InterpolateNoise(x * freq) * amp;
	}

	float PerlinNoise2D (float x, float y, float amp, float freqx, float freqy) const 
	{
		return InterpolateNoise(x * freqx, y * freqy) * amp;
	}

	float PerlinNoise3D (float x, float y, float z, float amp, float freqx, float freqy, float freqz) 
	{
		return InterpolateNoise(x * freqx, y * freqy, z * freqz) * amp;
	}

	void  CacheNoise (unsigned maxx) 
	{
		// TODO:
	}

	void  CacheNoise (unsigned maxx, unsigned maxy) 
	{
		// TODO
	}

	void  CacheNoise (unsigned maxx, unsigned maxy, unsigned maxz) 
	{
		//TODO
	}
};

#endif // __ATOM_MATH_PERLINNOISE_H


