#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#include <random>
#include <cstdint>
#include <array>

#define RANDOM_SIZE_1D 256 //Must be 2^n
#define PERMUTATION_SIZE RANDOM_SIZE_1D * 2

class Vec2
{
public :
	Vec2()
	{
		x = 0;
		y = 0;
	}

	Vec2(float x, float y) : x(x), y(y)
	{

	}

	void operator+=(const Vec2& a)
	{
		this->x += a.x;
		this->y += a.y;
	}

	void operator*=(float t)
	{
		this->x *= t;
		this->y *= t;
	}

	float x;
	float y;
};

class Math
{
public:
	static float lerp(float a, float b, float t)
	{
		return (1 - t) * a + b * t;
	}

	static float smoothLerp(float a, float b, float t)
	{
		return lerp(a, b, (1 - cos(t * M_PI)) * 0.5);
	}

	static float findScale(float a, float b, float x)
	{
		return (x - a) / (b - a);
	}

	static float dot(Vec2 a, Vec2 b)
	{
		return a.x * b.x + a.y * b.y;
	}

	static float randomf(float min, float max)
	{		
		return min + (float)rand() / (float)(RAND_MAX / (max - min));
	}

	static int random(int min, int max)
	{
		return rand() % max + min;
	}
};

class Perlin
{
public :
	Perlin() 
	{
		srand(time(nullptr));
		m_periodMask = random.size() - 1;
		for (size_t i = 0; i < random.size(); i++)
		{
			random[i] = Math::randomf(-1, 1);
		}

		for (size_t i = 0; i < m_permutation.size(); i++)
		{
			m_permutation[i] = i % RANDOM_SIZE_1D;
		}

		std::random_shuffle(m_permutation.begin(), m_permutation.end());
	}

	float noise1D(float x)
	{
		x *= frequency;
		x += offset.x;

		int minID = (int)std::floor(x) & m_periodMask;
		int maxID = minID + 1 < random.size() ? minID + 1 : 0;
		float t = x - minID;

		return Math::smoothLerp(random[minID], random[maxID], t) * amplitude;
	}

	float noise2D(Vec2 p)
	{
		p *= frequency;
		p += offset;

		int west = (int)std::floor(p.x) & m_periodMask;
		int east = west + 1 < random.size() ? west + 1 : 0;

		int north = (int)std::floor(p.y) & m_periodMask;
		int south = north + 1 < random.size() ? north + 1 : 0;

		/*
		p10         p11
		*-----------*
		|	 		|
		|	 		|
		|			|
		|			|
		*-----------*
		p00			p01
		*/

		float p00 = random[m_permutation[m_permutation[west] + south]];
		float p01 = random[m_permutation[m_permutation[east] + south]];
		float p10 = random[m_permutation[m_permutation[west] + north]];
		float p11 = random[m_permutation[m_permutation[east] + north]];

		Vec2 delta(p.x - west, p.y - north);
		float dxNorth = Math::lerp(p10, p11, delta.x);
		float dxSouth = Math::lerp(p00, p01, delta.x);

		return Math::smoothLerp(dxNorth, dxSouth, delta.y);
	}

	float amplitude = 100;
	float frequency = 0.1;
	int octaves = 3;

	std::array<float, RANDOM_SIZE_1D> random;
	Vec2 offset;
	
private :
	int m_periodMask;
	std::array<std::uint32_t, PERMUTATION_SIZE> m_permutation;
};