#ifndef MATH
#define MATH
#include <math.h>

// constexpr means that the compiler will evaluate these expressions during compile-time. So runtime performance is
// improved
constexpr float PI = 3.14159265f;
constexpr double PI_D = 3.1415926535897932;

template<typename T>
constexpr auto square(const T &x)
{
	return x*x;
}

template<typename T>
constexpr T interpolate(const T &src, const T &dist, float alpha)
{
	return alpha * (dist-src) + src;
}

template<typename T>
constexpr T toRadians(T deg)
{
	return deg * PI / (T)180.0;
}

template<typename T>
constexpr T toDegrees(T rad)
{
	return rad * (T)180.0 / PI;
}

template<typename T>
T wrapAngle(T theta)
{
	const T remainder = fmod(theta,(T)2.0*(T)PI_D);
	return remainder > (T)PI_D ? remainder - (T)2.0 * (T)PI_D : remainder;
}
#endif