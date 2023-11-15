#pragma once
#include <iostream>
#include <random>
#include <cstdint>
#include <array>

#define _USE_MATH_DEFINES
#include <math.h>


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
};

class Perlin
{
public :
	Perlin() 
	{
		srand(time(nullptr));
		m_periodMask = random1D.size() - 1;
		for (size_t i = 0; i < random1D.size(); i++)
		{
			random1D[i] = Math::randomf(-1, 1);
		}

		for (size_t y = 0; y < random2D.size(); y++)
		{
			for (size_t x = 0; x < random2D.size(); x++)
			{
				random2D[y][x] = Math::randomf(0, 1);
			}
		}
	}

	float noise1D(float x)
	{
		x *= frequency;
		x += offset.x;

		int minID = (int)std::floor(x) & m_periodMask;
		int maxID = minID + 1 < random1D.size() ? minID + 1 : 0;
		float t = x - minID;

		return Math::smoothLerp(random1D[minID], random1D[maxID], t) * amplitude;
	}

	float noise2D(Vec2 p)
	{
		p *= frequency;
		p += offset;

		int west = (int)std::floor(p.x) & m_periodMask;
		int east = west + 1 < random2D[0].size() ? west + 1 : 0;

		int north = (int)std::floor(p.y) & m_periodMask;
		int south = north + 1 < random2D.size() ? north + 1 : 0;

		Vec2 p00(west, south);
		Vec2 p01(east, south);
		Vec2 p10(west, north);
		Vec2 p11(east, north);

		Vec2 delta(p.x - west, p.y - north);
		float dxNorth = Math::lerp(random2D[p10.y][p10.x], random2D[p11.y][p11.x], delta.x);
		float dxSouth = Math::lerp(random2D[p00.y][p00.x], random2D[p01.y][p01.x], delta.x);

		return Math::smoothLerp(dxNorth, dxSouth, delta.y);
	}

	float amplitude = 100;
	float frequency = 0.1;
	int octaves = 3;
	
	std::array<float, 256> random1D;
	std::array<std::array<float, 256>, 256> random2D;
	Vec2 offset;


private :
	int m_periodMask;
};