/*
 * Util_Collision.c
 *
 *  Created on: May 25, 2015
 *      Author: begah
 */

#include "Util.h"

_Bool Square_Collide(int x, int y, int width, int height, int x2, int y2,
		int width2, int height2)
{
	if (x < x2 + width2 && x + width > x2 && y < y2 + height2
			&& y + height > y2)
		return true;
	return false;
}

_Bool Square_PreciseCollide(float x, float y, float x2, float y2, float xx,
		float yy, float xx2, float yy2)
{
	if (x < xx2 && x2 > xx && y < yy2 && y2 > yy)
		return true;
	return false;
}

float sign(struct Vector2f p1, struct Vector2f p2, struct Vector2f p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

_Bool Point_inSquare(struct Vector2f point, float x, float y, float width,
		float height)
{
	if (point.x >= x && point.x <= x + width && point.y >= y
			&& point.y <= y + height)
		return true;
	return false;
}

_Bool Point_inPreciseSquare(struct Vector2f point, float x, float y, float xx,
		float yy)
{
	if (point.x >= x && point.x <= xx && point.y >= y
			&& point.y <= yy)
		return true;
	return false;
}

_Bool Point_inTriangle(struct Vector2f point, struct Vector2f v1,
		struct Vector2f v2, struct Vector2f v3)
{
	_Bool b1, b2, b3;

	b1 = sign(point, v1, v2) < 0.0f;
	b2 = sign(point, v2, v3) < 0.0f;
	b3 = sign(point, v3, v1) < 0.0f;

	return ((b1 == b2) && (b2 == b3));
}

_Bool Point_inQuad(struct Vector2f point, struct Quad quad)
{
	return Point_inTriangle(point, quad.v1, quad.v2, quad.v4) || Point_inTriangle(point, quad.v2, quad.v3, quad.v4);
}
