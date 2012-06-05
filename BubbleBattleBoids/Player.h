#pragma once

#include "GameObject.h"

// Player objects
class Player : public GameObject
{
public:
	enum PlayerNum
	{
		NNobody = -1,
		NOne,
		NTwo,
		NThree,
		NFour
	};

	Player(PlayerNum playerNum);
	virtual ~Player();

	PlayerNum GetPlayerNum() const { return m_PlayerNum; }

	virtual Type GetType() const { return TPlayer; }

	float GetMoveForce() const { return m_MoveForce; }
	void SetMoveForce(float force) { m_MoveForce = force; }

	float GetShieldStrength() const { return m_ShieldStrength; }
	float GetShieldForce() const { return m_ShieldForce; }

	void Hurt();

	virtual void PreUpdate();
	virtual void PostUpdate();
	virtual void Draw();
	virtual void Collision(GameObject* other, const Vec2& point, const Vec2& normal);

private:
	float m_MoveForce;
	float m_ShieldPressure;
	float m_ShieldStrength;
	float m_ShieldStrengthLast;
	float m_ShieldForce;
	float m_HitTimer;
	float m_Damage;
	PlayerNum m_PlayerNum;
};