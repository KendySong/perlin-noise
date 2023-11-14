#pragma once
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

	static float unlerp(float a, float b, float x)
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
		for (size_t i = 0; i < m_random1D.size(); i++)
		{
			m_random1D[i] = Math::randomf(-10, 10);
		}
	}

	float noise1D(float x)
	{
		x *= frequency;
		int roundX = std::floor(x);
		int leftSide = roundX - (roundX % frequencyPixel);
		int rightSide = leftSide + frequencyPixel;
		float t = Math::unlerp(leftSide, rightSide, x);
		return Math::lerp(m_random1D[leftSide/ frequencyPixel], m_random1D[rightSide/ frequencyPixel], (1-cos(t * M_PI))/2) * this->amplitude;
	}

	int frequencyPixel = 75;
	float amplitude = 10;
	float frequency = 1;

private :
	std::array<float, 4096> m_random1D;
};