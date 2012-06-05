#pragma once

#include "GameObject.h"

// Boid objects
class Boid : public GameObject
{
public:
	Boid(const Vec2& position);
	virtual ~Boid();

	virtual Type GetType() const { return TBoid; }

	float GetAnger() const { return m_Anger; }
	bool IsAngry() const { return m_Anger >= 0.75f; }

	virtual void PreUpdate();
	virtual void PostUpdate();
	virtual void Draw();
	virtual void Collision(GameObject* other, const Vec2& point, const Vec2& normal);

	static bool CanSpawnBoid();
	static void SpawnBoid(const Vec2& pos);

	float GetMaxSpeed() const { return m_MaxSpeed; }
	void SetMaxSpeed(float speed) { m_MaxSpeed = speed; }

	float GetMaxSteeringForce() const { return m_MaxSteeringForce; }
	void SetMaxSteeringForce(float force) { m_MaxSteeringForce = force; }

private:
	struct BoidFriend
	{
		Boid* boid;
		float distance;
	};

	void InsertClosest(Boid* boid, float distance, BoidFriend* boids, int& count, int maxCount);
	void FindClosest(BoidFriend* boids, int& count, int maxCount);
	Vec2 Steer(const Vec2& target) const;
	Vec2 Flock(BoidFriend* boids, int count);
	Vec2 Attack();
	void AvoidWalls();
	Vec2 Wander();

private:
	float m_MaxSpeed;
	float m_MaxSteeringForce;
	float m_Anger;
	float m_Timer;
	float m_WanderTheta;

	static int s_Count;
};