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

struct Vector5f Vector5_Create(float x, float y, float z, float w, float h)
{
	struct Vector5f vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.w = w;
	vec.h = h;
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

struct Vector7f Vector7_Createf(float x, float y, float z, float r, float g, float b, float a)
{
    struct Vector7f vec;
	vec.x = x;
	vec.y = y;
	vec.z = z;
	vec.r = r;
	vec.g = g;
	vec.b = b;
	vec.a = a;
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

struct Vector2f Vector2_Normalize(struct Vector2f Vector)
{
    int X = Vector.x < 0 ? -1 : 1, Y = Vector.y < 0 ? -1 : 1;
    if(Vector.x < 0)
        Vector.x *= -1;
    if(Vector.y < 0)
        Vector.y *= -1;

    if(Vector.x >= Vector.y)
        return Vector2_Create(X, Vector.y / Vector.x * Y);
    else
        return Vector2_Create(Vector.x / Vector.y * X, Y);
};

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


double cosLK(int Degrees) // Cosinus function, uses a lookup table
{
    static double const CosLookUp[] = { 1.00000, 0.99985, 0.99939, 0.99863, 0.99756, 0.99619, 0.99452, 0.99255,
        0.99027, 0.98769, 0.98481, 0.98163, 0.97815, 0.97437, 0.97030, 0.96593, 0.96126, 0.95630, 0.95106,
        0.94552, 0.93969, 0.93358, 0.92718, 0.92050, 0.91355, 0.90631, 0.89879, 0.89101, 0.88295, 0.87462,
        0.86603, 0.85717, 0.84805, 0.83867, 0.82904, 0.81915, 0.80902, 0.79864, 0.78801, 0.77715, 0.76604,
        0.75471, 0.74314, 0.73135, 0.71934, 0.70711, 0.69466, 0.68200, 0.66913, 0.65606, 0.64279, 0.62932,
        0.61566, 0.60182, 0.58779, 0.57358, 0.55919, 0.54464, 0.52992, 0.51504, 0.50000, 0.48481, 0.46947,
        0.45399, 0.43837, 0.42262, 0.40674, 0.39073, 0.37461, 0.35837, 0.34202, 0.32557, 0.30902, 0.29237,
        0.27564, 0.25882, 0.24192, 0.22495, 0.20791, 0.19081, 0.17365, 0.15643, 0.13917, 0.12187, 0.10453,
        0.08716, 0.06976, 0.05234, 0.03490, 0.01745, 0.00000 };

    if(Degrees < 0)
        Degrees += 360;
    if(Degrees >= 360)
        Degrees -= 360;

    if(Degrees <= 90)
        return CosLookUp[Degrees];
    else if(Degrees <= 180)
        return -CosLookUp[-(Degrees - 180)];
    else if(Degrees <= 270)
        return -CosLookUp[(Degrees - 180)];
    else
        return CosLookUp[-(Degrees - 360)];
}

double sinLK(int Degrees) // Sinus function, uses a lookup table
{
    static double const SinLookUp[91] = { 0.00000, 0.01745, 0.03490, 0.05234, 0.06976, 0.08716, 0.10453, 0.12187,
        0.13917, 0.15643, 0.17365, 0.19081, 0.20791, 0.22495, 0.24192, 0.25882, 0.27564, 0.29237, 0.30902,
        0.32557, 0.34202, 0.35837, 0.37461, 0.39073, 0.40674, 0.42262, 0.43837, 0.45400, 0.46948, 0.48481,
        0.50000, 0.51504, 0.52992, 0.54464, 0.55920, 0.57358, 0.58779, 0.60182, 0.61566, 0.62932, 0.64279,
        0.65606, 0.66913, 0.68200, 0.69466, 0.70711, 0.71934, 0.73135, 0.74314, 0.75471, 0.76604, 0.77715,
        0.78801, 0.79864, 0.80902, 0.81915, 0.82904, 0.83867, 0.84805, 0.85717, 0.86603, 0.87462, 0.88295,
        0.89101, 0.89879, 0.90631, 0.91355, 0.92050, 0.92718, 0.93358, 0.93969, 0.94552, 0.95106, 0.95630,
        0.96126, 0.96593, 0.97030, 0.97437, 0.97815, 0.98163, 0.98481, 0.98769, 0.99027, 0.99255, 0.99452,
        0.99619, 0.99756, 0.99863, 0.99939, 0.99985, 1 };

    if(Degrees < 0)
        Degrees += 360;
    if(Degrees >= 360)
        Degrees -= 360;

    if(Degrees <= 90)
        return SinLookUp[Degrees];
    else if(Degrees <= 180)
        return SinLookUp[-(Degrees - 180)];
    else if(Degrees <= 270)
        return -SinLookUp[(Degrees - 180)];
    else
        return -SinLookUp[-(Degrees - 360)];
}
