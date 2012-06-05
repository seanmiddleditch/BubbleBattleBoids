#include "PCH.h"
#include "Wall.h"
#include "Graphics.h"
#include "Math.h"
#include "Collision.h"
#include "Player.h"

Wall::Wall(const Vec2& position, float scale) : GameObject(position)
{
	SetScale(scale);
	SetShape(Rectangle);
}

void Wall::Collision(GameObject* other, const Vec2& point, const Vec2& normal)
{
	// get collision normal and tangent
	Vec2 tangent = Vec2(-normal.y, normal.x);

	// calculate velocity of object, adjusting for player shield
	Vec2 vel = other->GetVelocity();
	if (other->GetType() == TPlayer)
		vel += -normal * std::max(0.f, ((Player*)other)->GetShieldForce());

	if (normal.Dot(vel) < 0.f)
	{
		Vec2 velocity = -normal * normal.Dot(vel) + tangent * tangent.Dot(vel);
		other->SetVelocity(velocity);
	}

	// push apart
	other->SetPosition(point + normal * other->GetScale() * 0.51f);
}

void Wall::Draw()
{
	DrawRect(GetScale(), GetScale(), Color());
}