/*
 * Util_Math.c
 *
 *  Created on: Apr 19, 2015
 *      Author: begah
 */

#include "Util.h"

void Random_Init()
{
	srand(time(NULL));
}

// Randomly gives back true or false
_Bool Random_NextBool()
{
	return rand() % 2;
}

// Randomly gives an Int from 0 to i-1 ( never will it return i)
short Random_NextInt(int i)
{
	return rand() % i;
}

char *Integer_toString(int Number)
{
	int Size = 1;
	float Temp = Number;
	while (1)
	{
		if ((Temp /= 10) < 1)
		{
			break;
		}
		Size++;
	}

	Number = Integer_Reverse(Number);

	char *Char = malloc(sizeof(char) * (Size + 1));
	Char[Size] = '\0';
	Temp = 0;
	while (Temp < Size)
	{
		Char[(int) Temp] = (char) (((int) '0') + Integer_FirstDigit(Number));
		Temp++;
		Number /= 10;
	}
	return Char;
}

int Integer_Reverse(int Number)
{
	int reverse = 0;

	while (Number != 0)
	{
		reverse = reverse * 10;
		reverse = reverse + Number % 10;
		Number = Number / 10;
	}

	return reverse;
}

int Integer_FirstDigit(int Number)
{
	return Number % 10;
}

double View_TranslateTo(double pos, double OldDimension, double NewDimension)
{
	return (pos * NewDimension) / OldDimension;
}

struct Vector2f Vector2_Create(float x, float y)
{
	struct Vector2f vec;
	vec.x = x;
	vec.y = y;
	return vec;
}

struct Vector3f Vector3_Create(float x, float y, float z)
{
	struct Vector3f vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	return vec;
}

struct Vector4f Vector4_Create(float x, float y, float z, float w)
{
	struct Vector4f vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.w = w;
	return vec;
}

struct Vector6f Vector6_Create(struct Vector2f vec2, struct Vector4f vec4)
{
	struct Vector6f vec;
	vec.x = vec2.x;
	vec.y = vec2.y;
	vec.z = vec4.x;
	vec.w = vec4.y;
	vec.h = vec4.z;
	vec.o = vec4.w;
	return vec;
}

struct Vector6f Vector6_Createf(float x, float y, float z, float w, float h,
		float o)
{
	struct Vector6f vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.w = w;
	vec.h = h;
	vec.o = o;
	return vec;
}

struct Vertex Vertex_Create(struct Vector3f vec3, struct Vector2f vec2)
{
	struct Vertex vert;
	vert.m_pos = vec3;
	vert.m_tex = vec2;
	return vert;
}

struct Quad Quad_Create(float x, float y, float x2, float y2, float x3,
		float y3, float x4, float y4)
{
	struct Quad quad;
	quad.v1 = Vector2_Create(x, y);
	quad.v2 = Vector2_Create(x2, y2);
	quad.v3 = Vector2_Create(x3, y3);
	quad.v4 = Vector2_Create(x4, y4);
	return quad;
}

// Used to calculate FPS
long int Time_elapsed(struct timeval Start, struct timeval End)
{
	struct timeval elapsed;
	elapsed.tv_sec = End.tv_sec - Start.tv_sec;
	if(End.tv_usec >= Start.tv_usec)
		elapsed.tv_usec = End.tv_usec - Start.tv_usec;
	else {
		elapsed.tv_sec--;
		elapsed.tv_usec = 1000000 + End.tv_usec - Start.tv_usec;
	}
	return elapsed.tv_sec;
}
