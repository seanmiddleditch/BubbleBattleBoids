#pragma once

#include "GameObject.h"

// Pit objects
class Pit : public GameObject
{
public:
	Pit(const Vec2& position);

	virtual Type GetType() const { return TPit; }

	virtual void PreUpdate();
	virtual void PostUpdate() {}
	virtual void Draw();
	virtual void Collision(GameObject* other, const Vec2& point, const Vec2& normal);

private:
	float m_Timer;
};