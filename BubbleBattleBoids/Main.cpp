// Bubble Battle Boids
// Sean Middleditch <sean@seanmiddleditch.com>
// Copyright (C) 2012

#include "PCH.h"
#include "Debug.h"
#include "Vec2.h"
#include "Graphics.h"
#include "Math.h"
#include "GameObject.h"
#include "Game.h"
#include "Player.h"
#include "Boid.h"
#include "Pit.h"
#include "Wall.h"
#include "Collision.h"

// GLUT callback functions
static void Display();
static void Reshape(int width, int height);
static void Idle();
static void Keyboard(unsigned char key, int x, int y);

// Game logic function
static void UpdateInput();
static void UpdatePhysics();
static void UpdateGameState();
static void ResetGame();

// Game state
float g_FrameTime = 0.f;
static LARGE_INTEGER g_LastFrame;
static LARGE_INTEGER g_Frequency;

enum State
{
	StateStart,
	StatePlay,
	StateDone
};
State g_State = StateStart;

Player* g_Players[MAX_PLAYERS] = { NULL };
Player::PlayerNum g_Winner = Player::NNobody;
int g_NumPlayers = 0;
double g_TotalTime = 0.f;

// Entry point
int main(int argc, char** argv)
{
	// initialize GLUT
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Bad-ass Bubble Boid Battle Bots Bonanza");

	glutDisplayFunc(&Display);
	glutReshapeFunc(&Reshape);
	glutIdleFunc(&Idle);
	glutKeyboardFunc(&Keyboard);

	ResetGame();

	// initialize frame timer
	QueryPerformanceFrequency(&g_Frequency);
	QueryPerformanceCounter(&g_LastFrame);

	// run game
	glutMainLoop();
	return 0;
}

void ResetGame()
{
	g_TotalTime = 0.f;

	// kill all existing objects
	for (GameObject* obj = GameObject::All(); obj != NULL; obj = obj->GetNext())
		obj->Destroy();

	g_NumPlayers = 0;
	g_Winner = Player::NNobody;
	
	// create outer walls
	new Wall(Vec2(-7.f, 7.5f), 15.f);
	new Wall(Vec2(27.f, 7.5f), 15.f);
	new Wall(Vec2(10.f, -9.5f), 20.f);
	new Wall(Vec2(10.f, 24.5f), 20.f);

	// create center area
	new Wall(Vec2(7.f, 8.f), 3.f);
	new Wall(Vec2(13.f, 6.f), 3.f);

	new Pit(Vec2(9.f, 8.f));
	new Pit(Vec2(9.f, 7.f));
	new Pit(Vec2(10.f, 7.f));
	new Pit(Vec2(11.f, 7.f));
	new Pit(Vec2(11.f, 6.f));

	// create corner walls/pits
	new Wall(Vec2(3.f, 12.f), 2.f);
	new Pit(Vec2(2.5f, 10.5f));
	new Pit(Vec2(3.5f, 10.5f));

	new Wall(Vec2(17.f, 3.f), 2.f);
	new Pit(Vec2(16.5f, 4.5f));
	new Pit(Vec2(17.5f, 4.5f));

	// create corner pits
	new Pit(Vec2(4.f, 1.f));
	new Pit(Vec2(3.f, 1.f));
	new Pit(Vec2(2.f, 1.f));
	new Pit(Vec2(1.f, 1.f));
	new Pit(Vec2(2.f, 2.f));
	new Pit(Vec2(1.f, 2.f));
	new Pit(Vec2(1.f, 3.f));
	new Pit(Vec2(1.f, 4.f));

	new Pit(Vec2(19.f, 11.f));
	new Pit(Vec2(19.f, 12.f));
	new Pit(Vec2(19.f, 13.f));
	new Pit(Vec2(19.f, 14.f));
	new Pit(Vec2(18.f, 13.f));
	new Pit(Vec2(18.f, 14.f));
	new Pit(Vec2(17.f, 14.f));
	new Pit(Vec2(16.f, 14.f));
}

// Render the scene
void Display()
{
	// initialize the rendering
	glViewport(0, 0, 800, 600);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 20, 0, 15, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// clear the screen
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	// draw all game objects
	for (GameObject* obj = GameObject::All(); obj != NULL; obj = obj->GetNext())
	{
		glPushMatrix();
		glTranslatef(obj->GetPosition().x, obj->GetPosition().y, 0.f);
		obj->Draw();
		glPopMatrix();
	}

	// screen mode
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, 0, 1);

	// timer
	char timer[64];
	sprintf_s(timer, "%02.2f", g_TotalTime);
	DrawText(Vec2(710.f, 585.f), .1, timer);

	// draw state text
	switch (g_State)
	{
	case StateStart:
		DrawText(Vec2(130.f, 300.f), .3f, "Press (A) to Join");
		if (g_NumPlayers > 0)
			DrawText(Vec2(110.f, 250.f), .3f, "Press Start to Play");
		break;
	case StatePlay:
		break;
	case StateDone:
		DrawText(Vec2(270.f, 300.f), .3f, "Game Over");
		switch (g_Winner)
		{
		case Player::NNobody:
			break;
		case Player::NOne:
			DrawText(Vec2(270.f, 250.f), .2f, "Player One Wins!");
			break;
		case Player::NTwo:
			DrawText(Vec2(270.f, 250.f), .2f, "Player One Wins!");
			break;
		case Player::NThree:
			DrawText(Vec2(270.f, 250.f), .2f, "Player One Wins!");
			break;
		case Player::NFour:
			DrawText(Vec2(270.f, 250.f), .2f, "Player One Wins!");
			break;
		}
		break;
	}

	// swap buffers
	glutSwapBuffers();
}

void Reshape(int width, int height)
{
}

void Idle()
{
	// update frame time
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	g_FrameTime = (now.QuadPart - g_LastFrame.QuadPart) / (double)g_Frequency.QuadPart;
	g_FrameTime = std::min(g_FrameTime, 1.f / 30.f);
	g_LastFrame = now;

	// run game logic
	UpdateInput();

	if (g_State == StatePlay)
	{
		g_TotalTime += g_FrameTime;

		for (GameObject* obj = GameObject::All(); obj != NULL; obj = obj->GetNext())
			obj->PreUpdate();

		UpdatePhysics();

		for (GameObject* obj = GameObject::All(); obj != NULL; obj = obj->GetNext())
			obj->PostUpdate();

		UpdateGameState();
	}

	// force redraw
	glutPostRedisplay();

	// clean dead objects
	GameObject::CleanUp();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case VK_ESCAPE:
		std::exit(0);
		break;
	}
}

// Game state
void UpdateInput()
{
	for (int i = 0; i != MAX_PLAYERS; ++i)
	{
		XINPUT_STATE state;
		memset(&state, 0, sizeof(state));
		DWORD rs = XInputGetState(i, &state);
		if (rs == ERROR_SUCCESS)
		{
			if (g_State == StateStart && g_Players[i] == NULL && 0 != (state.Gamepad.wButtons & XINPUT_GAMEPAD_A))
			{
				++g_NumPlayers;
				new Player((Player::PlayerNum)i);
			}

			if (g_State == StateStart && g_Players[i] != NULL && 0 != (state.Gamepad.wButtons & XINPUT_GAMEPAD_B))
			{
				--g_NumPlayers;
				g_Players[i]->Destroy();
			}


			if (g_State == StateStart && g_NumPlayers > 0 && 0 != (state.Gamepad.wButtons & XINPUT_GAMEPAD_START))
				g_State = StatePlay;

			if (g_State == StateDone && 0 != (state.Gamepad.wButtons & XINPUT_GAMEPAD_START))
			{
				ResetGame();
				g_State = StateStart;
			}

			if (g_State == StatePlay && g_NumPlayers == 1 && 0 != (state.Gamepad.wButtons & XINPUT_GAMEPAD_B))
			{
				ResetGame();
				g_State = StateStart;
			}

			if (g_State == StateStart && 0 != (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
				std::exit(0);
		}
	}
}

void UpdatePhysics()
{
	// integrate physics for all game objects
	for (GameObject* obj = GameObject::All(); obj != NULL; obj = obj->GetNext())
		obj->IntegratePhysics();

	// perform collision tests
	for (GameObject* obj1 = GameObject::All(); obj1 != NULL; obj1 = obj1->GetNext())
		for (GameObject* obj2 = obj1->GetNext(); obj2 != NULL; obj2 = obj2->GetNext())
		{
			bool hit = false;

			// point on obj2, normal from obj1 to obj2
			Vec2 point, normal;

			if (obj1->GetShape() == GameObject::Circle && obj2->GetShape() == GameObject::Circle)
				hit = CircleCircleCollision(obj1->GetPosition(), .5f * obj1->GetScale(), obj2->GetPosition(), .5f * obj2->GetScale(), point, normal);
			else if (obj1->GetShape() == GameObject::Circle && obj2->GetShape() == GameObject::Rectangle)
				hit = CircleRectCollision(obj1->GetPosition(), .5f * obj1->GetScale(), obj2->GetPosition(), Vec2(.5f, .5f) * obj2->GetScale(), point, normal);
			else
			{
				if (CircleRectCollision(obj2->GetPosition(), .5f * obj2->GetScale(), obj1->GetPosition(), Vec2(.5f, .5f) * obj1->GetScale(), point, normal))
				{
					hit = true;
					normal = -normal;
				}
			}

			if (hit)
			{
				obj1->Collision(obj2, point, -normal);
				obj2->Collision(obj1, point, normal);
			}
		}
}

void UpdateGameState()
{
	switch (g_State)
	{
	case StateStart:
	case StatePlay:
		{
			int count = 0;
			Player* winner = NULL;
			for (int i = 0; i != MAX_PLAYERS; ++i)
			{
				if (g_Players[i] != NULL)
				{
					++count;
					winner = g_Players[i];
				}
			}

			// game over, no winners
			if (count == 0)
			{
				g_Winner = Player::NNobody;
				g_State = StateDone;
			}

			// game over, one winner
			if (g_NumPlayers > 1 && count == 1)
			{
				g_Winner = winner->GetPlayerNum();
				g_State = StateDone;
			}
		}
		break;
	case StateDone:
		break;
	}
}