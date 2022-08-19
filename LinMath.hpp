#pragma once
#include <cmath>

float fast_sqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return 1.0f / y;
}

struct vec2D
{
	//--------------- constructors ---------------
	vec2D() : x(0.0f), y(0.0f) {}
	vec2D(float x, float y) : x(x), y(y){}
	vec2D(const vec2D& other) : x(other.x), y(other.y){}//copy constructor
	vec2D& operator=(const vec2D& other){
		return *this = vec2D(other);
	}

	//----------------- math operators --------------
	vec2D operator+(const vec2D& other) const
	{
		return vec2D(x + other.x, y + other.y);
	}
	vec2D operator*(float value) const
	{
		return vec2D(x * value, y * value);
	}
	float length() const
	{
		return std::sqrt(x * x + y * y);
	}
	float fast_length() const
	{
		return std::sqrt(x * x + y * y);
	}

	//------------ math functions ----------------
	static float dot(const vec2D& vec1, const vec2D& vec2) const
	{
		return vec1.x * vec2.x + vec1.y * vec2.y;
	}

	float x, y;
};
