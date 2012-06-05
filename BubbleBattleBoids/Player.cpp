#include "PCH.h"
#include "Player.h"
#include "Game.h"
#include "Graphics.h"

Player::Player(PlayerNum playerNum) : m_MoveForce(20.f), m_ShieldPressure(0.f), m_ShieldStrength(0.f), m_ShieldStrengthLast(0.f), m_ShieldForce(0.f), m_HitTimer(0.f), m_Damage(0.f), m_PlayerNum(playerNum), GameObject(Vec2())
{
	ASSERT(playerNum != NNobody);
	ASSERT(g_Players[playerNum] == NULL);

	SetMass(1.0f);
	SetDamping(.9f);

	switch (playerNum)
	{
	case NOne:
		SetPosition(Vec2(5.f, 5.f));
		break;
	case NTwo:
		SetPosition(Vec2(15.f, 10.f));
		break;
	case NThree:
		SetPosition(Vec2(5.f, 10.f));
		break;
	case NFour:
		SetPosition(Vec2(15.f, 5.f));
		break;
	}

	g_Players[playerNum] = this;
}

Player::~Player()
{
	ASSERT(g_Players[m_PlayerNum] == this);

	g_Players[m_PlayerNum] = NULL;
}

void Player::Hurt()
{
	if (m_HitTimer == 0.f)
	{
		m_HitTimer = 0.3f;
		m_Damage += 1.f;

		if (m_Damage >= 5.f)
			Destroy();
	}
}

void Player::PreUpdate()
{
	// pre-update shield
	m_ShieldStrengthLast = m_ShieldStrength;
	m_ShieldStrength = std::max(0.f, m_ShieldStrength - g_FrameTime * 3.f);

	// apply input
	XINPUT_STATE state;
	memset(&state, 0, sizeof(state));
	DWORD rs = XInputGetState(m_PlayerNum, &state);
	if (rs == ERROR_SUCCESS)
	{
		// update movement
		Vec2 left(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY);
		float leftMag = left.Length();
		if (leftMag > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			leftMag = (leftMag - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) / (float)(UINT16_MAX - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			left = left.Normalize() * leftMag;

			AddForce(left * GetMoveForce());
		}

		// update shield
		unsigned char rightTrigger = state.Gamepad.bRightTrigger;
		if (rightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			float shield = (rightTrigger - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) / (float)(UINT8_MAX - XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			if (shield > m_ShieldPressure)
			{
				m_ShieldStrength = std::min(1.f, m_ShieldStrength + shield - m_ShieldPressure);
				m_ShieldPressure = shield;	
			}
		}
		else
			m_ShieldPressure = 0.f;
	}

	// update shield
	m_ShieldForce = std::max(0.f, (m_ShieldStrength - m_ShieldStrengthLast) / g_FrameTime * 0.05f);

	// update scale for size plus shield
	SetScale(1.f + m_ShieldStrength);
}

void Player::PostUpdate()
{
	// heal
	m_Damage = std::max(0.f, m_Damage - g_FrameTime * 0.5f);
	m_HitTimer = std::max(0.f, m_HitTimer - g_FrameTime);

	// lock to world
	if (GetPosition().x < 0.f)
	{
		SetPosition(Vec2(0.f, GetPosition().y));
		SetVelocity(Vec2(0.f, GetVelocity().y));
	}
	if (GetPosition().x > 20.f)
	{
		SetPosition(Vec2(20.f, GetPosition().y));
		SetVelocity(Vec2(0.f, GetVelocity().y));
	}
	if (GetPosition().y < 0.f)
	{
		SetPosition(Vec2(GetPosition().x, 0.f));
		SetVelocity(Vec2(GetVelocity().x, 0.f));
	}
	if (GetPosition().y > 15.f)
	{
		SetPosition(Vec2(GetPosition().x, 15.f));
		SetVelocity(Vec2(GetVelocity().x, 0.f));
	}
}

void Player::Draw()
{
	Color color;
	switch (m_PlayerNum)
	{
	case NOne:
		color = Color(0.1f, 0.1f, .9f, 1.f);
		break;
	case NTwo:
		color = Color(.9f, 0.1f, .1f, 1.f);
		break;
	case NThree:
		color = Color(0.5f, 1.f, .9f, 1.f);
		break;
	case NFour:
		color = Color(0.1f, 1.f, 0.2f, 1.f);
		break;
	}

	const Color hurt(1.f, 0.5f, 0.f, 1.f);
	float t = (5.f - m_Damage) / 5.f;
	color = t * color + (1.f - t) * hurt;

	DrawCircle(0.5f * GetScale(), Color(0.f, 1.f, 1.f, 1.f));
	DrawCircle(0.5f, color);
}

void Player::Collision(GameObject* other, const Vec2& point, const Vec2& normal)
{
	if (m_ShieldStrength == 0.f)
	{
		if (other->GetType() == TBoid)
			Hurt();
		else if ((other->GetVelocity() - GetVelocity()).Length() >= 8.f)
			Hurt();
	}

	if (other->GetType() == TPlayer)
	{
		// bounce effect
		Vec2 normal = other->GetPosition() - GetPosition();
		normal = normal.Normalize();

		Vec2 relVel = other->GetVelocity() - GetVelocity();
		float relVelMag = relVel.Length();

		float mass = GetMass() + other->GetMass();

		Vec2 vel1 = GetVelocity();
		vel1 += normal * GetShieldForce();

		Vec2 vel2 = other->GetVelocity();
		vel2 += -normal * ((Player*)other)->GetShieldForce();

		Vec2 v1 = ((GetMass() - other->GetMass()) / mass) * vel1 + (2.f * other->GetMass() / mass) * vel2 - GetVelocity();
		Vec2 v2 = ((other->GetMass() - GetMass()) / mass) * vel2 + (2.f * GetMass() / mass) * vel1 - other->GetVelocity();
		AddForce(v1 / g_FrameTime);
		other->AddForce(v2 / g_FrameTime);

		SetPosition(other->GetPosition() + -normal * (GetScale() + other->GetScale() * 0.5f));
	}
}