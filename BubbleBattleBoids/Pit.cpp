#include "PCH.h"
#include "Pit.h"
#include "Graphics.h"
#include "Math.h"
#include "Boid.h"
#include "Game.h"

Pit::Pit(const Vec2& position) : GameObject(position), m_Timer(Rand(1.f, 20.f))
{
	SetShape(Rectangle);
}

void Pit::PreUpdate()
{
	if (Boid::CanSpawnBoid())
	{
		m_Timer -= g_FrameTime;
		if (m_Timer < 0.f)
		{
			m_Timer = Rand(0.f, 20.f);
			Boid::SpawnBoid(GetPosition());
		}
	}
}

void Pit::Draw()
{
	DrawRect(1.f, 1.f, Color(0.f, 0.f, 0.f, 1.f));
}

void Pit::Collision(GameObject* other, const Vec2& point, const Vec2& normal)
{
	if (other->GetType() == TPlayer)
	{
		// player's center is in the pit; "fall in"
		if (point == other->GetPosition())
		{
			if (other->GetVelocity().Length() < 5.f)
				other->Destroy();
		}
	}
}