#pragma once

#include "Vec2.h"

// Color object
struct Color
{
public:
	Color() : r(1.f), g(1.f), b(1.f), a(1.f) {}
	Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}

	float r, g, b, a;
};

inline Color operator*(const Color& lhs, const Color& rhs) { return Color(lhs.r*rhs.r, lhs.g*rhs.g, lhs.b*rhs.b, lhs.a*rhs.a); }
inline Color operator+(const Color& lhs, const Color& rhs) { return Color(lhs.r+rhs.r, lhs.g+rhs.g, lhs.b+rhs.b, lhs.a+rhs.a); }
inline Color operator*(const Color& lhs, float rhs) { return Color(lhs.r*rhs, lhs.g*rhs, lhs.b*rhs, lhs.a*rhs); }
inline Color operator*(float lhs, const Color& rhs) { return Color(lhs*rhs.r, lhs*rhs.g, lhs*rhs.b, lhs*rhs.a); }
inline Color& operator*=(Color& lhs, float rhs) { lhs.r *= rhs, lhs.g *= rhs, lhs.b *= rhs, lhs.a *= rhs; return lhs; }
inline Color& operator*=(Color& lhs, const Color& rhs) { lhs.r *= rhs.r, lhs.g *= rhs.g, lhs.b *= rhs.b, lhs.a *= rhs.a; return lhs; }

void DrawCircle(float radius, Color color);
void DrawRect(float width, float height, Color color);

void DebugCircle(const Vec2& center, float radius);
void DebugLine(const Vec2& start, const Vec2& end);

void DrawText(const Vec2& pos, float scale, const char* string);