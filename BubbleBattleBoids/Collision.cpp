#include "PCH.h"
#include "Collision.h"

bool CircleRectCollision2(const Vec2& cPos, float cRadius, const Vec2& rPos, const Vec2& rHalf)
{
	if (cPos.x > rPos.x + rHalf.x + cRadius)
		return false;
	if (cPos.x < rPos.x - rHalf.x - cRadius)
		return false;
	if (cPos.y > rPos.y + rHalf.y + cRadius)
		return false;
	if (cPos.y < rPos.y - rHalf.y - cRadius)
		return false;
	
	if ((cPos - rPos).Length() > rHalf.Length() + cRadius)
		return false;

	return true;
}

bool CircleRectCollision(const Vec2& cPos, float cRadius, const Vec2& rPos, const Vec2& rHalf, Vec2& outPoint, Vec2& outNormal)
{
	Vec2 cRel = cPos - rPos;

	// find closest feature (point) and normal
	if (cRel.x <= -rHalf.x)
	{
		if (cRel.y <= -rHalf.y)
		{
			outPoint = rPos + Vec2(-rHalf.x, -rHalf.y);
			outNormal = cPos - outPoint;
		}
		else if (cRel.y >= rHalf.y)
		{
			outPoint = rPos + Vec2(-rHalf.x, +rHalf.y);
			outNormal = cPos - outPoint;
		}
		else
		{
			outPoint = rPos + Vec2(-rHalf.x, cRel.y);
			outNormal = Vec2(-1.f, 0.f);
		}
	}
	else if (cRel.x >= rHalf.x)
	{
		if (cRel.y <= -rHalf.y)
		{
			outPoint = rPos + Vec2(+rHalf.x, -rHalf.y);
			outNormal = cPos - outPoint;
		}
		else if (cRel.y >= rHalf.y)
		{
			outPoint = rPos + Vec2(+rHalf.x, +rHalf.y);
			outNormal = cPos - outPoint;
		}
		else
		{
			outPoint = rPos + Vec2(+rHalf.x, cRel.y);
			outNormal = Vec2(+1.f, 0.f);
		}
	}
	else
	{
		if (cRel.y <= -rHalf.y)
		{
			outPoint = rPos + Vec2(cRel.x, -rHalf.y);
			outNormal = Vec2(0.f, -1.f);
		}
		else if (cRel.y >= rHalf.y)
		{
			outPoint = rPos + Vec2(cRel.x, +rHalf.y);
			outNormal = Vec2(0.f, +1.f);
		}
		else
		{
			outPoint = cPos;
			outNormal = Vec2(1.f, 0.f); // arbitrary vector
		}
	}
	
	if ((cPos - outPoint).Length() < cRadius)
	{
		outNormal = outNormal.Normalize();
		return true;
	}
	else
		return false;
}

bool CircleCircleCollision(const Vec2& c1Pos, float c1Radius, const Vec2& c2Pos, float c2Radius, Vec2& outPoint, Vec2& outNormal)
{
	outNormal = c1Pos - c2Pos;
	outNormal.Normalize();
	if (outNormal.Length() <= c1Radius + c2Radius)
	{
		outPoint = c2Pos + outNormal * c2Radius;
		return true;
	}
	else
		return false;
}

bool RectRectCollision2(const Vec2& r1Pos, const Vec2& r1Half, const Vec2& r2Pos, const Vec2& r2Half)
{
	if (r1Pos.x - r1Half.x > r2Pos.x + r2Half.x)
		return false;
	if (r1Pos.x + r1Half.x < r2Pos.x - r2Half.x)
		return false;
	if (r1Pos.y - r1Half.y > r2Pos.y + r2Half.y)
		return false;
	if (r1Pos.y + r1Half.y < r2Pos.y - r2Half.y)
		return false;

	return true;
}