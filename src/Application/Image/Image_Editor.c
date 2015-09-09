/*
 * Image_Editor.c
 *
 *  Created on: Sep 3, 2015
 *      Author: mathi
 */

#include "Image_Editor.h"

struct Image_Editor *IE;

void ImageEditor_Init()
{
	IE = Image_Editor_Init(0, 0, Game_Width, Game_Height);
}

void ImageEditor_Render()
{
	if(OnScreen_Keyboard) {
		IE->Editor_Dimensions = Vector4_Create(0, Game_Height / 20.0f * 9.0f, Game_Width, Game_Height / 20.0f * 11.0f);
	} else
		IE->Editor_Dimensions = Vector4_Create(0, 0, Game_Width, Game_Height);
	Image_Editor_Render(IE);
}

void ImageEditor_Close()
{
	Image_Editor_Free(&IE);
}
