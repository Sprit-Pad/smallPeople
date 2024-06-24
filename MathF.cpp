#include "MathF.h"
#include <stdlib.h>

float MathF::Lerp(float a, float b, float t)
{
	return a * (1.0f - t) + (b * t);
}

float MathF::RamdomRange(float min, float max)
{
	float value = static_cast<float>(rand()) / RAND_MAX;
	return min + (max - min) * value;
}