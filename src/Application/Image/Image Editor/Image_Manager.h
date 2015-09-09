/*
 * Image_Manager.h
 *
 *  Created on: July 18, 2015
 *      Author: begah
 */

#ifndef IMAGE_MANAGER_H_
#define IMAGE_MANAGER_H_

#include "../Image_Editor.h"

struct Image_Tab
{
	struct Image_RawData *Image_Edit, *Image_Original;
	char *Name_Edit, *Name_Original;

	float BarY, BarValue;
	float X, Y;
};

struct Image_Editor
{
	GLuint Image_Texture;

	struct Vector4f Editor_Dimensions, Image_View;

	int State;
	struct Gui_Button *Confirm_Button, *Cancel_Button;

	struct Vector4f Hover_addColor;

	struct Gui_TextBox *Color_Red, *Color_Green, *Color_Blue, *Color_Alpha, *Name;

	struct Gui_Vertical_ScrollBar *Image_Scroll;

	struct Quad Change_Confirm, Close, Delete, New;
	_Bool Change_Confirm_Hover, Close_Hover, Delete_Hover, New_Hover;
	GLuint Change_Confirm_Texture, Close_Texture, Delete_Texture, New_Texture;

	struct Quad Image_Tab;

	struct Quad Button_Pencil, Button_PickColor, Button_Fill, Button_PanView, Keyboard_Show;
	GLuint Button_PencilTexture, Button_PickColorTexture, Button_FillTexture, Button_PanViewTexture;
	_Bool Button_PencilHover, Button_PickColorHover, Button_PencilOn, Button_PickColorOn, Button_FillHover, Button_FillOn, Button_PanViewHover, Button_PanViewOn;
	_Bool PanningView;
	float PanningView_OX, PanningView_OY;

	struct Quad Button_Save, Button_Load;
	GLuint Button_SaveTexture, Button_LoadTexture;
	_Bool Button_SaveHover, Button_LoadHover;

	struct Quad Image_Editor_View;
	struct Vector4f CurrentColor;

	unsigned char *ViewData;

	short ScrollingSpeed;

	float Image_Tab_Scroll;
	int Image_Tab_Index;
	struct vector_t *Image_Tab_List;
	_Bool Image_Tab_canScroll, Image_Tab_isScrolling;

	struct vector_t *Buttons, *TextBoxs;
};

unsigned int Image_createTexture(struct Image_RawData *Image);
struct Image_RawData *Image_loadPNG(const char *Name);

struct Image_Editor *Image_Editor_Init(float x, float y, float width, float height);
void Image_Editor_Render(struct Image_Editor *SE);
void Image_Editor_Free(struct Image_Editor **IE);

#endif /* IMAGE_MANAGER_H_ */
