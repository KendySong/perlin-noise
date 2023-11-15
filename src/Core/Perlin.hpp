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
		for (size_t i = 0; i < random1D.size(); i++)
		{
			random1D[i] = Math::randomf(-1, 1);
		}
	}

	float noise1D(float x)
	{
		x *= frequency;
		x += offset.x;

		int minID = (int)std::floor(x) % random1D.size();
		int maxID = minID + 1 < random1D.size() ? minID + 1 : 0;
		float t = x - minID;

		return Math::smoothLerp(random1D[minID], random1D[maxID], t) * amplitude;
	}

	float amplitude = 100;
	float frequency = 1;
	Vec2 offset;

	std::array<float, 20> random1D;
};