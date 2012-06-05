#include "PCH.h"
#include "Boid.h"
#include "Graphics.h"
#include "Math.h"
#include "Game.h"
#include "Player.h"

// Boid objects
int Boid::s_Count = 0;

Boid::Boid(const Vec2& position) : GameObject(position), m_MaxSpeed(1.f), m_MaxSteeringForce(1.f), m_Anger(0.f), m_WanderTheta(0.f)
{
	SetScale(0.4f);
	++s_Count;
}

Boid::~Boid()
{
	--s_Count;
}

struct NearestBoids
{
	Boid* boid;
	float distance;
};

void Boid::InsertClosest(Boid* boid, float distance, BoidFriend* boids, int& count, int maxCount)
{
	if (count < maxCount)
	{
		boids[count].boid = boid;
		boids[count].distance = distance;
		++count;
	}
	else
	{
		for (int i = 0; i != maxCount; ++i)
		{
			if (distance < boids[i].distance)
			{
				std::swap(boid, boids[i].boid);
				std::swap(distance, boids[i].distance);
				InsertClosest(boid, distance, boids, count, maxCount);
				break;
			}
		}
	}
}

void Boid::FindClosest(BoidFriend* boids, int& count, int maxCount)
{
	count = 0;

	for (GameObject* obj = GameObject::All(); obj != NULL; obj = obj->GetNext())
	{
		if (obj != this)
		{
			Vec2 delta = GetPosition() - obj->GetPosition();
			float dist = delta.Length();

			// average position and direction for boids close enough to matter
			if (dist < 2.25f)
				InsertClosest((Boid*)obj, dist, boids, count, maxCount);
		}
	}
}

void Boid::PreUpdate()
{
	// find nearest boids
	BoidFriend boids[5];
	int count = 0;
	FindClosest(boids, count, 5);

	// avoid walls
	AvoidWalls();

	// update anger
	if (count != 0)
	{
		float anger = 0.f;
		for (int i = 0; i != count; ++i)
			anger += boids[i].boid->m_Anger;
		anger /= count;

		// slowly adjust towards group anger
		if (anger > m_Anger)
			m_Anger += g_FrameTime * count;
		else
			m_Anger -= g_FrameTime * 0.5f;
	}

	m_Anger += Rand(0.f, 0.05f) * g_FrameTime;
	m_Anger = Clamp(m_Anger, 0.f, 1.f);

	// calculate flocking steering
	AddForce(Flock(boids, count));

	// attack if angry, otherwise flock and wander
	if (m_Anger > 0.5f)
	{
		float t = (m_Anger - 0.5f) * 2.f;
		AddForce(Attack() * t);
	}
	else
		AddForce(Wander());
}

Vec2 Boid::Flock(BoidFriend* boids, int count)
{
	Vec2 cohesion, alignment, separation;

	// average the positions and velocities of all boids in area
	int sepCount = 0;

	for (int i = 0; i != count; ++i)
	{
		cohesion += boids[i].boid->GetPosition() - GetPosition();
		alignment += boids[i].boid->GetVelocity();

		if (boids[i].distance < 1.25f)
		{
			Vec2 delta = (GetPosition() - boids[i].boid->GetPosition());
			if (!delta.IsZero())
			{
				separation += delta.Normalize() / boids[i].distance;
				++sepCount;
			}
		}
	}

	if (count != 0)
	{
		cohesion /= count;
		alignment /= count;
	}

	if (sepCount != 0)
		separation /= sepCount;

	return Steer(cohesion) + Steer(alignment) + Steer(separation);
}

Vec2 Boid::Attack()
{	
	// steer towards nearest player
	Player* nearest = NULL;
	Vec2 nearestV;
	float nearestD = FLT_MAX;
	for (int i = 0; i != MAX_PLAYERS; ++i)
	{
		if (g_Players[i] != NULL)
		{
			Vec2 v = g_Players[i]->GetPosition() - GetPosition();
			float d = v.Length();
			if (d < nearestD)
			{
				nearest = g_Players[i];
				nearestD = d;
				nearestV = v;
			}
		}
	}

	Vec2 player;
	if (nearest != NULL)
		return Steer(nearestV);
	else
		return Vec2();
}

Vec2 Boid::Wander()
{
	float r = 1.2f;
	float d = 1.5f;

	m_WanderTheta += Rand(-Pi, Pi) * g_FrameTime;
	float heading = std::atan2f(GetVelocity().y, GetVelocity().y);

	Vec2 dir = GetVelocity().Normalize() * d;
	Vec2 offset = Vec2(std::cos(m_WanderTheta + heading), std::sin(m_WanderTheta + heading)) * r;

	return Steer(offset + dir);
}

void Boid::AvoidWalls()
{
	for (GameObject* obj = GameObject::All(); obj != NULL; obj = obj->GetNext())
	{
		// do strong separation for walls
		if (obj->GetType() == GameObject::TWall)
		{
			Vec2 delta = GetPosition() - obj->GetPosition();
			float dist = delta.Length();
			if (dist < 2.f && dist > 0.f)
				AddForce(Steer(delta.Normalize() / dist));
		}
	}

	// avoid outer walls
	Vec2 steer;

	if (GetPosition().x < 1.f)
		steer += Steer(Vec2(1.f, 0.f));
	if (GetPosition().x > 19.f)
		steer += Steer(Vec2(-1.f, 0.f));
	if (GetPosition().y < 1.f)
		steer += Steer(Vec2(0.f, 1.f));
	if (GetPosition().y > 14.f)
		steer += Steer(Vec2(0.f, -1.f));

	AddForce(2.f * steer);
}

void Boid::PostUpdate()
{
}

void Boid::Draw()
{
	DrawCircle(0.5f * GetScale(), Color(1.f, 1.f - m_Anger * 0.5f, 0.f, 1.f));
}

Vec2 Boid::Steer(const Vec2& target) const
{
	if (!target.IsZero())
	{
		Vec2 dir = target.Normalize();
		dir *= m_MaxSpeed;
		Vec2 steer = dir - GetVelocity();
		steer = steer.Clamp(m_MaxSteeringForce);
		return steer;
	}
	else
		return Vec2();
}

bool Boid::CanSpawnBoid()
{
	return s_Count < 50;
}

void Boid::SpawnBoid(const Vec2& pos)
{
	Boid* boid = new Boid(pos);
	boid->SetMaxSpeed(4.f);
	boid->SetMaxSteeringForce(4.f);
	boid->SetVelocity((Vec2(Rand(-1.f, 1.f), Rand(-1.f, 1.f))).Normalize() * boid->GetMaxSpeed());
}

void Boid::Collision(GameObject* other, const Vec2& point, const Vec2& normal)
{
	if (other->GetType() == TPlayer)
	{
		// bounce effect
		Vec2 normal = other->GetPosition() - GetPosition();
		normal = normal.Normalize();

		Vec2 relVel = other->GetVelocity() - GetVelocity();
		float relVelMag = relVel.Length();

		float mass = GetMass() + other->GetMass();

		Vec2 pVel = other->GetVelocity();
		pVel += -normal * ((Player*)other)->GetShieldForce();

		Vec2 v1 = ((GetMass() - other->GetMass()) / mass) * GetVelocity() + (2.f * other->GetMass() / mass) * pVel - GetVelocity();
		Vec2 v2 = ((other->GetMass() - GetMass()) / mass) * other->GetVelocity() + (2.f * GetMass() / mass) * GetVelocity() - other->GetVelocity();
		AddForce(v1 / g_FrameTime);
		other->AddForce(v2 / g_FrameTime);

		SetPosition(other->GetPosition() + -normal * (GetScale() + other->GetScale() * 0.5f));
	}
	else if (other->GetType() == TBoid)
	{
		m_Anger += g_FrameTime;
	}
	else if (other->GetType() == TWall)
	{
		if (GetVelocity().Length() > 4.f)
			Destroy();
	}
}