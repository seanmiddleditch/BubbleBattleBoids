#pragma once

#include "Vec2.h"

bool CircleRectCollision(const Vec2& cPos, float cRadius, const Vec2& rPos, const Vec2& rHalf, Vec2& outPoint, Vec2& outNormal);
bool CircleCircleCollision(const Vec2& c1Pos, float c1Radius, const Vec2& c2Pos, float c2Radius, Vec2& outPoint, Vec2& outNormal);