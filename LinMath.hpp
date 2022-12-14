#pragma once
#include <cmath>
#include <cstdlib>
#include <string>


static constexpr inline float fast_sqrt(float number)
{
	const float threehalfs = 1.5F;

	float x2 = number * 0.5F;
	float y  = number;
	long i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return 1.0f / y;
}

inline static constexpr float DegreesToRadians(float degree) noexcept
{
    return 3.1415926f * degree / 180.0f;
}

struct vec2D
{
	//--------------- constructors ---------------
	vec2D() : x(0.0f), y(0.0f) {}
	vec2D(float x, float y) : x(x), y(y){}
	vec2D(const vec2D& other) : x(other.x), y(other.y){}//copy constructor
	vec2D& operator=(const vec2D& other){
	    x = other.x;
	    y = other.y;
		return *this;
	}
	~vec2D(){};

	//----------------- math operators --------------
	inline vec2D operator+(const vec2D& other) const noexcept
	{
		return vec2D(x + other.x, y + other.y);
	}
	inline vec2D operator-(const vec2D& other) const noexcept
	{
		return vec2D(x - other.x, y - other.y);
	}
	inline vec2D operator*(float value) const noexcept
	{
		return vec2D(x * value, y * value);
	}
	inline vec2D operator/(float value) const noexcept
	{
		return vec2D(x / value, y / value);
	}
	inline bool operator!=(const vec2D& other) const noexcept
	{
		return x != other.x || y != other.y;
	}
    inline vec2D makePerpendicular() const noexcept
    {
        return vec2D(y, -x);
    }
	inline vec2D& translate(const vec2D& offset) noexcept
	{
        x += offset.x;
        y += offset.y;
        return *this;
	}
	inline vec2D& scale(float factor)
	{
        x *= factor;
        y *= factor;
        return *this;
	}
	inline vec2D& normalize()
	{
        return scale(1.0f / length());
	}
	inline constexpr float length() const noexcept
	{
		return std::sqrt(x * x + y * y);
	}
	inline constexpr float fast_length() const noexcept
	{
		return fast_sqrt(x * x + y * y);
	}

	//------------ math functions ----------------
	static inline float dot(const vec2D& vec1, const vec2D& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y;
	}

	float x, y;
};

inline static constexpr float sign(const vec2D& p, const vec2D& v1, const vec2D& v2) noexcept
{
    return (p.x - v2.x) * (v1.y - v2.y) - (v1.x - v2.x) * (p.y - v2.y);
}

inline static constexpr bool IsPointInTriangle(const vec2D& p, const vec2D& v1, const vec2D& v2, const vec2D& v3) noexcept
{
    bool b1 = sign(p, v1, v2) < 0.0f;
    bool b2 = sign(p, v2, v3) < 0.0f;
    bool b3 = sign(p, v3, v1) < 0.0f;
    return (b1 == b2) && (b2 == b3);
}
