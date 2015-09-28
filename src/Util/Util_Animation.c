/*
 * Util_Animation.c
 *
 *  Created on: Sep 19, 2015
 *      Author: mathi
 */

#include "Util.h"

double Double_PI = M_PI * 2.0;

struct Animation *Animation_LoadInternal(const char *Name)
{
	struct Animation *Animation = malloc(sizeof(struct Animation));

	int Names2Length = String_length(Name);
	char Names2[Names2Length + 1 + 4 + String_length(".png")];
	memcpy(Names2, Name, sizeof(char) * Names2Length);

#ifndef _WIN32
	Names2[Names2Length++] = '/';
#else
	Names2[Names2Length++] = '\\';
#endif

	Animation->Frame_Count = 0;

	while (1) {
		char *T = Integer_toString(Animation->Frame_Count);
		memcpy(Names2 + Names2Length, T, sizeof(char) * (String_length(T) + 1));
		String_Add(Names2, ".png");

		free(T);
		if (FileInternal_Exists(Names2))
			Animation->Frame_Count++;
		else
			break;
	}

	Animation->Frames = malloc(sizeof(struct Image) * Animation->Frame_Count);

	for (int i = 0; i < Animation->Frame_Count; i++) {
		char *T = Integer_toString(i);
		memcpy(Names2 + Names2Length, T, sizeof(char) * (String_length(T) + 1));
		String_Add(Names2, ".png");

		free(T);

		struct Image *Image = Image_Load(Names2);
		Animation->Frames[i] = *Image;
		free(Image);
	}

	Animation->Current_Frame = 0;
	Animation->Time_perFrame = 20; // 20 ms

	gettimeofday(&Animation->Current_FrameStart, NULL);

	Animation->x = Animation->y = 0;
	Animation->z = 0;

	Animation->width = Animation->height = 50;
	Animation->Angle = 0;

    Animation->Calculated_Quad = Quad_Create(Animation->x, Animation->y, Animation->x, Animation->y + Animation->height,
        Animation->x + Animation->width, Animation->y + Animation->height, Animation->x + Animation->width, Animation->y);

    Animation->Copied = false;

	return Animation;
}

struct Animation *Animation_LoadExternal(const char * restrict Name)
{
	struct Animation *Animation = malloc(sizeof(struct Animation));

	int Names2Length = String_length(Name);
	char Names2[Names2Length + 1 + 4 + String_length(".png")];
	memcpy(Names2, Name, sizeof(char) * Names2Length);

#ifndef _WIN32
	Names2[Names2Length++] = '/';
#else
	Names2[Names2Length++] = '\\';
#endif

    Animation->Frame_Count = 0;

	while (1) {
		char *T = Integer_toString(Animation->Frame_Count);
		memcpy(Names2 + Names2Length, T, sizeof(char) * (String_length(T) + 1));
		String_Add(Names2, ".png");

		free(T);
		if (FileExternal_Exists(Names2))
			Animation->Frame_Count++;
		else
			break;
	}

	Animation->Frames = malloc(sizeof(struct Image) * Animation->Frame_Count);
    struct Image *Image;
	for (int i = 0; i < Animation->Frame_Count; i++) {
		char *T = Integer_toString(i);
		memcpy(Names2 + Names2Length, T, sizeof(char) * (String_length(T) + 1));
		String_Add(Names2, ".png");

		free(T);

		Image = Image_LoadExternal(Names2);
		Animation->Frames[i] = *Image;

	}free(Image);

	Animation->Current_Frame = 0;
	Animation->Time_perFrame = 20; // 20 ms

	gettimeofday(&Animation->Current_FrameStart, NULL);

	Animation->x = Animation->y = 0;
	Animation->z = 0;

	Animation->width = Animation->height = 50;
	Animation->Angle = 0;

	Animation->Calculated_Quad = Quad_Create(Animation->x, Animation->y, Animation->x, Animation->y + Animation->height,
             Animation->x + Animation->width, Animation->y + Animation->height, Animation->x + Animation->width, Animation->y);

    Animation->Copied = false;

	return Animation;
}

void Animation_SetSize(struct Animation *Animation, float w, float h)
{
	Animation->width = w;
	Animation->height = h;

    Animation->Calculated_Quad = Quad_Create(Animation->x, Animation->y, Animation->x, Animation->y + Animation->height,
        Animation->x + Animation->width, Animation->y + Animation->height, Animation->x + Animation->width, Animation->y);
}

void Animation_SetPosition(struct Animation *Animation, float x, float y)
{
	Animation->x = x;
	Animation->y = y;

    Animation->Calculated_Quad = Quad_Create(Animation->x, Animation->y, Animation->x, Animation->y + Animation->height,
        Animation->x + Animation->width, Animation->y + Animation->height, Animation->x + Animation->width, Animation->y);
}

void Animation_SetAngle(struct Animation *Animation, double Angle)
{
    if(Angle >= Double_PI)
        Angle -= Double_PI;
    else if(Angle < 0)
        Angle += Double_PI;

    Animation->Angle = Angle;

    if(Angle == 0)
        Animation->Calculated_Quad = Quad_Create(Animation->x, Animation->y, Animation->x, Animation->y + Animation->height,
            Animation->x + Animation->width, Animation->y + Animation->height, Animation->x + Animation->width, Animation->y);
    else // Calculate angle
    {
        // Heavy on the CPU
        double centerX = Animation->x + Animation->width / 2.0,
            centerY = Animation->y + Animation->height / 2.0;

        double sA = sin(Angle);
        double cA = cos(Angle);

        double x = cA * (Animation->x - centerX) - sA * (Animation->y - centerY);
        double y = sA * (Animation->x - centerX) + cA * (Animation->y - centerY);
        double x2 = cA * (Animation->x - centerX) - sA * (Animation->y + Animation->height - centerY);
        double y2 = sA * (Animation->x - centerX) + cA * (Animation->y + Animation->height - centerY);
        double x3 = cA * (Animation->x + Animation->width - centerX) - sA * (Animation->y + Animation->height - centerY);
        double y3 = sA * (Animation->x + Animation->width - centerX) + cA * (Animation->y + Animation->height - centerY);
        double x4 = cA * (Animation->x + Animation->width - centerX) - sA * (Animation->y - centerY);
        double y4 = sA * (Animation->x + Animation->width - centerX) + cA * (Animation->y - centerY);

        Animation->Calculated_Quad = Quad_Create(x + centerX, y + centerY, x2 + centerX, y2 + centerY, x3 + centerX, y3 + centerY, x4 + centerX, y4 + centerY);
    }
}

// Toggle : If true the stop it from reversing, else enable reversion
void Animation_toggleReverseOnFinish(struct Animation *Animation)
{
    Animation->Copied = !Animation->Copied;
    if(Animation->Copied)
    {
        Animation->Frames = realloc(Animation->Frames, sizeof(struct Image) * Animation->Frame_Count * 2);
        for(int i = Animation->Frame_Count; i < Animation->Frame_Count * 2; i++)
        {
            Animation->Frames[i] = Animation->Frames[Animation->Frame_Count * 2 - i - 1];
        }
        Animation->Frame_Count *= 2;
    } else {
        Animation->Frame_Count /= 2;
        Animation->Frames = realloc(Animation->Frames, sizeof(struct Image) * Animation->Frame_Count);
    }
}

void Animation_Render(struct Animation *Animation)
{
	struct timeval Current;
	gettimeofday(&Current, NULL);

	long int Curr = Current.tv_usec + Current.tv_sec * 1000000,
			Last = Animation->Current_FrameStart.tv_usec + Animation->Current_FrameStart.tv_sec * 1000000;
	Curr /= 1000;
	Last /= 1000;

	if(Curr - Last >= Animation->Time_perFrame)
	{
		Animation->Current_FrameStart = Current;
		Animation->Current_Frame++;
		if(Animation->Current_Frame >= Animation->Frame_Count)
			Animation->Current_Frame = 0;
	}

    Image_Shader.pushQuad(Animation->Calculated_Quad,
                          Quad_Create(Animation->Frames[Animation->Current_Frame].x, Animation->Frames[Animation->Current_Frame].y2, Animation->Frames[Animation->Current_Frame].x,
                                      Animation->Frames[Animation->Current_Frame].y, Animation->Frames[Animation->Current_Frame].x2, Animation->Frames[Animation->Current_Frame].y,
                                      Animation->Frames[Animation->Current_Frame].x2, Animation->Frames[Animation->Current_Frame].y2), Animation->Frames[Animation->Current_Frame].Image, Vector4_Create(0, 0, 0, 0), Animation->z);

}

void Animation_Free(struct Animation *Animation)
{
    // If Animation has been reversed on finish, half of the pointers will be copies of the first half
    int ToDelete = Animation->Copied ? Animation->Frame_Count / 2 : Animation->Frame_Count;
	for(int i = 0; i < ToDelete; i++)
	{
		Image_FreeSimple(Animation->Frames + i);
	}

	free(Animation);
}

void Animation_FreeSimple(struct Animation *Animation)
{
    // If Animation has been reversed on finish, half of the pointers will be copies of the first half
    int ToDelete = Animation->Copied ? Animation->Frame_Count / 2 : Animation->Frame_Count;
	for(int i = 0; i < ToDelete; i++)
	{
		Image_FreeSimple(Animation->Frames + i);
	}
}
