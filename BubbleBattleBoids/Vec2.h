#pragma once

struct Vec2
{
	Vec2() : x(0.f), y(0.f) {}
	Vec2(float x, float y) : x(x), y(y) {}

	float x;
	float y;

	float Length() const { return std::sqrt(Dot(*this)); }
	Vec2 Normalize() const { ASSERT(!IsZero()); return Vec2(x / Length(), y / Length()); }
	bool IsZero() const { return x == 0.f && y == 0.f; }
	Vec2 Invert() const { return Vec2(1.f / x, 1.f / y); }
	inline Vec2 Clamp(float max) const;
	float Dot(const Vec2& rhs) const { return x*rhs.x + y*rhs.y; }
};

inline Vec2 operator+(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x + rhs.x, lhs.y + rhs.y); }
inline Vec2 operator-(const Vec2& lhs, const Vec2& rhs) { return Vec2(lhs.x - rhs.x, lhs.y - rhs.y); }
inline Vec2 operator-(const Vec2& rhs) { return Vec2(-rhs.x, -rhs.y); }
inline Vec2 operator*(float lhs, const Vec2& rhs) { return Vec2(lhs * rhs.x, lhs * rhs.y); }
inline Vec2 operator*(const Vec2& lhs, float rhs) { return Vec2(lhs.x * rhs, lhs.y * rhs); }
inline Vec2 operator/(const Vec2& lhs, float rhs) { return Vec2(lhs.x / rhs, lhs.y / rhs); }
inline Vec2& operator+=(Vec2& lhs, const Vec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
inline Vec2& operator*=(Vec2& lhs, float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
inline Vec2& operator/=(Vec2& lhs, float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }

inline bool operator==(const Vec2& lhs, const Vec2& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

Vec2 Vec2::Clamp(float max) const
{
	if (Length() > max)
		return Normalize() * max;
	else
		return *this;
}