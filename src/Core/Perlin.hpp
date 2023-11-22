#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

#include <random>
#include <cstdint>
#include <array>

#define RANDOM_SIZE_1D 512						//Must be 2^n
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

	Vec2 operator*(float t)
	{
		return { this->x * t, this->y * t };
	}

	Vec2 operator/(float t)
	{
		return { this->x / t, this->y / t };
	}

	Vec2 operator-(Vec2 a)
	{
		return { this->x - a.x, this->y - a.y };
	}

	float x;
	float y;
};

class Math
{
public:
	static float distance(Vec2 a, Vec2 b)
	{
		Vec2 dt = a - b;
		return sqrt(dt.x * dt.x + dt.y * dt.y);
	}

	static float length(Vec2 a)
	{
		return sqrt(a.x * a.x + a.y * a.y);
	}

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

	static Vec2 normalize(Vec2 a)
	{
		return a / Math::length(a);
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

enum class NoiseType
{
	Perlin1D = 0,
	Fractal = 1,
	Turbulence = 2,
	End = 3
};

enum class LerpType
{
	Linear = 0,
	Cosine = 1,
	End = 2
};

class Perlin
{
public:
	Perlin(float seed)
	{
		srand(seed);
		m_periodMask = RANDOM_SIZE_1D - 1;
		for (size_t i = 0; i < RANDOM_SIZE_1D; i++)
		{
			random[i] = Math::normalize(Vec2(Math::randomf(-1, 1), Math::randomf(-1, 1)));
		}

		for (size_t i = 0; i < PERMUTATION_SIZE; i++)
		{
			m_permutation[i] = i % RANDOM_SIZE_1D;
		}

		std::random_shuffle(m_permutation.begin(), m_permutation.end());
	}

	float noise1D(float x)
	{
		float baseAmplitude = this->amplitude;
		float baseFrequency = this->frequency;

		float noiseHeight = 0;
		for (size_t i = 0; i < octaves; i++)
		{
			noiseHeight += baseNoise1D(x);
			this->amplitude *= persistance;
			this->frequency *= lacunarity;
		}

		this->amplitude = baseAmplitude;
		this->frequency = baseFrequency;
		return noiseHeight > 1 ? 1 : noiseHeight;
	}

	float noise2D(Vec2 p)
	{
		float baseAmplitude = this->amplitude;
		float baseFrequency = this->frequency;

		float maxHeight = 0;
		if (renderForImage)
		{
			for (size_t i = 0; i < octaves; i++)
			{
				maxHeight += this->amplitude;
				this->amplitude *= persistance;
				this->frequency *= lacunarity;
			}
		}

		this->amplitude = baseAmplitude;
		this->frequency = baseFrequency;

		float noiseHeight = 0;
		switch (type)
		{
		case NoiseType::Fractal:
		default:
			for (size_t i = 0; i < octaves; i++)
			{
				noiseHeight += baseNoise2D(p);
				this->amplitude *= persistance;
				this->frequency *= lacunarity;
			}
			break;

		case NoiseType::Turbulence:
			for (size_t i = 0; i < octaves; i++)
			{
				noiseHeight += abs(baseNoise2D(p));
				this->amplitude *= persistance;
				this->frequency *= lacunarity;
			}
			break;
		}

		this->amplitude = baseAmplitude;
		this->frequency = baseFrequency;
		return renderForImage ? noiseHeight / maxHeight : noiseHeight;
	}

private:
	float baseNoise1D(float x)
	{
		x *= frequency;
		x += offset.x;

		int minID = (int)std::floor(x) & m_periodMask;
		int maxID = minID + 1 < random.size() ? minID + 1 : 0;
		float t = x - minID;

		float height = 0;
		switch (lerp)
		{
		case LerpType::Linear:
			height = Math::lerp(random[minID].x, random[maxID].x, t);
			break;

		case LerpType::Cosine:
		default:
			height = Math::smoothLerp(random[minID].x, random[maxID].x, t);
			break;
		}

		return height * amplitude;
	}

	float baseNoise2D(Vec2 p)
	{
		p *= frequency;
		p += offset;

		int west = (int)std::floor(p.x) & m_periodMask;
		int east = west + 1 < random.size() ? west + 1 : 0;

		int north = (int)std::floor(p.y) & m_periodMask;
		int south = north + 1 < random.size() ? north + 1 : 0;

		/*
		v10         v11
		*-----------*
		|	 		|
		|	 		|
		|			|
		|			|
		*-----------*
		v00			v01
		*/

		Vec2 v00 = random[m_permutation[m_permutation[west] + south]];
		Vec2 v01 = random[m_permutation[m_permutation[east] + south]];
		Vec2 v10 = random[m_permutation[m_permutation[west] + north]];
		Vec2 v11 = random[m_permutation[m_permutation[east] + north]];

		float p00 = Math::dot(v00, p - Vec2(west, south));
		float p01 = Math::dot(v01, p - Vec2(east, south));
		float p10 = Math::dot(v10, p - Vec2(west, north));
		float p11 = Math::dot(v11, p - Vec2(east, north));

		Vec2 delta(p.x - west, p.y - north);
		float dxNorth = Math::smoothLerp(p10, p11, delta.x);
		float dxSouth = Math::smoothLerp(p00, p01, delta.x);

		float height = 0;
		switch (lerp)
		{
		case LerpType::Linear:
			height = Math::lerp(dxNorth, dxSouth, delta.y);
			break;

		case LerpType::Cosine:
		default:
			height = Math::smoothLerp(dxNorth, dxSouth, delta.y);
			break;
		}

		switch (type)
		{
		case NoiseType::Fractal:
		default:
			return (height + 1) / 2 * amplitude;
			break;

		case NoiseType::Turbulence:
			return height * amplitude;
			break;
		}	
	}

public:
	float amplitude = 1;
	float frequency = 0.03;
	float lacunarity = 2;
	float persistance = 0.5;
	int octaves = 4;

	std::array<Vec2, RANDOM_SIZE_1D> random;
	Vec2 offset;

	bool renderForImage = true;
	NoiseType type = NoiseType::Fractal;
	LerpType lerp = LerpType::Cosine;

private:
	int m_periodMask;
	std::array<std::uint32_t, PERMUTATION_SIZE> m_permutation;
};