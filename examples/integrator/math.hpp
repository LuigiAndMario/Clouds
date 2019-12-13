#ifndef INCLUDE_MATH
#define INCLUDE_MATH

// ============================================================
template <typename T>
class vec2
{
public:
	vec2() { mData[0] = mData[1] = (T)0; }
	vec2(const T& x, const T& y) { mData[0] = x; mData[1] = y; }
	T& x() { return mData[0]; }
	T& y() { return mData[1]; }
	const T& x() const { return mData[0]; }
	const T& y() const { return mData[1]; }
	T* ptr() { return mData; }
	vec2<T> operator +(const vec2<T>& other) const { return vec2<T>(mData[0] + other.mData[0], mData[1] + other.mData[1]); }
	vec2<T> operator -(const vec2<T>& other) const { return vec2<T>(mData[0] - other.mData[0], mData[1] - other.mData[1]); }
	vec2<T> operator *(const vec2<T>& other) const { return vec2<T>(mData[0] * other.mData[0], mData[1] * other.mData[1]); }
	vec2<T> operator /(const vec2<T>& other) const { return vec2<T>(mData[0] / other.mData[0], mData[1] / other.mData[1]); }
	vec2<T> operator *(const T& other) const { return vec2<T>(mData[0] * other, mData[1] * other); }
	vec2<T> operator /(const T& other) const { return vec2<T>(mData[0] / other, mData[1] / other); }
	T lengthSqr() const { return mData[0] * mData[0] + mData[1] * mData[1]; }
	T length() const { return std::sqrt(lengthSqr()); }
private:
	T mData[2];
};
typedef vec2<float> vec2f;
typedef vec2<int> vec2i;

// ============================================================
template <typename T>
class vec3
{
public:
	vec3() { mData[0] = mData[1] = mData[2] = (T)0; }
	vec3(const vec2<T>& xy, const T& z) { mData[0] = xy.x();  mData[1] = xy.y(); mData[2] = z; }
	vec3(const T& x, const T& y, const T& z) { mData[0] = x; mData[1] = y; mData[2] = z; }
	T& x() { return mData[0]; }
	T& y() { return mData[1]; }
	T& z() { return mData[2]; }
	const T& x() const { return mData[0]; }
	const T& y() const { return mData[1]; }
	const T& z() const { return mData[2]; }
	const vec2<T> xy() const { return vec2<T>(mData[0], mData[1]); }
	T* ptr() { return mData; }
	vec3<T> operator +(const vec3<T>& other) const { return vec3<T>(mData[0] + other.mData[0], mData[1] + other.mData[1], mData[2] + other.mData[2]); }
	vec3<T> operator -(const vec3<T>& other) const { return vec3<T>(mData[0] - other.mData[0], mData[1] - other.mData[1], mData[2] - other.mData[2]); }
	vec3<T> operator *(const vec3<T>& other) const { return vec3<T>(mData[0] * other.mData[0], mData[1] * other.mData[1], mData[2] * other.mData[2]); }
	vec3<T> operator /(const vec3<T>& other) const { return vec3<T>(mData[0] / other.mData[0], mData[1] / other.mData[1], mData[2] / other.mData[2]); }
	vec3<T> operator *(const T& other) const { return vec3<T>(mData[0] * other, mData[1] * other, mData[2] * other); }
	vec3<T> operator /(const T& other) const { return vec3<T>(mData[0] / other, mData[1] / other, mData[2] / other); }
private:
	T mData[3];
};
typedef vec3<float> vec3f;
typedef vec3<int> vec3i;


#endif // !INCLUDE_MATH
