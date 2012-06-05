#include "PCH.h"
#include "Graphics.h"
#include "Math.h"

void DrawCircle(float radius, Color color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.f, 0.f);

	for (int i = 0; i != 13; ++i)
	{
		float x = cosf(i / 12.f * 2.f * Pi) * radius;
		float y = sinf(i / 12.f * 2.f * Pi) * radius;
		glVertex2f(x, y);
	}

	glEnd();
}

void DrawRect(float width, float height, Color color)
{
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_QUADS);

	glVertex2f(-width * 0.5f, +height * 0.5f);
	glVertex2f(-width * 0.5f, -height * 0.5f);
	glVertex2f(+width * 0.5f, -height * 0.5f);
	glVertex2f(+width * 0.5f, +height * 0.5f);

	glEnd();
}

void DebugCircle(const Vec2& center, float radius)
{
	glColor4f(1.f, 0.f, 0.f, 1.f);
	glBegin(GL_LINE_LOOP);

	for (int i = 0; i != 13; ++i)
	{
		float x = cosf(i / 12.f * 2.f * Pi) * radius;
		float y = sinf(i / 12.f * 2.f * Pi) * radius;
		glVertex2f(x + center.x, y + center.y);
	}

	glEnd();
}

void DebugLine(const Vec2& start, const Vec2& end)
{
	glColor4f(1.f, 0.f, 0.f, 1.f);
	glBegin(GL_LINES);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, end.y);
	glEnd();
}

void DrawText(const Vec2& pos, float scale, const char* text)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(2.f);

	glPushMatrix();
	glTranslatef(pos.x, pos.y, 0.f);
	glScalef(scale, scale, 1.f);
	glColor4f(1.f, 0.f, 0.f, 1.f);
	for (int i = 0; text[i] != '\0'; ++i)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, text[i]);
	glPopMatrix();


	glLineWidth(1.f);
}